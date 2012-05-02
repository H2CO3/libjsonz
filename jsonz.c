/**
 * jsonz.c
 * libjsonz
 *
 * Created by Árpád Goretity on 28/11/2011.
 * Licensed under a CreativeCommons Attribution 3.0 Unported License
**/

#include <jsonz/jsonz.h>
#include <jsonz/jsonz-internal.h>


void *jsonz_object_parse(const char *json)
{
	void *obj = jsonz_object_recursive_parse(json, -1);
	return obj;
}

char *jsonz_object_build(void *obj)
{
	jsonz_type_t root_type = jsonz_object_get_type(obj);
	int length = 0;
	char *result = malloc(1);
	result[0] = '\0';
	char *buf = NULL;
	int i;

	if (root_type == jsonz_type_array)
	{
		length = jsonz_object_array_length(obj);
		buf = jsonz_string_concat(result, "[", NULL);
		free(result);
		result = buf;
	}
	else if (root_type == jsonz_type_object)
	{
		length = jsonz_object_object_length(obj);
		buf = jsonz_string_concat(result, "{", NULL);
		free(result);
		result = buf;
	}

	for (i = 0; i < length; i++)
	{
		if (i)
		{
			/**
			 * no separator before the first child
			**/
			buf = jsonz_string_concat(result, ", ", NULL);
			free(result);
			result = buf;
		}

		void *child = NULL;
		char *key = NULL;
		char *appendee = NULL;
		if (root_type == jsonz_type_array)
		{
			child = jsonz_object_array_nth_element(obj, i);
		}
		else if (root_type == jsonz_type_object)
		{
			key = jsonz_object_object_nth_key(obj, i);
			child = jsonz_object_object_get_element(obj, key);
		}

		jsonz_type_t type = jsonz_object_get_type(child);
		if (type == jsonz_type_null)
		{
			appendee = strdup("null");
		}
		else if (type == jsonz_type_bool)
		{
			char *app_tmp =
			    jsonz_object_number_get_bool_value(child) ?
			    "true" : "false";
			appendee = strdup(app_tmp);
		}
		else if (type == jsonz_type_number)
		{
			appendee = malloc(48);
			memset(appendee, 0, 48);
			double val = jsonz_object_number_get_num_value(child);
			sprintf(appendee, "%24.16lf", val);
		}
		else if (type == jsonz_type_string)
		{
			const char *str_tmp = jsonz_object_string_get_str(child);
			appendee = jsonz_string_concat("\"", str_tmp, "\"", NULL);
		}
		else if (type == jsonz_type_array)
		{
			appendee = jsonz_object_build(child);
		}
		else if (type == jsonz_type_object)
		{
			appendee = jsonz_object_build(child);
		}

		if (root_type == jsonz_type_array)
		{
			buf = jsonz_string_concat(result, appendee, NULL);
			free(result);
			result = buf;
		}
		else if (root_type == jsonz_type_object)
		{
			buf = jsonz_string_concat(result, "\"", key, "\"", ": ", appendee, NULL);
			free(result);
			result = buf;
		}
		free(appendee);
	}

	if (root_type == jsonz_type_array)
	{
		buf = jsonz_string_concat(result, "]", NULL);
		free(result);
		result = buf;
	}
	else if (root_type == jsonz_type_object)
	{
		buf = jsonz_string_concat(result, "}", NULL);
		free(result);
		result = buf;
	}

	return result;
}


void jsonz_result_free(jsonz_result_t *result)
{
	if (!result)
	{
		return;
	}
	if (result->positions)
	{
		free(result->positions);
	}
	if (result->lengths)
	{
		free(result->lengths);
	}
	if (result->types)
	{
		free(result->types);
	}
	free(result);
}

jsonz_type_t jsonz_result_get_root_type(jsonz_result_t *result)
{
	if (!result)
	{
		return jsonz_type_null;
	}
	return result->root_type;
}

int jsonz_result_get_position(jsonz_result_t *result, int idx)
{
	if (!result)
	{
		return 0;
	}
	if (idx >= result->count)
	{
		return 0;
	}
	return result->positions[idx];
}

int jsonz_result_get_length(jsonz_result_t *result, int idx)
{
	if (!result)
	{
		return 0;
	}
	if (idx >= result->count)
	{
		return 0;
	}
	return result->lengths[idx];
}

jsonz_type_t jsonz_result_get_type(jsonz_result_t *result, int idx)
{
	if (!result)
	{
		return jsonz_type_null;
	}
	if (idx >= result->count)
	{
		return jsonz_type_null;
	}
	return result->types[idx];
}

int jsonz_result_get_count(jsonz_result_t *result)
{
	if (!result)
	{
		return 0;
	}
	return result->count;
}

jsonz_result_t *jsonz_parse(const char *json, int len)
{
	if (!json)
	{
		return NULL;
	}
	int l = len < 0 ? strlen(json) : len;
	jsonz_result_t *result = jsonz_result_new();
	const char *c = json;
	int object_depth = 0;
	int array_depth = 0;
	int had_outmost_structured = 0;
	int position = 0;
	int length = 0;
	while (*c && (c - json) < l)
	{
		switch (*c)
		{
		case '"':
			if (!had_outmost_structured)
			{
				/**
				 * do not parse scalars
				**/
				jsonz_result_free(result);
				return NULL;
			}
			/**
			 * found a string: walk past it
			**/
			position = c - json + 1;
			/**
			 * avoid segfault by checking for *c being terminating NULL
			**/
			while (*++c != '"' && *c && (c - json) < l)
			{
				/**
				 * if it's an escape char, just walk past it,
				 * * as it potentially can be an escaped "
				**/
				if (*c == '\\')
				{
					c++;
				}
			}
			if (!*c)
			{
				/**
				 * End of string but string isn't closed
				 * fprintf(stderr, "libjsonz error: mismatched string\n");
				**/
				jsonz_result_free(result);
				return NULL;
			}
			if (c - json >= l)
			{
				/**
				 * fprintf(stderr, "libjsonz error: unexpected end of string\n"); 
				**/
				jsonz_result_free(result);
				return NULL;
			}
			length = c - json - position;
			jsonz_result_add(result, position, length, jsonz_type_string);
			break;
		case '{':
			/**
			 * if it's the root element, don't return it
			**/
			if (!had_outmost_structured)
			{
				had_outmost_structured = 1;
				jsonz_result_set_root_type(result, jsonz_type_object);
				break;
			}
			/**
			 * else, found an inner object, walk past it
			**/
			object_depth++;
			position = c - json;
			/**
			 * avoid segfault by checking for *c being terminating NULL
			**/
			while (object_depth && *c && (c - json < l))
			{
				c++;
				if (*c == '{')
				{
					object_depth++;
				}
				else if (*c == '}')
				{
					object_depth--;
				}
			}
			if (object_depth && !*c)
			{
				/**
				 * End of string but object isn't closed
				 * fprintf(stderr, "libjsonz error: mismatched object\n");
				**/
				jsonz_result_free(result);
				return NULL;
			}
			if (c - json >= l)
			{
				/**
				 * fprintf(stderr, "libjsonz error: unexpected end of string\n");
				**/
				jsonz_result_free(result);
				return NULL;
			}
			length = c - json - position + 1;
			jsonz_result_add(result, position, length, jsonz_type_object);
			break;
		case '[':
			/**
			 * if it's the root element, don't return it
			**/
			if (!had_outmost_structured)
			{
				had_outmost_structured = 1;
				jsonz_result_set_root_type(result,
							   jsonz_type_array);
				break;
			}
			/**
			 * else, found an inner array, walk past it
			**/
			array_depth++;
			position = c - json;
			/**
			 * avoid segfault by checking for *c being terminating NULL
			**/
			while (array_depth && *c && (c - json) < l)
			{
				c++;
				if (*c == '[')
				{
					array_depth++;
				}
				else if (*c == ']')
				{
					array_depth--;
				}
			}
			if (array_depth && !*c)
			{
				/**
				 * End of string but array isn't closed
				 * fprintf(stderr, "libjsonz error: mismatched array\n");
				**/
				jsonz_result_free(result);
				return NULL;
			}
			if (c - json >= l)
			{
				/**
				 * fprintf(stderr, "libjsonz error: unexpected end of string\n");
				**/
				jsonz_result_free(result);
				return NULL;
			}
			length = c - json - position + 1;
			jsonz_result_add(result, position, length, jsonz_type_array);
			break;
		case '+':
		case '-':
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (!had_outmost_structured)
			{
				/**
				 * do not parse scalars
				**/
				jsonz_result_free(result);
				return NULL;
			}
			/**
			 * found a number: go until we encounter a non-number character
			**/
			position = c - json;
			while (strchr("0123456789.eE+-", *c) && (c - json) < l)
			{
				c++;
			}
			if (c - json >= l)
			{
				/**
				 * fprintf(stderr, "libjsonz error: unexpected end of string\n");
				**/
				jsonz_result_free(result);
				return NULL;
			}
			length = c - json - position;
			jsonz_result_add(result, position, length, jsonz_type_number);
			break;
		case ':':
			if (!had_outmost_structured)
			{
				/**
				 * meaningless out of an object
				**/
				jsonz_result_free(result);
				return NULL;
			}
			/**
			 * if the root element is not an object, there should be no colons
			**/
			if (jsonz_result_get_root_type(result) != jsonz_type_object)
			{
				/*
				 * fprintf(stderr, "libjsonz error: found key separator in array\n");
				 */
				jsonz_result_free(result);
				return NULL;
			}
			break;
		case ',':
			if (!had_outmost_structured)
			{
				/**
				 * meaningless out of an array or object
				**/
				jsonz_result_free(result);
				return NULL;
			}
			/**
			 * there can be a comma in any structured data type,
			 * and we don't want error checking, so just ignore it
			**/
			break;
		case ' ':
		case '\n':
		case '\r':
		case '\t':
			/**
			 * skip any whitespace
			**/
			break;
		case 't':
			if (!had_outmost_structured)
			{
				/**
				 * do not parse scalars
				**/
				jsonz_result_free(result);
				return NULL;
			}
			/**
			 * should be true
			**/
			if (strncmp(c, "true", 4) == 0)
			{
				jsonz_result_add(result, c - json, 4, jsonz_type_bool);
				c += 4;
				break;
			}
			/**
			 * else: error
			 * fprintf(stderr, "libjsonz error: true expected\n");
			**/
			jsonz_result_free(result);
			return NULL;
			break;
		case 'f':
			if (!had_outmost_structured)
			{
				/*
				 * do not parse scalars
				 */
				jsonz_result_free(result);
				return NULL;
			}
			/**
			 * should be false
			**/
			if (strncmp(c, "false", 5) == 0)
			{
				jsonz_result_add(result, c - json, 5, jsonz_type_bool);
				c += 5;
				break;
			}
			/**
			 * else: error
			 * fprintf(stderr, "libjsonz error: false expected\n");
			**/
			jsonz_result_free(result);
			return NULL;
			break;
		case 'n':
			if (!had_outmost_structured)
			{
				/**
				 * do not parse scalars
				**/
				jsonz_result_free(result);
				return NULL;
			}
			/**
			 * should be null
			**/
			if (strncmp(c, "null", 4) == 0)
			{
				jsonz_result_add(result, c - json, 4, jsonz_type_null);
				c += 4;
				break;
			}
			/**
			 * else: error
			 * fprintf(stderr, "libjsonz error: null expected\n");
			**/
			jsonz_result_free(result);
			return NULL;
			break;
		case ']':
			/**
			 * handle termination of root element
			**/
			if (jsonz_result_get_root_type(result) != jsonz_type_array)
			{
				/**
				 * error
				 * fprintf(stderr, "libjsonz error: improper termination of root array\n");
				**/
				jsonz_result_free(result);
				return NULL;
			}
			break;
		case '}':
			/**
			 * handle termination of root element
			**/
			if (jsonz_result_get_root_type(result) != jsonz_type_object)
			{
				/**
				 * error
				 * fprintf(stderr, "libjsonz error: improper termination of root object\n");
				**/
				jsonz_result_free(result);
				return NULL;
			}
			break;
		default:
			/**
			 * there shouldn't be any other case
			 * fprintf(stderr, "libjsonz error: unexpected character: %c\n", *c);
			**/
			jsonz_result_free(result);
			return NULL;

			break;
		}
		c++;
	}
	return result;
}
