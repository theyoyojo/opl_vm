#include "types.h"
#include <string.h>

#define ALLOC_OR_RETNULL(id, type) type * id = (type *)malloc(sizeof(type)) ; \
					if(!id) return NULL
app_t * C_app(size_t count, ...) {
	ALLOC_OR_RETNULL(new, app_t) ;

	va_list arglist ;
	
	new->head = HEADER_INIT(T_APP, D_app) ;
	new->expr_list = olist_init() ;
	va_start(arglist, count) ;
	for (size_t i = 0; i < count; ++i) {
		olist_append(new->expr_list, va_arg(arglist, obj_t *)) ;
	}
	va_end(arglist) ;

	return new ;

}
void D_app(obj_t ** app_ptr) {
	olist_free(&(*(app_t **)app_ptr)->expr_list) ;
	free(*app_ptr) ;
	*app_ptr = NULL ;
}

if_t * C_if(obj_t * e_pred, obj_t * e_true, obj_t * e_false) {
	ALLOC_OR_RETNULL(new, if_t) ;
	*new = IF_INIT(e_pred, e_true, e_false) ;
	return new ;
}

void D_if(obj_t ** if_ptr) {
	if_t * ifexpr = *(if_t **)if_ptr ;
	ifexpr->expr_pred->head.D_func(&ifexpr->expr_pred) ;
	ifexpr->expr_true->head.D_func(&ifexpr->expr_true) ;
	ifexpr->expr_false->head.D_func(&ifexpr->expr_false) ;
	free(ifexpr) ;
	*if_ptr = NULL ;
}

char * prim_syms[] = {
	"+",
	"*",
	"/",
	"-",
	"<=",
	"<",
	"=",
	">",
	">="
};

#define PRIM_SYMS_LENGTH sizeof(prim_syms)/8

prim_val_t prim_stov(char * prim) {
	size_t i ;
	for (i = 0; i < PRIM_SYMS_LENGTH; ++i) {
		if (strcmp(prim, prim_syms[i]) == 0) {
			return (prim_val_t)i ;
		}
	}
	return PRIM_INVALID ;
}

char * prim_vtos(prim_val_t prim_val) {
	if (prim_val != PRIM_INVALID) {
		return prim_syms[prim_val] ;
	}
	else {
		return "[?]" ;
	}
}

prim_t * C_prim(char * prim) {
	ALLOC_OR_RETNULL(new, prim_t) ;
	*new = PRIM_INIT(prim) ;
	return new ;
}
void D_prim(obj_t ** prim_ptr) {
	free(*prim_ptr) ;
	*prim_ptr = NULL ;
}

bool_t * C_bool(bool value) {
	ALLOC_OR_RETNULL(new, bool_t) ;
	*new = BOOL_INIT(value) ;
	return new ;
}

void D_bool(obj_t ** bool_ptr) {
	free(*bool_ptr) ;
	*bool_ptr = NULL ;
}

num_t * C_num(double value) {
	ALLOC_OR_RETNULL(new, num_t) ;
	*new = NUM_INIT(value) ;
	return new ;
}

void D_num(obj_t ** num_ptr) {
	free(*num_ptr) ;
	*num_ptr = NULL ;
}

