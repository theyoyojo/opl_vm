#include "types.h"
#include "stack.h"
#include <string.h>

obj_t * C_lam(obj_t * recname, olist_t * binding, obj_t * expr) {
	assert(binding), assert(expr) ;
	ALLOC_OR_RETNULL(new, lam_t) ;	
	*new = LAM_INIT(recname, binding, expr) ;
	return (obj_t *)new ;
}

obj_t * C_lam_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, lam_t) ;	
	*new = LAM_INIT(C_obj_copy(((lam_t *)old)->recname),
			olist_init_copy(((lam_t *)old)->binding),
			C_obj_copy(((lam_t *)old)->expr)) ;
	return (obj_t *)new ;
}

olist_t * lam_get_binding(obj_t * lam) {
	assert(obj_typeof(lam) == T_LAM) ;
	return ((lam_t *)lam)->binding ;
}

obj_t * lam_get_expr(obj_t * lam) {
	assert(obj_typeof(lam) == T_LAM) ;
	return ((lam_t *)lam)->expr;
}
	
void D_lam(obj_t ** lam_ptr) {
	assert(lam_ptr) ;
	assert(*lam_ptr) ;
	D_OBJ(((lam_t *)*lam_ptr)->recname) ;
	olist_free(&((lam_t *)*lam_ptr)->binding) ;
	D_OBJ(((lam_t *)*lam_ptr)->expr) ;
	free(*lam_ptr) ;
	*lam_ptr = NULL ;
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


void value_print(obj_t * value) {
	assert(obj_isvalue(value)) ;

	switch (obj_typeof(value)) {
	case T_NUM:
		printf("%g", ((num_t *)value)->value) ;
		break ;
	case T_BOOL:
		printf("%s", ((bool_t *)value)->value ? "True" : "False") ;
		break ;
	case T_PRIM:
		printf("%s", prim_vtos(((prim_t *)value)->value)) ;
		break ;
	case T_LAM:
		printf("LAM[$(") ;
		for (size_t i = 0; i < olist_length(lam_get_binding(value)); ++i) {
			expr_print(olist_get(lam_get_binding(value),i)) ;
			if (i != olist_length(lam_get_binding(value)) - 1) {
				printf(", ") ;
			}
		}
		printf(") ") ;
		expr_print(lam_get_expr(value)) ;
		printf("]") ;
		break ;
	case T_CLO:
		printf("CLO[$(") ;
		for (size_t i = 0; i < olist_length(lam_get_binding(clo_get_lam(value))); ++i) {
			expr_print(olist_get(lam_get_binding(clo_get_lam(value)),i)) ;
			if (i != olist_length(lam_get_binding(clo_get_lam(value))) - 1) {
				printf(", ") ;
			}
		}
		printf(") ") ;
		expr_print(lam_get_expr(clo_get_lam(value))) ;
		printf("]") ;
		break ;
	default:
		printf("SOMETHING WENT terribly WRONG IN value_print()") ;
	}
}
void expr_print(obj_t * expr) {
	if (obj_isvalue(expr)) {
		value_print(expr) ;
		return ;
	}
	switch (obj_typeof(expr)) {
	case T_APP:
		printf("App[") ;
		for (size_t i = 0; i < olist_length(app_get_expr_list(expr)); ++i) {
			expr_print(olist_get(app_get_expr_list(expr), i)) ;
			putchar(' ') ;
		}
		printf("]") ;
		break ;
	case T_IF:
		printf("If[") ;
		expr_print(if_get_pred(expr)) ;
		putchar(' ') ;
		expr_print(if_get_true(expr)) ;
		putchar(' ') ;
		expr_print(if_get_false(expr)) ;
		printf("]") ;
		break ;
	case T_IDENT:
		printf("%s", ((ident_t *)expr)->value) ;
		break ;
	default:
		printf("SOMETHING WENT terribly WRONG IN expr_print()") ;
	}
}
