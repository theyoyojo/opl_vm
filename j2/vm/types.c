#include "types.h"
#include <string.h>

/* obj_t * (*C_copy_table[])(obj_t *) = { */
/* 	[T_NUM] = C_num_copy, */
/* 	[T_BOOL] = C_bool_copy, */
/* 	[T_PRIM] = C_prim_copy, */
/* 	[T_IF] = C_if_copy, */
/* 	[T_APP] = C_app_copy, */
/* } ; */

obj_t * C_app(size_t count, ...) {
	ALLOC_OR_RETNULL(new, app_t) ;

	va_list arglist ;
	
	new->head = HEADER_INIT(T_APP, D_app, C_app_copy) ;
	new->expr_list = olist_init() ;
	va_start(arglist, count) ;
	for (size_t i = 0; i < count; ++i) {
		olist_append(new->expr_list, va_arg(arglist, obj_t *)) ;
	}
	va_end(arglist) ;

	return (obj_t *)new ;

}

obj_t * C_app_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, app_t) ;

	new->head = HEADER_INIT(T_APP, D_app, C_app_copy) ;

	/* new->expr_list = olist_init() ; */

	/* olist_t * old_expr_list = ((app_t *)old)->expr_list ; */
	/* obj_t * current ; */
	/* for (size_t i = 0; i < olist_length(old_expr_list); ++i) { */
	/* 	current = olist_get(old_expr_list, i) ; */
	/* 	olist_append(new->expr_list, C_obj_copy(current)) ; */
	/* } */
	
	new->expr_list = olist_init_copy(((app_t *)old)->expr_list) ;

	return (obj_t *)new ;
}

void D_app(obj_t ** app_ptr) {
	olist_free(&(*(app_t **)app_ptr)->expr_list) ;
	free(*app_ptr) ;
	*app_ptr = NULL ;
}

obj_t * app_copy_first(obj_t * app) {
	assert(obj_typeof(app) == T_APP) ;
	return C_obj_copy(olist_get(((app_t *)app)->expr_list, 0)) ;
}

obj_t * C_if(obj_t * e_pred, obj_t * e_true, obj_t * e_false) {
	ALLOC_OR_RETNULL(new, if_t) ;
	*new = IF_INIT(e_pred, e_true, e_false) ;
	return (obj_t *)new ;
}

obj_t * C_if_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, if_t) ;
	*new = IF_INIT(
		C_obj_copy(((if_t *)old)->expr_pred),
		C_obj_copy(((if_t *)old)->expr_true),
		C_obj_copy(((if_t *)old)->expr_false)) ;
	return (obj_t *)new ;
}

void D_if(obj_t ** if_ptr) {
	if_t * ifexpr = *(if_t **)if_ptr ;
	D_OBJ(ifexpr->expr_pred) ;
	D_OBJ(ifexpr->expr_true) ;
	D_OBJ(ifexpr->expr_false) ;
	free(ifexpr) ;
	*if_ptr = NULL ;
}

obj_t * if_copy_pred(obj_t * ifexpr) {
	assert(obj_typeof(ifexpr) == T_IF) ;
	return C_obj_copy(((if_t *)ifexpr)->expr_pred) ;
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

obj_t * C_prim(char * prim) {
	ALLOC_OR_RETNULL(new, prim_t) ;
	*new = PRIM_INIT(prim) ;
	return (obj_t *)new ;
}

obj_t * C_prim_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, prim_t) ;
	*new = PRIM_INIT(prim_vtos(((prim_t *)old)->value)) ;
	return (obj_t *)new ;
}

void D_prim(obj_t ** prim_ptr) {
	free(*prim_ptr) ;
	*prim_ptr = NULL ;
}

obj_t * C_bool(bool value) {
	ALLOC_OR_RETNULL(new, bool_t) ;
	*new = BOOL_INIT(value) ;
	return (obj_t *)new ;
}

void D_bool(obj_t ** bool_ptr) {
	free(*bool_ptr) ;
	*bool_ptr = NULL ;
}

obj_t * C_bool_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, bool_t) ;
	*new = BOOL_INIT(((bool_t *)old)->value);
	return (obj_t *)new ;
}

obj_t * C_num(double value) {
	ALLOC_OR_RETNULL(new, num_t) ;
	*new = NUM_INIT(value) ;
	return (obj_t *)new;
}

obj_t * C_num_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, num_t) ;
	*new = NUM_INIT(((num_t *)old)->value);
	return (obj_t *)new ;
}

void D_num(obj_t ** num_ptr) {
	free(*num_ptr) ;
	*num_ptr = NULL ;
}

