#ifndef OBJ_H
#define OBJ_H

#include <stdbool.h>

#define ALLOC_OR_RETNULL(id, type) type * id = (type *)malloc(sizeof(type)) ; \
					if(!id) return NULL

typedef enum _type {
	T_EXP,
	T_APP,
	T_IF,
	T_VAL,
	T_BOOL,
	T_PRIM,
	T_NUM,
	T_IDENT,
	T_LAM,
	T_STACK,
	T_FRIF,
	T_FRAPP,
	T_FRRET,
	T_ENV,
	T_CLO,
} type_t ;

struct _obj ;
typedef struct _header {
	type_t type;	
	void (*D_obj)(struct _obj **) ;
	struct _obj * (*C_obj_copy)(struct _obj *) ;
} header_t ;

#define HEADER_INIT(_type, dtor_id, ctor_copy_id) (header_t) { \
	.type = _type,  \
	.D_obj = dtor_id, \
	.C_obj_copy = ctor_copy_id }

typedef struct _obj {
	header_t head ;
} obj_t ;

type_t obj_typeof(obj_t * obj) ;

void obj_repr(obj_t * obj, char * buf, int buflen) ;

bool obj_isvalue(obj_t * obj) ;

bool obj_isexpr(obj_t * obj) ;

bool obj_isframe(obj_t * obj) ;

void (*D_obj(obj_t * obj))(obj_t **) ;
#define D_OBJ(obj) ({ if(obj) { D_obj(obj)(&obj) ; } })

obj_t * C_obj_copy(obj_t * obj) ;

#endif /* !OBJ_H */
