#ifndef OBJ_H
#define OBJ_H

#define ALLOC_OR_RETNULL(id, type) type * id = (type *)malloc(sizeof(type)) ; \
					if(!id) return NULL

typedef enum _type {
	T_EXP,
	T_APP,
	T_IF,
	T_VAL,
	T_BOOL,
	T_PRIM,
	T_NUM
} type_t ;

struct _obj ;
typedef struct _header {
	type_t type;	
	void (*D_func)(struct _obj **) ;
} header_t ;

#define HEADER_INIT(_type, dtor_id) (header_t) { \
	.type = _type,  \
	.D_func = dtor_id }

typedef struct _obj {
	header_t head ;
} obj_t ;

#endif /* !OBJ_H */
