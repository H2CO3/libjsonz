/*
  jsonz/jsonz-types.h
  libjsonz
  
  Created by Árpád Goretity on 29/12/2011.
  Licensed under a CreativeCommons Attribution 3.0 Unported License
*/


#ifndef __JSONZ_JSONZ_TYPES_H__
#define __JSONZ_JSONZ_TYPES_H__

typedef enum {
	jsonz_type_null,
	jsonz_type_number,
	jsonz_type_bool,
	jsonz_type_string,
	jsonz_type_array,
	jsonz_type_object
} jsonz_type_t;

#endif /* !__JSONZ_JSONZ_TYPES_H__ */
