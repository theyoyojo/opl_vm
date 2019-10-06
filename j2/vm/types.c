#include "types.h"
#include <string.h>

obj_t * C_prog(void) {
	ALLOC_OR_RETNULL(new, prog_t) ;
	*new = PROG_INIT() ;
	return (obj_t *)new ;
}

obj_t * C_prog_copy(obj_t * old) {
	assert(old) ;
	prog_t * old_prog = (prog_t *)old ;
	ALLOC_OR_RETNULL(new, prog_t) ;
	*new = PROG_INIT() ;
	for (size_t i = 0; i < olist_length(old_prog->topforms); ++i) {
		olist_append(new->topforms, C_obj_copy(olist_get(old_prog->topforms,i))) ;
	}
	
	return (obj_t *)new ;
}

void D_prog(obj_t ** prog_ptr) {
	assert(prog_ptr) ;
	assert(*prog_ptr) ;
	olist_free(&((prog_t *)*prog_ptr)->topforms) ;
	free(*prog_ptr) ;
	*prog_ptr = NULL ;
}

void prog_append(obj_t * prog, obj_t * topform) {
	assert(obj_typeof(prog) == T_PROG) ;
	prog_t * prog_ = (prog_t *)prog ;
	if (olist_length(prog_->topforms) > 0 && 
		obj_typeof(olist_get(prog_->topforms,olist_length(prog_->topforms) - 1)) != T_FUNC &&
		obj_typeof(topform) == T_FUNC) {
		printf("Error: function definitions must come before the expression\n") ;
		assert(0) ;
	}
	olist_append(prog_->topforms, topform) ;
}

olist_t * prog_get_topforms(obj_t * prog) {
	return ((prog_t *)prog)->topforms ;
}

obj_t * C_func(olist_t * binding, obj_t * expr) {
	assert(binding), assert(expr) ;
	ALLOC_OR_RETNULL(new, func_t) ;	
	*new = FUNC_INIT(binding, expr) ;
	return (obj_t *)new ;
}
obj_t * C_func_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, func_t) ;	
	*new = FUNC_INIT(olist_init_copy(((func_t *)old)->binding),
			C_obj_copy(((func_t *)old)->expr)) ;
	return (obj_t *)new ;
}

olist_t * func_get_binding(obj_t * func) {
	assert(obj_typeof(func) == T_FUNC) ;
	return ((func_t *)func)->binding ;
}

obj_t * func_get_expr(obj_t * func) {
	assert(obj_typeof(func) == T_FUNC) ;
	return ((func_t *)func)->expr;
}

char * func_get_name(obj_t * func) {
	assert(obj_typeof(func) == T_FUNC) ;
	olist_t * binding = func_get_binding(func) ;
	ident_t * name_ident = (ident_t *)olist_get(binding, 0) ;
	return name_ident->value ;
}
	
void D_func(obj_t ** func_ptr) {
	assert(func_ptr) ;
	assert(*func_ptr) ;
	olist_free(&((func_t *)*func_ptr)->binding) ;
	D_OBJ(((func_t *)*func_ptr)->expr) ;
	free(*func_ptr) ;
	*func_ptr = NULL ;
}

obj_t * C_ident(char * name) {
	assert(name) ;
	ALLOC_OR_RETNULL(new, ident_t) ;
	new->head = HEADER_INIT(T_IDENT, D_ident, C_ident_copy) ;
	new->length = sizeof(name) ;
	new->value = (char *)malloc(new->length * sizeof(char)) ;
	strncpy(new->value, name, new->length) ;
	return (obj_t *) new ;
}

obj_t * C_ident_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, ident_t) ;
	new->head = HEADER_INIT(T_IDENT, D_ident, C_ident_copy) ;
	new->length = ((ident_t *)old)->length ;
	new->value = (char *)malloc(new->length * sizeof(char)) ;
	strncpy(new->value, ((ident_t *)old)->value, new->length) ;
	return (obj_t *)new ;
}

void D_ident(obj_t ** ident_ptr) {
	assert(ident_ptr) ;
	assert(*ident_ptr) ;
	free(((ident_t *)*ident_ptr)->value) ;
	free(*ident_ptr) ;
	*ident_ptr = NULL ;
}

char * ident_get_name(obj_t * ident) {
	assert(obj_typeof(ident) == T_IDENT) ;
	return ((ident_t *)ident)->value ;
}

int ident_cmp(obj_t * first, obj_t * second) {
	return strcmp(ident_get_name(first), ident_get_name(second)) ;
}

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

/* construct an app from an expr list */
obj_t * C_app_list(olist_t * expr_list) {
	ALLOC_OR_RETNULL(new, app_t) ;
	new->head = HEADER_INIT(T_APP, D_app, C_app_copy) ;
	new->expr_list = expr_list ;
	return (obj_t *)new ;
}

obj_t * C_app_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, app_t) ;

	new->head = HEADER_INIT(T_APP, D_app, C_app_copy) ;
	
	new->expr_list = olist_init_copy(((app_t *)old)->expr_list) ;

	return (obj_t *)new ;
}

void D_app(obj_t ** app_ptr) {
	assert(app_ptr) ;
	olist_free(&(*(app_t **)app_ptr)->expr_list) ;
	free(*app_ptr) ;
	*app_ptr = NULL ;
}

obj_t * app_copy_first(obj_t * app) {
	assert(obj_typeof(app) == T_APP) ;
	return C_obj_copy(olist_get(((app_t *)app)->expr_list, 0)) ;
}

olist_t * app_get_expr_list(obj_t * app) {
	assert(obj_typeof(app) == T_APP) ;
	return ((app_t *)app)->expr_list ;
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

obj_t * if_get_pred(obj_t * ifexpr) {
	assert(obj_typeof(ifexpr) == T_IF) ;
	return ((if_t *)ifexpr)->expr_pred ;
}

obj_t * if_get_true(obj_t * ifexpr) {
	assert(obj_typeof(ifexpr) == T_IF) ;
	return ((if_t *)ifexpr)->expr_true ;
}

obj_t * if_get_false(obj_t * ifexpr) {
	assert(obj_typeof(ifexpr) == T_IF) ;
	return ((if_t *)ifexpr)->expr_false ;
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

