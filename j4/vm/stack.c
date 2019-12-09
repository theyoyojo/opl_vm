#include "stack.h"
#include "types.h"
#include <string.h>

obj_t * C_stack() {
	ALLOC_OR_RETNULL(new, stack_t) ;
	new->head = HEADER_INIT(T_STACK, stack_t, gen_repr_stack, D_stack, C_stack_copy) ;
	new->data = olist_init() ;
	return (obj_t *)new ;
}

obj_t * C_stack_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, stack_t) ;
	new->head = HEADER_INIT(T_STACK, stack_t, gen_repr_stack, D_stack, C_stack_copy) ;
	new->data = olist_init_copy(((stack_t *)old)->data) ;
	return (obj_t *)new ;
}

#define STACK_PREFIX_STRSZ 6

void gen_repr_stack(obj_t * obj) {
	static char part1[] = "Stack:\n" ;
	static char separator[] = "\n" ;
	char ** frame_reprs ;
	char buff[STACK_PREFIX_STRSZ + 1] ;
	size_t stack_size, prefixes, separators, i ;
	obj_t * tmp ;

	obj->head.repr_size = 0 ;

	stack_size = stack_height(obj) ;
	if (!(frame_reprs = (char **)malloc(sizeof(char *) * stack_size))) return ;
	for (i = 0; i < stack_size; ++i) {
		tmp = olist_get(((stack_t *)obj)->data, i) ;
		frame_reprs[i] = obj_repr(tmp) ;
		obj->head.repr_size += obj_repr_size(tmp) ;
	}
	
	separators = stack_size * sizeof(separator) ;
	prefixes = stack_size * STACK_PREFIX_STRSZ ;
	obj->head.repr_size +=
		  sizeof(part1)
		+ separators
		+ prefixes ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;

	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		for (i = 0; i < stack_size; ++i) {
			sprintf(buff, "%4zu: ", stack_size - i - 1) ;
			strcat(obj->head.repr, buff) ;
			strcat(obj->head.repr, frame_reprs[i]) ;
			strcat(obj->head.repr, separator) ;
		}
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}

	free(frame_reprs) ;
	
}

void D_stack(obj_t ** stack_ptr) {
	assert(stack_ptr) ;
	assert(*stack_ptr) ;
	D_obj_repr(*stack_ptr) ;
	olist_free(&((stack_t *)*stack_ptr)->data) ;
	free(*stack_ptr) ;
	*stack_ptr = NULL ;
}

void stack_push(obj_t * stack, obj_t * frame) {
	assert(obj_typeof(stack) == T_STACK) ;
	assert(obj_isframe(frame)) ;
	
	olist_insert(&((stack_t *)stack)->data, frame, 0) ;
}

void stack_chop(obj_t * stack) {
	assert(obj_typeof(stack) == T_STACK) ;
	stack_t * _stack = (stack_t *)stack ;
	if (olist_empty(((stack_t *)stack)->data)) {
		return ;
	}
	else {
		olist_del(&_stack->data, 0) ;
	}
}

bool stack_empty(obj_t * stack) {
	assert(obj_typeof(stack) == T_STACK) ;
	return olist_empty(((stack_t *)stack)->data) ;
}

size_t stack_height(obj_t * stack) {
	assert(obj_typeof(stack) == T_STACK) ;
	return olist_length(((stack_t *)stack)->data) ;	
}

obj_t * stack_top(obj_t * stack) {
	assert(obj_typeof(stack) == T_STACK) ;
	if (stack_empty(stack)) {
		return NULL ;
	} else {
		return olist_get(((stack_t *)stack)->data, 0) ;
	}
}

void stack_trace(obj_t * stack) {
	assert(obj_typeof(stack) == T_STACK) ;
	/* stack_t * stack_ = (stack_t *)stack ; */
		
	/* obj_t * tmp ; */
	/* printf("Stack trace:\n") ; */

	/* for (size_t i = 0; i < olist_length(stack_->data); ++i) { */
	/* 	tmp = olist_get(stack_->data, i) ; */
	/* 	printf("\t%s %p: frame: %s env: %s\n", */
	/* 		i == 0 ? "at" : "by", */
	/* 		tmp, frame_get_name(tmp), frame_get_env_name(tmp)) ; */
	/* 	frame_print(tmp) ; */
	/* } */

	printf("%s\n", obj_repr(stack)) ;
}

/* actually, this function should definitely get a new reference, 
 * I had this idea of some frames storing multiple references to be distributed
 * for each needed use, but I think that was misguided */
obj_t * stack_top_env(obj_t * stack) {
	assert(stack) ;
	assert(!stack_empty(stack)) ;

	obj_t ** env_ptr ;
	/* obj_t * tmp ; */

	switch(obj_typeof(stack_top(stack))) {
	case T_FRAPP:
		env_ptr = &((frapp_t *)stack_top(stack))->env ;
		break ;
	case T_FRIF:
		env_ptr = &((frif_t *)stack_top(stack))->env ;
		break ;
	case T_FRRET:
		env_ptr = &((frret_t *)stack_top(stack))->env ;
		break ;
	default:
		env_ptr = NULL ;
		printf("Exception: non frame object in stack\n") ;
		stack_trace(stack) ;
		exit(1) ; // TODO
	}

	return env_inc_ref(*env_ptr) ;
	
}

obj_t * C_frif(obj_t * ifexpr, obj_t * env) {
	assert(obj_typeof(ifexpr) == T_IF) ;
	ALLOC_OR_RETNULL(new, frif_t) ;
	new->head = HEADER_INIT(T_FRIF, frif_t, gen_repr_frif, D_frif, C_frif_copy) ;
	new->e_true = C_obj_copy(((if_t *)ifexpr)->expr_true) ;
	new->e_false = C_obj_copy(((if_t *)ifexpr)->expr_false) ;
	new->env = env_inc_ref(env) ;
	return (obj_t *)new ;
}

obj_t * C_frif_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, frif_t) ;
	new->head = HEADER_INIT(T_FRIF, frif_t, gen_repr_frif, D_frif, C_frif_copy) ;
	new->e_true = C_obj_copy(((frif_t *)old)->e_true) ;
	new->e_false = C_obj_copy(((frif_t *)old)->e_false) ;
	new->env = env_inc_ref(((frif_t *)old)->env) ;
	return (obj_t *)new ;
}

void gen_repr_frif(obj_t * obj) {
	static char part1[] = "[Kif " ;
	static char part2[] = " " ;
	static char part3[] = "] " ;
	char * true_repr, * false_repr ;
	true_repr = obj_repr(frif_true(obj)) ;
	false_repr = obj_repr(frif_false(obj)) ;
	obj->head.repr_size =
		  sizeof(part1)
		+ obj_repr_size(frif_true(obj))
		+ sizeof(part2)
		+ obj_repr_size(frif_false(obj))
		+ sizeof(part3) ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;

	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		strcat(obj->head.repr, true_repr) ;
		strcat(obj->head.repr, part2) ;
		strcat(obj->head.repr, false_repr) ;
		strcat(obj->head.repr, part3) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
	
}

obj_t * frif_true(obj_t * frif) {
	assert(obj_typeof(frif) == T_FRIF) ;
	return ((frif_t *)frif)->e_true ;
}
obj_t * frif_false(obj_t * frif) {
	assert(obj_typeof(frif) == T_FRIF) ;
	return ((frif_t *)frif)->e_false;

}

void D_frif(obj_t ** frif_ptr) {
	assert(frif_ptr) ;
	assert(*frif_ptr) ;
	frif_t * frif = *(frif_t **)frif_ptr ;
	D_obj_repr(*frif_ptr) ;
	D_obj(frif->e_true)(&frif->e_true) ;
	D_obj(frif->e_false)(&frif->e_false) ;
	D_OBJ(frif->env) ;
	free(frif) ;
	*frif_ptr = NULL ;
}

obj_t * frif_copy_true(obj_t * frif) {
	assert(obj_typeof(frif) == T_FRIF) ;
	return C_obj_copy(((frif_t *)frif)->e_true) ;
}

obj_t * frif_copy_false(obj_t * frif) {
	assert(obj_typeof(frif) == T_FRIF) ;
	return C_obj_copy(((frif_t *)frif)->e_false) ;
}

obj_t * C_frapp(obj_t * app, obj_t * env) {
	assert(obj_typeof(app) == T_APP) ;
	ALLOC_OR_RETNULL(new, frapp_t) ;
	new->head = HEADER_INIT(T_FRAPP, frapp_t, gen_repr_frapp, D_frapp, C_frapp_copy) ;
	new->vals = olist_init() ;
	new->exprs = olist_init_copy(((app_t *)app)->expr_list) ;
	/* the frapp should have an env ref available for each expr NOTE: this was a bad idea */
	/* for (size_t i = 0; i < olist_length(new->exprs); ++i) { */
	new->env = env_inc_ref(env) ;
	/* } */
	return (obj_t *)new ;
}
obj_t * C_frapp_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, frapp_t) ;
	new->head = HEADER_INIT(T_FRAPP, frapp_t, gen_repr_frapp, D_frapp, C_frapp_copy) ;
	new->vals = olist_init_copy(((frapp_t *)old)->vals) ;
	new->exprs = olist_init_copy(((frapp_t *)old)->exprs) ;
	/* for (size_t i = 0; i < olist_length(new->exprs); ++i) { */
	new->env = env_inc_ref(((frapp_t *)old)->env) ;
	/* } */
	return (obj_t *)new ;
}

void gen_repr_frapp(obj_t * obj) {
	static char part1[] = "[Kapp " ;
	static char part2[] = " | " ;
	static char part3[] = "]" ;
	char 	** val_reprs,
		** expr_reprs ;
	size_t vals_size, exprs_size, spaces, i ;
	obj_t * tmp ;

	obj->head.repr_size = 0 ;

	vals_size = olist_length(frapp_get_vals(obj)) ;
	if (!(val_reprs = (char **)malloc(sizeof(char *) * vals_size))) return ;
	for (i = 0; i < vals_size; ++i) {
		/* printf("aSDFSDFDSF\n\n") ; */
		tmp = olist_get(frapp_get_vals(obj), i) ;
		val_reprs[i] = obj_repr(tmp) ;
		obj->head.repr_size += obj_repr_size(tmp) ;
	}

	exprs_size = olist_length(frapp_get_exprs(obj)) ;
	if (!(expr_reprs = (char **)malloc(sizeof(char *) * exprs_size))) { free(val_reprs) ; return ; }
	for (i = 0; i < exprs_size; ++i) {
		tmp = olist_get(frapp_get_exprs(obj), i) ;
		expr_reprs[i] = obj_repr(tmp) ;
		obj->head.repr_size += obj_repr_size(tmp) ;
	}

	spaces = vals_size + exprs_size - 2 ;

	obj->head.repr_size +=
		  sizeof(part1)
		+ sizeof(part2)
		+ sizeof(part3)
		+ spaces ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;

	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		for (i = 0; i < vals_size; ++i) {
			strcat(obj->head.repr, val_reprs[i]) ;
			if (i < vals_size - 1) {
				strcat(obj->head.repr, " ") ;
			}
		}
		strcat(obj->head.repr, part2) ;
		for (i = 0; i < exprs_size; ++i) {
			strcat(obj->head.repr, expr_reprs[i]) ;
			if (i < exprs_size - 1) {
				strcat(obj->head.repr, " ") ;
			}
		}
		strcat(obj->head.repr, part3) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}

	free(val_reprs) ;
	free(expr_reprs) ;
}

void D_frapp(obj_t ** frapp_ptr) {
	assert(frapp_ptr) ;
	assert(*frapp_ptr) ;
	frapp_t * frapp = *(frapp_t **)frapp_ptr ;
	D_obj_repr(*frapp_ptr) ;
	olist_free(&frapp->vals) ;
	olist_free(&frapp->exprs) ;
	D_OBJ(frapp->env) ;
	free(frapp) ;
	*frapp_ptr = NULL ;
}


obj_t * frapp_pop_expr(obj_t * frapp) {
	assert(obj_typeof(frapp) == T_FRAPP) ;
	D_obj_repr(frapp) ;
	return olist_pop_index(&((frapp_t *)frapp)->exprs, 0) ;
}

void frapp_push_value(obj_t * frapp, obj_t * obj) {
	assert(obj_typeof(frapp) == T_FRAPP) ;
	olist_append(&((frapp_t *)frapp)->vals, obj) ;
	D_obj_repr(frapp) ;
}

bool frapp_has_more_exprs(obj_t * frapp) {
	assert(obj_typeof(frapp) == T_FRAPP) ;
	return !olist_empty(((frapp_t *)frapp)->exprs) ;
}

olist_t * frapp_get_vals(obj_t * frapp) {
	assert(obj_typeof(frapp) == T_FRAPP) ;
	return ((frapp_t *)frapp)->vals ;
}

olist_t * frapp_get_exprs(obj_t * frapp) {
	assert(obj_typeof(frapp) == T_FRAPP) ;
	return ((frapp_t *)frapp)->exprs;
}

obj_t * frapp_get_first_value(obj_t * frapp) {
	return olist_get(frapp_get_vals(frapp),0) ;
}

obj_t * C_frret(obj_t * env) {
	ALLOC_OR_RETNULL(new, frret_t) ;
	new->head = HEADER_INIT(T_FRRET, frret_t, gen_repr_frret, D_frret, C_frret_copy) ;
	new->env = env_inc_ref(env) ;
	return (obj_t *)new ;
}

obj_t * C_frret_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, frret_t) ;
	new->head = HEADER_INIT(T_FRRET, frret_t, gen_repr_frret, D_frret, C_frret_copy) ;
	new->env = env_inc_ref(((frret_t *)old)->env) ;
	return (obj_t *)new ;
}
void D_frret(obj_t ** frret_ptr) {
	assert(frret_ptr) ;
	assert(*frret_ptr) ;
	D_obj_repr(*frret_ptr) ;
	D_OBJ(((frret_t*)*frret_ptr)->env) ;
	free(*frret_ptr) ;
	*frret_ptr = NULL ;
}

void gen_repr_frret(obj_t * obj) {
	static char part1[] = "[Kret]" ;

	obj->head.repr_size = sizeof(part1) ;
	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;

	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
}

obj_t * C_env(void) {
	ALLOC_OR_RETNULL(new, env_t) ;
	*new = ENV_INIT() ;
	return (obj_t *)new ;

}

/* Perhaps one should not make copies of the env... */
/* or maybe the copy should just be a reference ...  */
obj_t * C_env_copy(obj_t * old) {
	env_t * old_env = (env_t *)old ;
	ALLOC_OR_RETNULL(new, env_t) ;
	new->head = HEADER_INIT(T_ENV, env_t, gen_repr_env, env_dec_ref, C_env_copy) ;
	new->idents = olist_init_copy(old_env->idents) ;
	new->vals = olist_init_copy(old_env->vals) ;
	new->refcnt = 1 ;
	return (obj_t *)new ;
}

void gen_repr_env(obj_t * obj) {
	static char part1[] = "{" ;
	static char part2[] = "}" ;
	static char separator[] = ":" ;
	char 	** ident_reprs,
		** val_reprs ;
	size_t idents_size, vals_size, spaces, separators, i ;
	obj_t * tmp ;

	obj->head.repr_size = 0 ;

	idents_size = env_length(obj) ;
	if (!(ident_reprs = (char **)malloc(sizeof(char *) * idents_size))) return ;
	for (i = 0; i < idents_size; ++i) {
		tmp = (obj_t *)env_get_ident(obj, i) ;
		ident_reprs[i] = obj_repr(tmp) ;
		obj->head.repr_size += obj_repr_size(tmp) ;
	}

	vals_size = env_girth(obj) ;
	if (!(val_reprs = (char **)malloc(sizeof(char *) * vals_size))) { free(ident_reprs) ; return ; }
	for (i = 0; i < vals_size; ++i) {
		tmp = env_get_val(obj, i) ;
		val_reprs[i] = obj_repr(tmp) ;
		obj->head.repr_size += obj_repr_size(tmp) ;
	}

	spaces = idents_size - 1 ;
	
	separators = idents_size * sizeof(separator) ;

	obj->head.repr_size +=
		  sizeof(part1)
		+ sizeof(part2)
		+ spaces
		+ separators ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;

	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		for (i = 0; i < vals_size; ++i) {
			strcat(obj->head.repr, ident_reprs[i]) ;
			/* printf("CAT REPR: %s\n", ident_reprs[i]) ; */
			strcat(obj->head.repr, separator) ;
			strcat(obj->head.repr, val_reprs[i]) ;
			if (i < vals_size - 1) {
				strcat(obj->head.repr, " ") ;
			}
		}
		strcat(obj->head.repr, part2) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}

	free(ident_reprs) ;
	free(val_reprs) ;
}

void D_env(obj_t ** env_ptr) {
	assert(env_ptr) ;
	assert(*env_ptr) ;
	env_t * env = *(env_t **)env_ptr ;
	D_obj_repr(*env_ptr) ;
	olist_free(&env->idents) ;
	olist_free(&env->vals) ;
	free(env) ;
	*env_ptr = NULL ;
}

/* maximum number identifiers that can be bound at once */
#define MAX_BIND_AT_ONCE 256UL
/* can fail by arity mismatch */
int env_bind(obj_t * env, olist_t * binding, olist_t * vals) {
	assert(env) ;
	size_t bindlen = olist_length(binding) ;
	size_t vallen = olist_length(vals) - 1;
	static unsigned char index_memo[MAX_BIND_AT_ONCE] ;
	memset(index_memo, 0, MAX_BIND_AT_ONCE) ;
	env_t * env_ = (env_t *)env ;

	D_obj_repr(env) ;

	if (bindlen > MAX_BIND_AT_ONCE) {
		printf("Exception: Binding too long to handle in one go\n"
				"\tlength: %lu, max: %lu\n",
				bindlen, MAX_BIND_AT_ONCE) ;
		return 1 ;
	}
	if (bindlen != vallen) {
		printf("Exception: Invalid environment binding: \n"
				"\tident count: %lu, differs from value count: %lu\n",
				bindlen, vallen) ;
		return 1 ;
	}

	/* FIXME wrong: Include function name for possible usage later as env name */

	/* first iterate through existing identifiers and replace matching */
	/* keep trace of indexes in arglists that ovewrite, these wont be appended */

	size_t i, j;
	for (i = 0; i < bindlen; ++i) {
		for (j = 0; j < olist_length(env_->idents); ++j) {
			if (!ident_cmp(olist_get(env_->idents, j),
						olist_get(binding, i))) {
				/* delete and reinsert */
				olist_del(&env_->vals, j);
				olist_insert(&env_->vals,
						C_obj_copy(olist_get(vals, i + 1)),j) ;
				index_memo[i] = 1 ;
				break ;
			}
		}
		if (index_memo[i]) continue ; /* skip overwritten */

		olist_append(&env_->idents, C_obj_copy(olist_get(binding, i))) ;
		olist_append(&env_->vals, C_obj_copy(olist_get(vals, i + 1))) ;
	}
	return 0 ;
}

int env_bind_direct(obj_t * env, obj_t * ident, obj_t * value) {

	D_obj_repr(env) ;

	assert(obj_typeof(env) == T_ENV) ;

	env_t * env_ = (env_t *)env ;

	olist_del(&env_->idents, 0) ;
	olist_del(&env_->vals, 0) ;

	if (!olist_insert(&env_->idents, ident, 0)) {
		return 1 ;
	}

	obj_t * tmp = value ;

	if (!olist_insert(&env_->vals, tmp, 0)) {
		return 1 ;
	}

	return 0 ;
}

/* check if an environment maps a variable to a value */
bool env_maps(obj_t * env, obj_t * ident) {
	/* if (!env) { */
	/* 	return false ; */
	/* } */	
	assert(env) ;
	if (env_empty(env)) { /* shortcut */
		return false ;
	}

	env_t * env_ = (env_t *)env ;
	ident_t * tmp ;
	for (size_t i = 0; i < olist_length(env_->idents); ++i) {
		tmp = (ident_t *)olist_get(env_->idents, i) ;	
		if (!ident_cmp(ident, (obj_t *)tmp)) {
			return true ;
		}
	}
	return false ;
}
/* Do the substitution, consume the identifier, return a copy of the mapped value */
obj_t * env_subst(obj_t * env, obj_t * ident) {
	assert(env) ;
	env_t * env_ = (env_t *)env ;
	ident_t * tmp ;
	/* start at one because zero is the func name */
	for (size_t i = 0; i < olist_length(env_->idents); ++i) {
		tmp = (ident_t *)olist_get(env_->idents, i) ;	
		if (!ident_cmp(ident, (obj_t *)tmp)) {
			/* if we found it, return the corresponding value */
			D_OBJ(ident) ;
			return C_obj_copy(olist_get(env_->vals, i)) ;
		}
	}
	/* yeah idk the caller is going to have to figure this one out */
	/* or I could return the ident... */
	D_OBJ(ident) ;
	return NULL ;
}

obj_t * env_inc_ref(obj_t * env) {
	assert(obj_typeof(env) == T_ENV) ;
	++((env_t *)env)->refcnt ;
	return env ;
}

void env_dec_ref(obj_t ** env_ptr) {
	assert(env_ptr) ;
	assert(obj_typeof(*env_ptr) == T_ENV) ;
	if (--(*(env_t **)env_ptr)->refcnt <= 0) {
		D_env(env_ptr) ;
	}
}

int env_get_ref(obj_t * env)  {
	assert(obj_typeof(env) == T_ENV) ;
	return ((env_t *)env)->refcnt ;
}

bool env_empty(obj_t * env) {
	assert(obj_typeof(env) == T_ENV) ;
	return olist_length(((env_t *)env)->idents) <= 0 ;
}

char * env_get_name(obj_t * env) {
	if (!env || env_empty(env)) {
		return "(none)" ;
	}
	else {
		return ident_get_name(olist_get(((env_t *)env)->idents, 0)) ;
	}
}

void env_print(obj_t * env) {
	(void)env ;
	/* deprecated but I may reuse this funciton */
	/* printf("Env: %s\n", env_get_name(env)) ; */
}

obj_t * C_clo(obj_t * lam, obj_t * env) {
	ALLOC_OR_RETNULL(new, clo_t) ;
	*new = CLO_INIT(C_obj_copy(lam), C_obj_copy(env)) ;

	/* note: this overwrites the first item in the list if it exists */
	env_bind_direct(new->env, C_obj_copy(lam_get_recname(lam)), (obj_t *)new) ;
	return (obj_t *)new ;
}

obj_t * clo_inc_ref(obj_t * clo) {
	assert(obj_typeof(clo) == T_CLO) ;
	++((clo_t *)clo)->refcnt ;
	return clo ;
}

void clo_dec_ref(obj_t ** clo_ptr) {
	assert(clo_ptr) ;
	assert(obj_typeof(*clo_ptr) == T_CLO) ;
	if (--(*(clo_t **)clo_ptr)->refcnt <= 0) {
		D_clo(clo_ptr) ;
	}
}

int clo_get_ref(obj_t * clo)  {
	assert(obj_typeof(clo) == T_CLO) ;
	return ((clo_t *)clo)->refcnt ;
}

obj_t * clo_get_lam(obj_t * clo) {
	assert(obj_typeof(clo) == T_CLO) ;
	return ((clo_t *)clo)->lam ;
}
obj_t * clo_get_env(obj_t * clo) {
	assert(obj_typeof(clo) == T_CLO) ;
	return env_inc_ref(((clo_t *)clo)->env) ;
}

obj_t * clo_get_env_noref(obj_t * clo) {
	assert(obj_typeof(clo) == T_CLO) ;
	return ((clo_t *)clo)->env ;
}

void gen_repr_clo(obj_t * obj) {
	static char part1[] = "[" ;
	/* static char part2[] = " " ; */
	static char part3[] = "]" ;
	char * lam_repr /*, * env_repr */ ; 
	obj_t * env ;
	      /* * recname ; */

	lam_repr = obj_repr(clo_get_lam(obj)) ;
	env = C_obj_copy(clo_get_env_noref(obj)) ;
	/* recname = C_obj_copy((obj_t *)env_get_ident(env, 0)) ; */
	/* env_bind_direct(env, recname, C_str("Self")) ; */

	/* olist_del(&((env_t *)env)->idents, 0) ; */
	olist_del(&((env_t *)env)->vals, 0) ;


	/* olist_insert(&((env_t *)env)->idents, recname, 0) ; */
	olist_insert(&((env_t *)env)->vals, C_str("Self"), 0) ;

	D_obj_repr(env) ;

	/* env_repr = obj_repr(env) ; */

	obj->head.repr_size =
		  sizeof(part1)
		+ obj_repr_size(clo_get_lam(obj))
		/* + sizeof(part2) */ 	/* I'm not sure how I feel about these, they make the output too thick */
		/* + obj_repr_size(env) */
		+ sizeof(part3) ;

	obj->head.repr = (char *)malloc(obj->head.repr_size + 1) ;
	
	if (obj->head.repr) {
		strcpy(obj->head.repr, part1) ;
		strcat(obj->head.repr, lam_repr) ;
		/* strcat(obj->head.repr, part2) ; */
		/* strcat(obj->head.repr, env_repr) ; */
		strcat(obj->head.repr, part3) ;
		obj->head.repr[obj_repr_size(obj)] = '\0' ;
	}
	
	D_OBJ(env) ;

}

void D_clo(obj_t ** clo_ptr) {
	assert(clo_ptr) ;
	assert(*clo_ptr) ;
	clo_t * clo = *(clo_t **)clo_ptr ;
	D_obj_repr(*clo_ptr) ;
	D_OBJ(clo->lam) ;
	/* we remove the self-reference in the environment by removing it before freeing the env */
	olist_pop_index(&(((env_t *)clo->env)->vals), 0) ;
	D_OBJ(clo->env) ;
	D_OBJ(clo->env_orig) ;
	free(clo) ;
	*clo_ptr = NULL ;
}

char * frame_get_name(obj_t * frame) {
	assert(obj_isframe(frame)) ;
	switch(obj_typeof(frame)) {
	case T_FRRET:
		return "return" ;
	case T_FRAPP:
		return "application" ;
	case T_FRIF:
		return "ifexpr" ;
	default:
		return "Big Error! this should not be returned!!!!!" ;
	}
}
char * frame_get_env_name(obj_t * frame) {
	assert(obj_isframe(frame)) ;
	switch(obj_typeof(frame)) {
	case T_FRRET:
		return env_get_name(((frret_t *)frame)->env) ;
	case T_FRAPP:
		return env_get_name(((frapp_t *)frame)->env) ;
	case T_FRIF:
		return env_get_name(((frif_t *)frame)->env) ;
	default:
		return "Big Error! this should not be returned!!!!!" ;
	}
}

/* idents in env */
size_t env_length(obj_t * env) {
	return olist_length(((env_t*)env)->idents) ;
}

/* vals in env */
size_t env_girth(obj_t * env) {
	return olist_length(((env_t*)env)->vals) ;
}

/* INVARIANT: idents are of type ident */
ident_t * env_get_ident(obj_t * env, size_t index) {
	if (index >= olist_length(((env_t *)env)->idents)) {
		return NULL ;
	}
	else {
		return (ident_t *)olist_get(((env_t *)env)->idents, index) ;
	}
}
obj_t * env_get_val(obj_t * env, size_t index) {
	if (index >= olist_length(((env_t *)env)->vals)) {
		return NULL ;
	}
	else {
		return olist_get(((env_t *)env)->vals, index) ;
	}
}
