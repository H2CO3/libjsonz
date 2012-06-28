/**
 * jsonz/jsonz.h
 * libjsonz
 *
 * Created by Árpád Goretity on 28/11/2011.
 * Licensed under a CreativeCommons Attribution 3.0 Unported License
 */


#ifndef __JSONZ_JSONZ_H__
#define __JSONZ_JSONZ_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <jsonz/jsonz-types.h>
#include <jsonz/jsonz-object.h>


typedef struct {
	int count;
	jsonz_type_t root_type;
	int *positions;
	int *lengths;
	jsonz_type_t *types;
} jsonz_result_t;

void *jsonz_object_parse(const char *json);
char *jsonz_object_build(void *obj);

jsonz_result_t *jsonz_parse(const char *json, int len);
void jsonz_result_free(jsonz_result_t *result);
jsonz_type_t jsonz_result_get_root_type(jsonz_result_t *result);
int jsonz_result_get_position(jsonz_result_t *result, int idx);
int jsonz_result_get_length(jsonz_result_t *result, int idx);
jsonz_type_t jsonz_result_get_type(jsonz_result_t *result, int idx);
int jsonz_result_get_count(jsonz_result_t *result);


#endif /* !__JSONZ_JSONZ_H__ */
