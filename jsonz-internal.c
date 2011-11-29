/*
  jsonz-internal.c
  libjsonz
  
  Created by Árpád Goretity on 28/11/2011.
  Licensed under a CreativeCommons Attribution 3.0 Unported License
*/

#include <jsonz-internal.h>


jsonz_result_t *jsonz_result_new() {
	jsonz_result_t *buf = malloc(sizeof(jsonz_result_t));
	if (!buf) {
		return NULL;
	}
	buf->count = 0;
	buf->root_type = jsonz_type_null; /* should be set */
	buf->positions = NULL;
	buf->lengths = NULL;
	buf->types = NULL;
	return buf;
}

jsonz_result_t *jsonz_result_set_root_type(jsonz_result_t *result, jsonz_type_t type) {
	if (!result) {
		return NULL;
	}
	result->root_type = type;
	return result;
}

jsonz_result_t *jsonz_result_add(jsonz_result_t *result, int position, int length, jsonz_type_t type) {
	if (!result) {
		return NULL;
	}
	int idx = result->count++;
	result->positions = realloc(result->positions, result->count * sizeof(int));
	result->lengths = realloc(result->lengths, result->count * sizeof(int));
	result->types = realloc(result->types, result->count * sizeof(jsonz_type_t));
	result->positions[idx] = position;
	result->lengths[idx] = length;
	result->types[idx] = type;
	return result;
}

