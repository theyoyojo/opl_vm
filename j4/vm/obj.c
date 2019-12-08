#include "obj.h"
#include <assert.h>

type_t obj_typeof(obj_t * obj) {
	return obj->head.type ;
}

size_t obj_sizeof(obj_t * obj) {
	return obj->head.size ;
}

bool obj_isvalue(obj_t * obj) {
	assert(obj) ;
	return obj_typeof(obj) == T_NUM ||
		obj_typeof(obj) == T_BOOL ||
		obj_typeof(obj) == T_PRIM ||
		obj_typeof(obj) == T_UNIT ||
		obj_typeof(obj) == T_PAIR ||
		obj_typeof(obj) == T_PTR ||
		obj_typeof(obj) == T_LAM ||
		obj_typeof(obj) == T_CLO ; /* I suppose a closure is a value */
}

bool obj_isexpr(obj_t * obj) {
	assert(obj) ;
	return obj_isvalue(obj) ||
		obj_typeof(obj) == T_APP ||
		obj_typeof(obj) == T_IF ||
		obj_typeof(obj) == T_IDENT ;
}

bool obj_isframe(obj_t * obj) {
	assert(obj) ;
	return obj_typeof(obj) == T_FRIF ||
		obj_typeof(obj) == T_FRAPP ||
		obj_typeof(obj) == T_FRRET ;
}

void (*D_obj(obj_t * obj))(obj_t **) {
	assert(obj) ;
	return obj->head.D_obj ;
}

obj_t * C_obj_copy(obj_t * obj) {
	assert(obj) ;
	return obj->head.C_obj_copy(obj) ;
}

