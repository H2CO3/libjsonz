/**
 * jsonzprettify.c
 * libjsonz
 *
 * Created by Árpád Goretity on 29/11/2011.
 * Licensed under a CreativeCommons Attribution 3.0 Unported License
**/

#include <stdio.h>
#include <jsonz/jsonz.h>

void recursive_dump(const char *json, int length, int padding_n, int needs_separator, int recursion_level);

/* 16 * 4 spaces */
static const char *padding_s = "                                                                ";

void recursive_dump(const char *json, int length, int padding_n, int needs_separator, int recursion_level)
{
	if (recursion_level > 15)
	{
		/*
		 * nested too deep 
		 */
		printf("ERROR: nested too deep\n");
		exit(1);
	}
	jsonz_result_t *buf = jsonz_parse(json, length);
	jsonz_type_t root = jsonz_result_get_root_type(buf);
	const char *padding_str = padding_s + strlen(padding_s) - padding_n * 4;
	char separator = needs_separator ? ',' : '\0';
	switch (root)
	{
	case jsonz_type_array:
		printf("[\n");
		break;
	case jsonz_type_object:
		printf("{\n");
		break;
	default:
		break;
	}
	int i;
	for (i = 0; i < jsonz_result_get_count(buf); i++)
	{
		int pos = jsonz_result_get_position(buf, i);
		int len = jsonz_result_get_length(buf, i);
		jsonz_type_t type = jsonz_result_get_type(buf, i);
		char comma = i < jsonz_result_get_count(buf) - 1 ? ',' : '\0';
		switch (type)
		{
		case jsonz_type_null:
			{
				printf("%snull%c\n", root == jsonz_type_object && (i % 2) ? "" : padding_str - 4, comma);
				break;
			}
		case jsonz_type_bool:
			{
				char b[6];
				strncpy(b, json + pos, len);
				b[len] = 0;
				printf("%s%s%c\n", root == jsonz_type_object && (i % 2) ? "" : padding_str - 4, b, comma);
				break;
			}
		case jsonz_type_number:
			{
				char ns[24];
				strncpy(ns, json + pos, len);
				ns[len] = 0;
				printf("%s%s%c\n", root == jsonz_type_object && (i % 2) ? "" : padding_str - 4, ns, comma);
				break;
			}
		case jsonz_type_string:
			{
				char s[100];
				strncpy(s, json + pos, len);
				s[len] = 0;
				switch (root)
				{
				case jsonz_type_array:
					printf("%s\"%s\"%c\n", padding_str - 4, s, comma);
					break;
				case jsonz_type_object:
					if (i % 2)
					{
						/*
						 * value 
						 */
						printf("\"%s\"%c\n", s, comma);
					}
					else
					{
						/*
						 * key 
						 */
						printf("%s\"%s\": ", padding_str - 4, s);
					}
					break;
				default:
					break;
				}
				break;
			}
		case jsonz_type_array:
			{
				if (root == jsonz_type_array)
				{
					/*
					 * printf("    "); 
					 */
					printf("%s", padding_str - 4);
				}
				recursive_dump(json + pos, len, padding_n + 1, comma == ',', recursion_level + 1);
				break;
			}
		case jsonz_type_object:
			{
				if (root == jsonz_type_array)
				{
					/*
					 * printf("    "); 
					 */
					printf("%s", padding_str - 4);
				}
				recursive_dump(json + pos, len, padding_n + 1, comma == ',', recursion_level + 1);
				break;
			}
		default:
			{
				break;
			}
		}
	}
	if (buf)
	{
		if (root == jsonz_type_object)
		{
			printf("%s}%c\n", padding_str, separator);
		}
		else
		{
			printf("%s]%c\n", padding_str, separator);
		}
	}
	jsonz_result_free(buf);
}

int main(int argc, char **argv)
{
	const char *json = argv[1];
	recursive_dump(json, -1, 0, 0, 0);
	return 0;
}

