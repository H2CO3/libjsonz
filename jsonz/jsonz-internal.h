/**
 * jsonz/jsonz-internal.h
 * libjsonz
 *
 * Created by Árpád Goretity on 28/11/2011.
 * Licensed under a CreativeCommons Attribution 3.0 Unported License
**/

#ifndef __JSONZ_JSONZ_INTERNAL_H__
#define __JSONZ_JSONZ_INTERNAL_H__

#define sentinel __attribute__((sentinel))

#include <jsonz/jsonz.h>


jsonz_result_t *jsonz_result_new();
jsonz_result_t *jsonz_result_set_root_type(jsonz_result_t *result, jsonz_type_t type);
jsonz_result_t *jsonz_result_add(jsonz_result_t *result, int position, int length, jsonz_type_t type);

void jsonz_object_free(void *obj);
void *jsonz_object_recursive_parse(const char *json, int length);

sentinel char *jsonz_string_concat(const char *str, ...);
char *jsonz_strndup(const char *str, size_t num);


#endif /* !__JSONZ_JSONZ_INTERNAL_H__ */
