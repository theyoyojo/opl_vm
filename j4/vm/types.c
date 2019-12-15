#include "types.h"
#include "stack.h"
#include <string.h>

obj_t * C_lam(obj_t * recname, olist_t * binding, obj_t * expr) {
	assert(binding), assert(expr) ;
	ALLOC_OR_RETNULL(new, lam_t) ;	
	*new = LAM_INIT(recname, binding, expr) ;
	new->refcnt = 1 ;
	return (obj_t *)new ;
}

obj_t * C_lam_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, lam_t) ;	
	*new = LAM_INIT(C_obj_copy(((lam_t *)old)->recname),
			olist_init_copy(((lam_t *)old)->binding),
			C_obj_copy(((lam_t *)old)->expr)) ;
	new->refcnt = 1 ;
	return (obj_t *)new ;
}


void gen_repr_lam(obj_t * obj) {
	static char part1[] = "(" LAMBDA_STR ".(" ;
	static char part2[] = ") " ;
	static char part3[] = ")" ;
	size_t binding_size,
	       spaces,
	       i ;
	char ** binding_reprs ;
	char * expr_repr ;
	obj_t * tmp ;

	binding_size = olist_length(lam_get_binding(obj)) ;
	spaces = binding_size - 1 ;
	obj->head.repr_size = 0 ;
	if (!(binding_reprs = (char **)malloc(sizeof(char *) * binding_size))) return ;
	for (i = 0; i < binding_size; ++i) {
		tmp = olist_get(lam_get_binding(obj), i) ;
		binding_reprs[i] = obj_repr(tmp) ;
		obj->head.repr_size += obj_repr_size(tmp) ;
	}
	
	expr_repr = obj_repr(lam_get_expr(obj)) ;
	obj->head.repr_size += obj_repr_size(lam_get_expr(obj))
		+ sizeof(part1)
		+ sizeof(part2)
		+ sizeof(part3)
		+ spaces ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;
	strcpy(obj->head.repr, part1) ;
	for (i = 0; i < binding_size; ++i) {
		strcat(obj->head.repr, binding_reprs[i]) ;
		if (i < binding_size - 1) {
			strcat(obj->head.repr, " ") ;
		}
	}
	strcat(obj->head.repr, part2) ;
	strcat(obj->head.repr, expr_repr) ;
	strcat(obj->head.repr, part3) ;
	obj->head.repr[obj_repr_size(obj)] = '\0' ;
	free(binding_reprs) ;
}

obj_t * lam_inc_ref(obj_t * lam) {
	assert(lam) ;
	++((lam_t *)lam)->refcnt ;
	return lam ;
}

void lam_dec_ref(obj_t ** lam_ptr) {
	assert(lam_ptr) ;
	assert(obj_typeof(*lam_ptr) == T_LAM) ;
	if (--(*(lam_t **)lam_ptr)->refcnt <= 0) {
		D_lam(lam_ptr) ;
	}
}

olist_t * lam_get_binding(obj_t * lam) {
	assert(obj_typeof(lam) == T_LAM) ;
	return ((lam_t *)lam)->binding ;
}

obj_t * lam_get_expr(obj_t * lam) {
	assert(obj_typeof(lam) == T_LAM) ;
	return ((lam_t *)lam)->expr;
}

obj_t * lam_get_recname(obj_t * lam) {
	assert(obj_typeof(lam) == T_LAM) ;
	return ((lam_t *)lam)->recname ;
}
	
void D_lam(obj_t ** lam_ptr) {
	assert(lam_ptr) ;
	assert(*lam_ptr) ;
	D_obj_repr(*lam_ptr) ;
	D_OBJ(((lam_t *)*lam_ptr)->recname) ;
	olist_free(&((lam_t *)*lam_ptr)->binding) ;
	D_OBJ(((lam_t *)*lam_ptr)->expr) ;
	free(*lam_ptr) ;
	*lam_ptr = NULL ;
}

/* better be null terinated */
obj_t * C_ident(char * name) {
	assert(name) ;
	ALLOC_OR_RETNULL(new, ident_t) ;
	new->head = HEADER_INIT(T_IDENT, ident_t, gen_repr_ident, ident_dec_ref, ident_inc_ref) ;
	new->size = strlen(name) ;
	if (!(new->value = (char *)malloc(new->size * sizeof(char) + 1))) {
		return NULL ;
	}
	strncpy(new->value, name, new->size) ;
	new->value[new->size] = '\0' ;
	new->refcnt = 1 ;
	return (obj_t *)new ;
}

void gen_repr_ident(obj_t * obj) {
	obj->head.repr_size = ident_get_size(obj) ;
	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;
	if (obj->head.repr) {
		strcpy(obj->head.repr, ident_get_name(obj)) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
}

obj_t * ident_inc_ref(obj_t * ident) {
	assert(ident) ;
	++((ident_t *)ident)->refcnt ;
	return ident ;
}

void ident_dec_ref(obj_t ** ident_ptr) {
	assert(ident_ptr) ;
	assert(obj_typeof(*ident_ptr) == T_IDENT) ;
	if (--(*(ident_t **)ident_ptr)->refcnt <= 0) {
		D_ident(ident_ptr) ;
	}
}

void D_ident(obj_t ** ident_ptr) {
	assert(ident_ptr) ;
	assert(*ident_ptr) ;
	D_obj_repr(*ident_ptr) ;
	free(((ident_t *)*ident_ptr)->value) ;
	free(*ident_ptr) ;
	*ident_ptr = NULL ;
}

char * ident_get_name(obj_t * ident) {
	assert(obj_typeof(ident) == T_IDENT) ;
	return ((ident_t *)ident)->value ;
}

size_t  ident_get_size(obj_t * ident) {
	assert(obj_typeof(ident) == T_IDENT) ;
	return ((ident_t *)ident)->size ;
}

int ident_cmp(obj_t * first, obj_t * second) {
	return strcmp(ident_get_name(first), ident_get_name(second)) ;
}

obj_t * C_ptr(void * addr, size_t size) {
	ALLOC_OR_RETNULL(new, ptr_t) ;
	*new = PTR_INIT(addr, size) ;
	return (obj_t *)new ;
}

obj_t * C_ptr_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, ptr_t) ;
	*new = PTR_INIT(ptr_addr(old), ptr_size(old)) ;
	return (obj_t *)new ;
}

void gen_repr_ptr(obj_t * obj) {
	static char part1[] = "##" ;
	static char part2[] = "@@" ;
	static char part3[] = "##" ;
	static char buff1[16] ;
	static char buff2[16] ;
	size_t addr_str_size,
	       size_str_size ;

	size_str_size = sprintf(buff1, "%zu", ptr_size(obj)) ;
	addr_str_size = sprintf(buff2, "%p", ptr_addr(obj)) ;

	obj->head.repr_size =
		  sizeof(part1)
		+ size_str_size
		+ sizeof(part2)
		+ addr_str_size
		+ sizeof(part3) ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;

	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		strcat(obj->head.repr, buff1) ;
		strcat(obj->head.repr, part2) ;
		strcat(obj->head.repr, buff2) ;
		strcat(obj->head.repr, part3) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}

}

/* gotta take out the trash (collect the garbage) */
void D_ptr(obj_t ** ptr_ptr) {
	assert(ptr_ptr) ;
	assert(*ptr_ptr) ;
	D_obj_repr(*ptr_ptr) ;
	free(*ptr_ptr) ;
	*ptr_ptr = NULL ;
}

void * ptr_addr(obj_t * ptr) {
	assert(obj_typeof(ptr) == T_PTR) ;
	return ((ptr_t *)ptr)->addr ;
}

size_t ptr_size(obj_t * ptr) {
	assert(obj_typeof(ptr) == T_PTR) ;
	return ((ptr_t *)ptr)->size ;
}


obj_t * C_abort(obj_t * expr) {
	ALLOC_OR_RETNULL(new, abort_t) ;
	*new = ABORT_INIT(expr) ;
	return (obj_t *)new ;
}

obj_t * C_abort_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, abort_t) ;
	*new = ABORT_INIT(C_obj_copy(abort_expr(old))) ;
	return (obj_t *)new ;
}
void D_abort(obj_t ** abort_ptr) {
	assert(abort_ptr) ;
	assert(*abort_ptr) ;
	D_obj_repr(*abort_ptr) ;
	D_OBJ(((abort_t *)*abort_ptr)->expr) ;
	free(*abort_ptr) ;
	*abort_ptr = NULL ;
}

void gen_repr_abort(obj_t * obj) {
	static char part1[] = "(abort " ;
	static char part2[] = ")" ;

	char * expr_repr ;

	expr_repr = obj_repr(abort_expr(obj)) ;

	obj->head.repr_size =
		  sizeof(part1)
		+ obj_repr_size(abort_expr(obj))
		+ sizeof(part2) ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;
		
	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		strcat(obj->head.repr, expr_repr) ;
		strcat(obj->head.repr, part2) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
}

obj_t * abort_expr(obj_t * abort) {
	assert(obj_typeof(abort) == T_ABORT) ;
	return ((abort_t *)abort)->expr ;
}

obj_t * C_app(size_t count, ...) {
	ALLOC_OR_RETNULL(new, app_t) ;

	va_list arglist ;
	
	new->head = HEADER_INIT(T_APP, app_t, gen_repr_app, D_app, C_app_copy) ;
	new->expr_list = olist_init() ;
	va_start(arglist, count) ;
	for (size_t i = 0; i < count; ++i) {
		olist_append(&new->expr_list, va_arg(arglist, obj_t *)) ;
	}
	va_end(arglist) ;

	return (obj_t *)new ;
}

/* construct an app from an expr list */
obj_t * C_app_list(olist_t * expr_list) {
	ALLOC_OR_RETNULL(new, app_t) ;
	new->head = HEADER_INIT(T_APP, app_t, gen_repr_app, D_app, C_app_copy) ;
	new->expr_list = expr_list ;
	return (obj_t *)new ;
}

obj_t * C_app_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, app_t) ;

	new->head = HEADER_INIT(T_APP, app_t, gen_repr_app, D_app, C_app_copy) ;
	
	new->expr_list = olist_init_copy(((app_t *)old)->expr_list) ;

	return (obj_t *)new ;
}

void gen_repr_app(obj_t * obj) {
	static char part1[] = "(" ;
	static char part2[] = ")" ;
	size_t 	app_size,
		spaces,
		i ;

	char ** app_expr_reprs ;
	obj_t * tmp ;

	app_size = olist_length(app_get_expr_list(obj)) ;
	spaces = app_size - 1 ;

	obj->head.repr_size = 0 ;
	if (!(app_expr_reprs = (char **)malloc(sizeof(char *) * app_size))) return ;
	for (i = 0; i < app_size; ++i) {
		tmp = olist_get(app_get_expr_list(obj), i) ;
		app_expr_reprs[i] = obj_repr(tmp) ;
		obj->head.repr_size += obj_repr_size(tmp) ;
	}
	obj->head.repr_size +=
		  spaces
		+ sizeof(part1)
		+ sizeof(part2) ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;
	
	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		for (i = 0; i < app_size; ++i) {
			strcat(obj->head.repr, app_expr_reprs[i]) ;
			if (i < app_size - 1) {
				strcat(obj->head.repr, " ") ;
			}
		}
		strcat(obj->head.repr, part2) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
	free(app_expr_reprs) ;
}

void D_app(obj_t ** app_ptr) {
	assert(app_ptr) ;
	D_obj_repr(*app_ptr) ;
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

void gen_repr_if(obj_t * obj) {
	static char  part1[] = "(if " ;
	static char  part2[] = " then " ;
	static char  part3[] = " else " ;
	static char  part4[] = ")" ;
	char * Rpred,
	     * Rtrue,
	     * Rfalse ;
	size_t Spred,
	       Strue,
	       Sfalse ;

	Rpred = obj_repr(if_get_pred(obj)) ;
	Rtrue = obj_repr(if_get_true(obj)) ;
	Rfalse = obj_repr(if_get_false(obj)) ;

	Spred = obj_repr_size(if_get_pred(obj)) ;
	Strue = obj_repr_size(if_get_true(obj)) ;
	Sfalse = obj_repr_size(if_get_false(obj)) ;

	obj->head.repr_size =
		  sizeof(part1)
		+ Spred
		+ sizeof(part2)
		+ Strue
		+ sizeof(part3)
		+ Sfalse
		+ sizeof(part4) ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;
	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		strcat(obj->head.repr, Rpred) ;
		strcat(obj->head.repr, part2) ;
		strcat(obj->head.repr, Rtrue) ;
		strcat(obj->head.repr, part3) ;
		strcat(obj->head.repr, Rfalse) ;
		strcat(obj->head.repr, part4) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
}

void D_if(obj_t ** if_ptr) {
	D_obj_repr(*if_ptr) ;
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

obj_t * C_pair(obj_t * first, obj_t * second) {
	ALLOC_OR_RETNULL(new, pair_t) ;
	*new = PAIR_INIT(first, second) ;
	return (obj_t *)new ;
}

obj_t * C_pair_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, pair_t) ;
	*new = PAIR_INIT(C_obj_copy(pair_first(old)), C_obj_copy(pair_second(old))) ;
	return (obj_t *)new ;
}

void gen_repr_pair(obj_t * obj) {
	static char part1[] = "<" ;
	static char part2[] = ", " ;
	static char part3[] = ">" ;
	char * Rfirst, * Rsecond ;

	Rfirst = obj_repr(pair_first(obj)) ;
	Rsecond = obj_repr(pair_second(obj)) ;

	obj->head.repr_size =
		  sizeof(part1)
		+ obj_repr_size(pair_first(obj))
		+ sizeof(part2)
		+ obj_repr_size(pair_second(obj))
		+ sizeof(part3) ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;

	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		strcat(obj->head.repr, Rfirst) ;
		strcat(obj->head.repr, part2) ;
		strcat(obj->head.repr, Rsecond) ;
		strcat(obj->head.repr, part3) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}

}

void D_pair(obj_t ** pair_ptr) {
	assert(pair_ptr) ;
	assert(*pair_ptr) ;
	D_obj_repr(*pair_ptr) ;
	D_OBJ(((pair_t *)*pair_ptr)->first) ;
	D_OBJ(((pair_t *)*pair_ptr)->second) ;
	free(*pair_ptr) ;
	*pair_ptr = NULL ;
}

obj_t * pair_first(obj_t * pair) {
	assert(obj_typeof(pair) == T_PAIR) ;
	return ((pair_t *)pair)->first ;
}

obj_t * pair_second(obj_t * pair) {
	assert(obj_typeof(pair) == T_PAIR) ;
	return ((pair_t *)pair)->second ;
}

void pair_overwrite_first(obj_t * pair, obj_t * new) {
	assert(obj_typeof(pair) == T_PAIR) ;
	((pair_t *)pair)->first = new ;

}
void pair_overwrite_second(obj_t * pair, obj_t * new) {
	assert(obj_typeof(pair) == T_PAIR) ;
	((pair_t *)pair)->second = new ;
}

char * prim_syms[] = {
	[PRIM_INVALID]	= "[?]",
	[PRIM_PLUS]	= "+",
	[PRIM_MULT]	= "*",
	[PRIM_DIV]	= "/",
	[PRIM_SUB]	= "-",
	[PRIM_MOD]	= "mod",
	[PRIM_LTEQ]	= "<=",
	[PRIM_LT]	= "<",
	[PRIM_EQ]	= "=",
	[PRIM_GT]	= ">",
	[PRIM_GTEQ]	= ">=",
	[PRIM_PAIR]	= "pair",
	[PRIM_FST]	= "fst",
	[PRIM_SND]	= "snd",
	[PRIM_BOX]	= "box",
	[PRIM_UNBOX]	= "unbox",
	[PRIM_SETBOX]	= "set-box!",
	[PRIM_PRINT]	= "print",
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
	return prim_syms[prim_val] ;
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

void gen_repr_prim(obj_t * obj) {
	obj->head.repr_size = sizeof(prim_vtos(prim_get_val(obj))) ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;

	if (obj->head.repr) {
		strcpy(obj->head.repr, prim_vtos(prim_get_val(obj))) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
}

prim_val_t prim_get_val(obj_t * prim) {
	assert(obj_typeof(prim) == T_PRIM) ;
	return ((prim_t *)prim)->value ;
}

void D_prim(obj_t ** prim_ptr) {
	D_obj_repr(*prim_ptr) ;
	free(*prim_ptr) ;
	*prim_ptr = NULL ;
}

obj_t * C_bool(bool value) {
	ALLOC_OR_RETNULL(new, bool_t) ;
	*new = BOOL_INIT(value) ;
	return (obj_t *)new ;
}

obj_t * C_bool_copy(obj_t * old) {
	assert(old) ;
	ALLOC_OR_RETNULL(new, bool_t) ;
	*new = BOOL_INIT(((bool_t *)old)->value);
	return (obj_t *)new ;
}

void gen_repr_bool(obj_t * obj) {
	static char * options[] = { "False", "True" } ;
	enum bool_options { OPTION_FALSE = 0, OPTION_TRUE = 1 } option ;

	if (bool_get(obj)) {
		option = OPTION_TRUE ;
	} else {
		option = OPTION_FALSE ;
	}

	obj->head.repr_size = strlen(options[option]) ;
	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;
	
	if (obj->head.repr) {
		strcpy(obj->head.repr, options[option]) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
}

void D_bool(obj_t ** bool_ptr) {
	D_obj_repr(*bool_ptr) ;
	free(*bool_ptr) ;
	*bool_ptr = NULL ;
}

bool bool_get(obj_t * obj) {
	assert(obj_typeof(obj) == T_BOOL) ;
	return ((bool_t *)obj)->value ;
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

double num_get(obj_t * obj) {
	assert(obj_typeof(obj) == T_NUM) ;
	return ((num_t *)obj)->value ;
}

void gen_repr_num(obj_t * obj) {
	static char buff[16] ;
	obj->head.repr_size = sprintf(buff, "%g", num_get(obj)) ;
	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;
	
	if (obj->head.repr) {
		strcpy(obj->head.repr, buff) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
}

void D_num(obj_t ** num_ptr) {
	D_obj_repr(*num_ptr) ;
	free(*num_ptr) ;
	*num_ptr = NULL ;
}

obj_t * C_str(char * str) {
	ALLOC_OR_RETNULL(new, str_t) ;
	new->head = HEADER_INIT(T_STR, str_t, gen_repr_str, D_str, C_str_copy) ;
	new->size = strlen(str) ;
	if (!(new->value = (char *)malloc(new->size + 1))) {
		return NULL ;
	}
	strncpy(new->value, str, new->size) ;
	new->value[new->size] = '\0' ;
	return (obj_t *)new ;
}

obj_t * C_stringify(obj_t * obj) {
	return C_str(obj_repr(obj)) ;
}

obj_t * C_str_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, str_t) ;
	new->head = HEADER_INIT(T_STR, str_t, gen_repr_str, D_str, C_str_copy) ;
	new->size = str_size(old) ;
	if (!(new->value = (char *)malloc(new->size + 1))) {
		return NULL ;
	}
	strncpy(new->value, str_get(old), new->size) ;
	new->value[new->size] = '\0' ;
	return (obj_t *)new ;

}

void gen_repr_str(obj_t * obj) {
	static char part1[] = "\"" ;
	static char part2[] = "\"" ;
	obj->head.repr_size =
		  sizeof(part1)
		+ str_size(obj)
		+ sizeof(part2) ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;
	
	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		strcat(obj->head.repr, str_get(obj)) ;
		strcat(obj->head.repr, part1) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
}

void D_str(obj_t ** str_ptr) {
	assert(str_ptr) ;
	assert(*str_ptr) ;
	D_obj_repr(*str_ptr) ;
	free(((str_t *)*str_ptr)->value) ;
	free(*str_ptr) ;
	*str_ptr = NULL ;
}

char * str_get(obj_t * str) {
	assert(obj_typeof(str) == T_STR) ;
	return ((str_t *)str)->value ;
}

size_t str_size(obj_t *str) {
	assert(obj_typeof(str) == T_STR) ;
	return ((str_t *)str)->size ;
}

static int _unit_refcnt = 0 ;
static obj_t * _unit_ptr = NULL ;

obj_t * _unit_inc(void) {
	if (_unit_refcnt <= 0) {
		if (!(_unit_ptr = (obj_t *)malloc(sizeof(obj_t)))) {
			return NULL ;
		}

		*_unit_ptr = (obj_t) {
			.head = HEADER_INIT(T_UNIT, obj_t, gen_repr_unit, D_unit, C_unit_copy)
		} ;
		_unit_refcnt = 1 ;
	} else {
		++_unit_refcnt ;
	}

	return _unit_ptr ;
}

void _unit_dec(void) {
	if (--_unit_refcnt <= 0) {
		D_obj_repr(_unit_ptr) ;
		free(_unit_ptr) ;
		_unit_ptr = NULL ;
	}
}

void gen_repr_unit(obj_t * obj) {
	static char * part1 = "[UNIT]" ;

	obj->head.repr_size = sizeof(part1) ;
	obj->head.repr = malloc(obj->head.repr_size) ;
	strcpy(obj->head.repr, part1) ;
}

obj_t * C_unit() {
	return _unit_inc() ;
}

obj_t * C_unit_copy(obj_t * old) {
	(void)old ;
	return _unit_inc() ;
}

void D_unit(obj_t ** unit_ptr) {
	(void)unit_ptr ;
	_unit_dec() ;
}
