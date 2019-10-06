#include "obj.h"
#include <assert.h>

type_t obj_typeof(obj_t * obj) {
	return obj->head.type ;
}

bool obj_isvalue(obj_t * obj) {
	assert(obj) ;
	return obj_typeof(obj) == T_NUM ||
		obj_typeof(obj) == T_BOOL ||
		obj_typeof(obj) == T_PRIM ||
		obj_typeof(obj) == T_IDENT ;
}

bool obj_isframe(obj_t * obj) {
	assert(obj) ;
	return obj_typeof(obj) == T_FRIF ||
		obj_typeof(obj) == T_FRAPP ;
}

/* void obj_repr(obj_t * obj, char * buf, int buflen) { */
/* 	assert(buf) ; */
/* 	switch(obj_typeof(obj)) { */
/* 		case T_NUM: */
/* 			sprintf(buf, "%g", ((num_t *)obj)->value) ; */
/* 			break ; */
/* 		case T_BOOL: */
/* 			if (((bool_t *)obj)->value) { */
/* 				sprintf(buf, "True") ; */
/* 			} */
/* 			else { */
/* 				sprintf(buf, "False") ; */
/* 			} */
/* 			break ; */
			
/* 	} */
/* } */

void (*D_obj(obj_t * obj))(obj_t **) {
	assert(obj) ;
	return obj->head.D_obj ;
}

obj_t * C_obj_copy(obj_t * obj) {
	assert(obj) ;
	return obj->head.C_obj_copy(obj) ;
}

