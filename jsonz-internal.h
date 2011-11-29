/*
  jsonz-internal.h
  libjsonz
  
  Created by Árpád Goretity on 28/11/2011.
  Licensed under a CreativeCommons Attribution 3.0 Unported License
*/

#ifndef __JSONZ_INTERNAL_H__
#define __JSONZ_INTERNAL_H__

#include <jsonz.h>


jsonz_result_t *jsonz_result_new();
jsonz_result_t *jsonz_result_set_root_type(jsonz_result_t *result, jsonz_type_t type);
jsonz_result_t *jsonz_result_add(jsonz_result_t *result, int position, int length, jsonz_type_t type);


#endif /* !__JSONZ_INTERNAL_H__ */

