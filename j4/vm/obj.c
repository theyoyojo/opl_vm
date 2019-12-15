#include "obj.h"
#include <assert.h>
#include <stdlib.h>

type_t obj_typeof(obj_t * obj) {
	return obj->head.type ;
}

size_t obj_sizeof(obj_t * obj) {
	return obj->head.size ;
}

char * obj_repr(obj_t * obj) {
	if (!obj->head.repr) {
		obj->head.repr_gen(obj) ;
	}

	return obj->head.repr ;
}

size_t obj_repr_size(obj_t * obj) {
	return obj->head.repr ? obj->head.repr_size : 0UL ;
}

bool obj_isvalue(obj_t * obj) {
	assert(obj) ;
	return obj_typeof(obj) == T_NUM ||
		obj_typeof(obj) == T_BOOL ||
		obj_typeof(obj) == T_PRIM ||
		obj_typeof(obj) == T_UNIT ||
		obj_typeof(obj) == T_PAIR ||
		obj_typeof(obj) == T_PTR ||
		obj_typeof(obj) == T_STR ||
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
		obj_typeof(obj) == T_FRRET ||
		obj_typeof(obj) == T_FRFR ;
}

void D_obj_repr(obj_t * obj) {
	assert(obj) ;
	if (obj->head.repr) {
		free(obj->head.repr) ;
		obj->head.repr = NULL ;
	}
}

void (*D_obj(obj_t * obj))(obj_t **) {
	assert(obj) ;

	return obj->head.D_obj ;
}

obj_t * C_obj_copy(obj_t * obj) {
	assert(obj) ;
	return obj->head.C_obj_copy(obj) ;
}

