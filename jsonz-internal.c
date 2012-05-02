/**
 * jsonz-internal.c
 * libjsonz
 *
 * Created by Árpád Goretity on 28/11/2011.
 * Licensed under a CreativeCommons Attribution 3.0 Unported License
**/

#include <jsonz/jsonz-internal.h>
#include <jsonz/jsonz-object.h>
#include <stdarg.h>
#include <string.h>

/**
 * Result API internal helpers
**/

jsonz_result_t *jsonz_result_new()
{
	jsonz_result_t *buf = malloc(sizeof(*buf));
	if (!buf)
	{
		return NULL;
	}
	buf->count = 0;
	buf->root_type = jsonz_type_null;	/* should be set */
	buf->positions = NULL;
	buf->lengths = NULL;
	buf->types = NULL;
	return buf;
}

jsonz_result_t *jsonz_result_set_root_type(jsonz_result_t *result, jsonz_type_t type)
{
	if (!result)
	{
		return NULL;
	}
	result->root_type = type;
	return result;
}

jsonz_result_t *jsonz_result_add(jsonz_result_t *result, int position, int length, jsonz_type_t type)
{
	if (!result)
	{
		return NULL;
	}
	int idx = result->count++;
	result->positions = realloc(result->positions, result->count * sizeof(*result->positions));
	result->lengths = realloc(result->lengths, result->count * sizeof(*result->lengths));
	result->types = realloc(result->types, result->count * sizeof(*result->types));
	result->positions[idx] = position;
	result->lengths[idx] = length;
	result->types[idx] = type;
	return result;
}

/**
 * Object API internal helpers
**/

void jsonz_object_free(void *obj)
{
	if (!obj)
	{
		return;
	}
	jsonz_type_t type = ((jsonz_object_stub_t *)obj)->type;
	if (type == jsonz_type_bool || type == jsonz_type_number)
	{
		free(obj);
	}
	else if (type == jsonz_type_string)
	{
		if (((jsonz_object_string_t *)obj)->value)
		{
			free(((jsonz_object_string_t *)obj)->value);
		}
		free(obj);
	}
	else if (type == jsonz_type_array)
	{
		/**
		 * recursion
		**/
		int count = ((jsonz_object_array_t *)obj)->length;
		void **children = ((jsonz_object_array_t *)obj)->children;
		int i;
		for (i = 0; i < count; i++)
		{
			jsonz_object_release(children[i]);
		}
		free(children);
		free(obj);
	}
	else if (type == jsonz_type_object)
	{
		/**
		 * recursion
		**/
		int count = ((jsonz_object_object_t *)obj)->length;
		void **children = ((jsonz_object_object_t *)obj)->children;
		char **keys = ((jsonz_object_object_t *)obj)->keys;
		int i;
		for (i = 0; i < count; i++)
		{
			jsonz_object_release(children[i]);
			free(keys[i]);
		}
		free(children);
		free(keys);
		free(obj);
	}
}

void *jsonz_object_recursive_parse(const char *json, int length)
{
	jsonz_result_t *buf = jsonz_parse(json, length);
	jsonz_type_t root_type = jsonz_result_get_root_type(buf);
	void *obj = NULL;
	if (root_type == jsonz_type_array)
	{
		obj = jsonz_object_new(root_type);
	}
	else if (root_type == jsonz_type_object)
	{
		obj = jsonz_object_new(root_type);
	}
	else
	{
		/**
		 * this shouldn't be any other type 
		**/
		jsonz_result_free(buf);
		return NULL;
	}
	int count = jsonz_result_get_count(buf);
	char *key = NULL;
	int i;
	for (i = 0; i < count; i++)
	{
		jsonz_type_t type = jsonz_result_get_type(buf, i);
		int pos = jsonz_result_get_position(buf, i);
		int len = jsonz_result_get_length(buf, i);
		if ((root_type == jsonz_type_object) && !(i % 2))
		{
			key = jsonz_strndup(json + pos, len);
			continue;
		}
		if (type == jsonz_type_null)
		{
			jsonz_object_null_t *null = jsonz_object_new(type);
			if (root_type == jsonz_type_array)
			{
				jsonz_object_array_add_element(obj, null, -1);
			}
			else if (root_type == jsonz_type_object)
			{
				jsonz_object_object_set_element(obj, key, null);
			}
			jsonz_object_release(null);
		}
		else if (type == jsonz_type_bool)
		{
			jsonz_object_number_t *num = jsonz_object_new(type);
			if (!strncmp(json + pos, "true", 4))
			{
				jsonz_object_number_set_bool_value(num, 1);
			}
			else if (!strncmp(json + pos, "false", 5))
			{
				jsonz_object_number_set_bool_value(num, 0);
			}
			if (root_type == jsonz_type_array)
			{
				jsonz_object_array_add_element(obj, num, -1);
			}
			else if (root_type == jsonz_type_object)
			{
				jsonz_object_object_set_element(obj, key, num);
			}
			jsonz_object_release(num);
		}
		else if (type == jsonz_type_number)
		{
			char *nstr = jsonz_strndup(json + pos, len);
			double n = strtod(nstr, NULL);
			free(nstr);
			jsonz_object_number_t *num = jsonz_object_new(type);
			jsonz_object_number_set_num_value(num, n);
			if (root_type == jsonz_type_array)
			{
				jsonz_object_array_add_element(obj, num, -1);
			}
			else if (root_type == jsonz_type_object)
			{
				jsonz_object_object_set_element(obj, key, num);
			}
			jsonz_object_release(num);
		}
		else if (type == jsonz_type_string)
		{
			jsonz_object_string_t *str = jsonz_object_new(type);
			char *ch_str = jsonz_strndup(json + pos, len);
			jsonz_object_string_set_str(str, ch_str);
			free(ch_str);
			if (root_type == jsonz_type_array)
			{
				jsonz_object_array_add_element(obj, str, -1);
			}
			else if (root_type == jsonz_type_object)
			{
				jsonz_object_object_set_element(obj, key, str);
			}
			jsonz_object_release(str);
		}
		else if (type == jsonz_type_array)
		{
			jsonz_object_array_t *other_obj = jsonz_object_recursive_parse(json + pos, len);
			if (root_type == jsonz_type_array)
			{
				jsonz_object_array_add_element(obj, other_obj, -1);
			}
			else if (root_type == jsonz_type_object)
			{
				jsonz_object_object_set_element(obj, key, other_obj);
			}
			jsonz_object_release(other_obj);
		}
		else if (type == jsonz_type_object)
		{
			jsonz_object_object_t *other_obj = jsonz_object_recursive_parse(json + pos, len);
			if (root_type == jsonz_type_array)
			{
				jsonz_object_array_add_element(obj, other_obj, -1);
			}
			else if (root_type == jsonz_type_object)
			{
				jsonz_object_object_set_element(obj, key, other_obj);
			}
			jsonz_object_release(other_obj);
		}
		free(key);
	}
	jsonz_result_free(buf);
	return obj;
}


/**
 * Miscellaneous helper functions
**/

sentinel char *jsonz_string_concat(const char *str, ...)
{
	if (!str)
	{
		return NULL;
	}
	char *base = strdup(str);
	int len_result = strlen(base);
	va_list args;
	va_start(args, str);
	while (1)
	{
		const char *addend = va_arg(args, const char *);
		if (!addend)
		{
			/**
			 * found sentinel
			**/
			break;
		}
		int len_addend = strlen(addend);
		len_result += len_addend + 1;
		char *tmp = realloc(base, len_result);
		if (!tmp)
		{
			free(base);
			va_end(args);
			return NULL;
		}
		base = tmp;
		strcat(base, addend);
	}
	va_end(args);
	return base;
}

char *jsonz_strndup(const char *str, size_t num)
{
	if (!str)
	{
		return NULL;
	}
	char *res = malloc(num + 1);
	memcpy(res, str, num);
	res[num] = '\0';
	return res;
}
