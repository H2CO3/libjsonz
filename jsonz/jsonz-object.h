/*
  jsonz/jsonz-object.h
  libjsonz
  
  Created by Árpád Goretity on 29/12/2011.
  Licensed under a CreativeCommons Attribution 3.0 Unported License
*/


#ifndef __JSONZ_JSONZ_OBJECT_H__
#define __JSONZ_JSONZ_OBJECT_H__

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <jsonz/jsonz-types.h>


typedef struct {
	jsonz_type_t type;
	int refcount;
} jsonz_object_stub_t;


typedef struct {
	jsonz_object_stub_t stub;
} jsonz_object_null_t;

typedef struct {
	jsonz_object_stub_t stub;
	union {
		double double_value;
		int int_value;
	};
} jsonz_object_number_t;

typedef struct {
	jsonz_object_stub_t stub;
	char *value;
} jsonz_object_string_t;

typedef struct {
	jsonz_object_stub_t stub;
	void **children;
	int length;
} jsonz_object_array_t;

typedef struct {
	jsonz_object_stub_t stub;
	char **keys;
	void **children;
	int length;
} jsonz_object_object_t;


void *jsonz_object_new(jsonz_type_t type);
void *jsonz_object_retain(void *obj);
void jsonz_object_release(void *obj);
void jsonz_object_free(void *obj);


jsonz_type_t jsonz_object_get_type(void *obj);


int jsonz_object_number_is_bool(jsonz_object_number_t *num);
int jsonz_object_number_get_bool_value(jsonz_object_number_t *num);
double jsonz_object_number_get_num_value(jsonz_object_number_t *num);
void jsonz_object_number_set_bool_value(jsonz_object_number_t *num, int value);
void jsonz_object_number_set_num_value(jsonz_object_number_t *num, double value);

char *jsonz_object_string_get_str(jsonz_object_string_t *string);
void jsonz_object_string_set_str(jsonz_object_string_t *string, char *str_new);

int jsonz_object_array_length(jsonz_object_array_t *arr);
void *jsonz_object_array_nth_element(jsonz_object_array_t *arr, int index);
void jsonz_object_array_add_element(jsonz_object_array_t *arr, void *element, int index);
void jsonz_object_array_delete_element(jsonz_object_array_t *arr, void *element);
void jsonz_object_array_delete_nth_element(jsonz_object_array_t *arr, int index);

int jsonz_object_object_length(jsonz_object_object_t *obj);
char *jsonz_object_object_nth_key(jsonz_object_object_t *obj, int index);
void *jsonz_object_object_get_element(jsonz_object_object_t *obj, char *key);
void jsonz_object_object_set_element(jsonz_object_object_t *obj, char *key, void *element);
void jsonz_object_object_delete_element(jsonz_object_object_t *obj, char *key);

#endif /* !__JSONZ_JSONZ_OBJECT_H__ */

