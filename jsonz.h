/*
  jsonz.h
  libjsonz
  
  Created by Árpád Goretity on 28/11/2011.
  Licensed under a CreativeCommons Attribution 3.0 Unported License
*/


#ifndef __JSONZ_H__
#define __JSONZ_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>


typedef enum {
	jsonz_type_null,
	jsonz_type_number,
	jsonz_type_bool,
	jsonz_type_string,
	jsonz_type_array,
	jsonz_type_object
} jsonz_type_t;

typedef struct {
	int count;
	jsonz_type_t root_type;
	int *positions;
	int *lengths;
	jsonz_type_t *types;
} jsonz_result_t;


jsonz_result_t *jsonz_parse(char *json, int len);
void jsonz_result_free(jsonz_result_t *result);
jsonz_type_t jsonz_result_get_root_type(jsonz_result_t *result);
int jsonz_result_get_position(jsonz_result_t *result, int idx);
int jsonz_result_get_length(jsonz_result_t *result, int idx);
jsonz_type_t jsonz_result_get_type(jsonz_result_t *result, int idx);
int jsonz_result_get_count(jsonz_result_t *result);


#endif /* !__JSONZ_H__ */

