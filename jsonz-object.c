/*
  jsonz/jsonz-object.c
  libjsonz
  
  Created by Árpád Goretity on 29/12/2011.
  Licensed under a CreativeCommons Attribution 3.0 Unported License
*/

#include <string.h>
#include <jsonz/jsonz-object.h>
#include <jsonz/jsonz-internal.h>

/*
  Memory management functions
*/

void *jsonz_object_new(jsonz_type_t type) {
	size_t size = 0;
	if (type == jsonz_type_null) {
		size = sizeof(jsonz_object_null_t);
	} else if (type == jsonz_type_number || type == jsonz_type_bool) {
		size = sizeof(jsonz_object_number_t);
	} else if (type == jsonz_type_string) {
		size = sizeof(jsonz_object_string_t);
	} else if (type == jsonz_type_array) {
		size = sizeof(jsonz_object_array_t);
	} else if (type == jsonz_type_object) {
		size = sizeof(jsonz_object_object_t);
	}
	if (!size) {
		return NULL;
	}
	void *obj = malloc(size);
	if (!obj) {
		return NULL;
	}
	memset(obj, 0, size);
	((jsonz_object_stub_t *)obj)->type = type;
	((jsonz_object_stub_t *)obj)->refcount = 1;
	return obj;
}

void *jsonz_object_retain(void *obj) {
	if (!obj) {
		return NULL;
	}
	((jsonz_object_stub_t *)obj)->refcount++;
	return obj;
}

void jsonz_object_release(void *obj) {
	if (!obj) {
		return;
	}
	((jsonz_object_stub_t *)obj)->refcount--;
	if (!((jsonz_object_stub_t *)obj)->refcount) {
		jsonz_object_free(obj);
	}
}

/*
  Querying object types
*/

jsonz_type_t jsonz_object_get_type(void *obj) {
	if (!obj) {
		return jsonz_type_null;
	}
	return ((jsonz_object_stub_t *)obj)->type;
}

/*
  Number and boolean functions
*/

int jsonz_object_number_is_bool(jsonz_object_number_t *num) {
	if (!num) {
		return 0;
	}
	return num->stub.type == jsonz_type_bool;
}

int jsonz_object_number_get_bool_value(jsonz_object_number_t *num) {
	if (!num) {
		return 0;
	}
	if (num->stub.type != jsonz_type_bool) {
		return 0;
	}
	return num->int_value;
}

double jsonz_object_number_get_num_value(jsonz_object_number_t *num) {
	if (!num) {
		return 0.0;
	}
	if (num->stub.type != jsonz_type_number) {
		return 0.0;
	}
	return num->double_value;
}

void jsonz_object_number_set_bool_value(jsonz_object_number_t *num, int value) {
	if (!num) {
		return;
	}
	if (num->stub.type != jsonz_type_bool) {
		return;
	}
	num->int_value = value;
}

void jsonz_object_number_set_num_value(jsonz_object_number_t *num, double value) {
	if (!num) {
		return;
	}
	if (num->stub.type != jsonz_type_number) {
		return;
	}
	num->double_value = value;
}

/*
  String functions
*/

char *jsonz_object_string_get_str(jsonz_object_string_t *string) {
	if (!string) {
		return NULL;
	}
	if (string->stub.type != jsonz_type_string) {
		return NULL;
	}
	return string->value;
}

void jsonz_object_string_set_str(jsonz_object_string_t *string, char *str_new) {
	if (!string || !str_new) {
		return;
	}
	if (string->stub.type != jsonz_type_string) {
		return;
	}
	char *tmp = strdup(str_new);
	if (!tmp) {
		return;
	}
	if (string->value) {
		free(string->value);
	}
	string->value = tmp;
}

/*
  Array functions
*/

int jsonz_object_array_length(jsonz_object_array_t *arr) {
	if (!arr) {
		return 0;
	}
	if (arr->stub.type != jsonz_type_array) {
		return 0;
	}
	return arr->length;
}

void *jsonz_object_array_nth_element(jsonz_object_array_t *arr, int index) {
	if (!arr) {
		return NULL;
	}
	if (arr->stub.type != jsonz_type_array) {
		return NULL;
	}
	if (index >= arr->length || index < 0) {
		return NULL;
	}
	return arr->children[index];
}

void jsonz_object_array_add_element(jsonz_object_array_t *arr, void *element, int index) {
	if (!arr || !element) {
		return;
	}
	if (arr->stub.type != jsonz_type_array) {
		return;
	}
	void *e = jsonz_object_retain(element);
	if (!e) {
		return;
	}
	if (index > arr->length) {
		return;
	}
	/* make place for the new element */
	arr->length++;
	void **c = realloc(arr->children, arr->length * sizeof(void *));
	if (!c) {
		arr->length--;
		return;
	}
	arr->children = c;
	int idx = index;
	if (idx < 0) {
		/* append to end */
		idx = arr->length - 1;
	}
	/* shift the elements with their index >= idx to the right */
	for (int i = arr->length - 1; i > idx; i--) {
		arr->children[i] = arr->children[i - 1];
	}
	arr->children[idx] = e;
}

void jsonz_object_array_delete_element(jsonz_object_array_t *arr, void *element) {
	if (!arr || !element) {
		return;
	}
	if (arr->stub.type != jsonz_type_array) {
		return;
	}
	int index = 0;
	while (index < arr->length) {
		/* delete every occurrence of the object */
		while (arr->children[index] == element) {
			jsonz_object_array_delete_nth_element(arr, index);
		}
		index++;
	}
}

void jsonz_object_array_delete_nth_element(jsonz_object_array_t *arr, int index) {
	if (!arr) {
		return;
	}
	if (arr->stub.type != jsonz_type_array) {
		return;
	}
	if (index >= arr->length || index < 0) {
		return;
	}

	/* release the deletee */
	jsonz_object_release(arr->children[index]);

	/* shift the elements with their index >= index to the left */
	for (int i = index; i < arr->length - 1; i++) {
		arr->children[i] = arr->children[i + 1];
	}
	/* then shrink the array */
	arr->length--;
	void **c = realloc(arr->children, arr->length * sizeof(void *));
	if (!c) {
		return;
	}
	arr->children = c;
}

/*
  Object functions (a. k. a. Associative Arrays)
*/

int jsonz_object_object_length(jsonz_object_object_t *obj) {
	if (!obj) {
		return 0;
	}
	if (obj->stub.type != jsonz_type_object) {
		return 0;
	}
	return obj->length;
}

char *jsonz_object_object_nth_key(jsonz_object_object_t *obj, int index) {
	if (!obj) {
		return NULL;
	}
	if (obj->stub.type != jsonz_type_object) {
		return NULL;
	}
	if (index >= obj->length || index < 0) {
		return NULL;
	}
	return obj->keys[index];
}

void *jsonz_object_object_get_element(jsonz_object_object_t *obj, char *key) {
	if (!obj || !key) {
		return NULL;
	}
	if (obj->stub.type != jsonz_type_object) {
		return NULL;
	}
	void *res = NULL;
	for (int i = 0; i < obj->length; i++) {
		if (!strcmp(obj->keys[i], key)) {
			res = obj->children[i];
			break;
		}
	}
	return res;
}

void jsonz_object_object_set_element(jsonz_object_object_t *obj, char *key, void *element) {
	if (!obj || !key || !element) {
		return;
	}
	if (obj->stub.type != jsonz_type_object) {
		return;
	}
	void *e = jsonz_object_retain(element);
	if (!e) {
		return;
	}
	/* if the object already contains this key, we release the old object, and retain the new one */
	for (int i = 0; i < obj->length; i++) {
		if (!strcmp(obj->keys[i], key)) {
			jsonz_object_release(obj->children[i]);
			obj->children[i] = e;
			return;
		}
	}
	/* if the key is not yet present, add it */
	obj->length++;
	char **k = realloc(obj->keys, obj->length * sizeof(char *));
	void **c = realloc(obj->children, obj->length * sizeof(void *));
	if (!k || !c) {
		obj->length--;
		return;
	}
	obj->keys = k;
	obj->children = c;
	char *key_new = strdup(key);
	if (!key_new) {
		obj->length--;
		return;
	}
	obj->keys[obj->length - 1] = key_new;
	obj->children[obj->length - 1] = e;
}

void jsonz_object_object_delete_element(jsonz_object_object_t *obj, char *key) {
	if (!obj || !key) {
		return;
	}
	if (obj->stub.type != jsonz_type_object) {
		return;
	}
	/* we only go until the first occurrence, as each key is unique in an object */
	for (int i = 0; i < obj->length; i++) {
		if (!strcmp(obj->keys[i], key)) {
			/* remove the key */
			free(obj->keys[i]);
			/* and the corresponding object */
			jsonz_object_release(obj->children[i]);
			/* and shift the remaining elements one to the left */
			for (int j = i; j < obj->length - 1; j++) {
				obj->keys[j] = obj->keys[j + 1];
				obj->children[j] = obj->children[j + 1];
			}
			/* and shrink the object */
			obj->length--;
			char **k = realloc(obj->keys, obj->length * sizeof(char *));
			void **c = realloc(obj->children, obj->length * sizeof(void *));
			if (!k || !c) {
				return;
			}
			obj->keys = k;
			obj->children = c;
			break;
		}
	}
}

