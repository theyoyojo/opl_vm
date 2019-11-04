#include "stack.h"
#include "types.h"
#include <string.h>

obj_t * C_stack() {
	ALLOC_OR_RETNULL(new, stack_t) ;
	new->head = HEADER_INIT(T_STACK, D_stack, C_stack_copy) ;
	new->data = olist_init() ;
	return (obj_t *)new ;
}

obj_t * C_stack_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, stack_t) ;
	new->head = HEADER_INIT(T_STACK, D_stack, C_stack_copy) ;
	new->data = olist_init_copy(((stack_t *)old)->data) ;
	return (obj_t *)new ;
}

void D_stack(obj_t ** stack_ptr) {
	assert(stack_ptr) ;
	assert(*stack_ptr) ;
	olist_free(&((stack_t *)*stack_ptr)->data) ;
	free(*stack_ptr) ;
	*stack_ptr = NULL ;
}

void stack_push(obj_t * stack, obj_t * frame) {
	assert(obj_typeof(stack) == T_STACK) ;
	assert(obj_isframe(frame)) ;
	
	olist_insert(((stack_t *)stack)->data, frame, 0) ;
}

void stack_chop(obj_t * stack) {
	assert(obj_typeof(stack) == T_STACK) ;
	stack_t * _stack = (stack_t *)stack ;
	if (olist_empty(((stack_t *)stack)->data)) {
		return ;
	}
	else {
		olist_del(_stack->data, 0) ;
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

/* This function does not return */
void stack_trace(obj_t * stack) {
	assert(obj_typeof(stack) == T_STACK) ;
	stack_t * stack_ = (stack_t *)stack ;
		
	obj_t * tmp ;
	printf("Stack trace of aborted future plans:\n") ;

	for (size_t i = 0; i < olist_length(stack_->data); ++i) {
		tmp = stack_top(stack) ;
		printf("\t%s %p: frame: %s env: %s\n",
			i == 0 ? "at" : "by",
			tmp, frame_get_name(tmp), frame_get_env_name(tmp)) ;
		frame_print(tmp) ;
		stack_chop(stack) ;
	}

	printf("===[TERMINATION IMMINENT]===\n") ;

	/* I could clean the main function's memory with a callback here... */
	exit(1) ;
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
	}

	return env_inc_ref(*env_ptr) ;
	
}

obj_t * C_frif(obj_t * ifexpr, obj_t * env) {
	assert(obj_typeof(ifexpr) == T_IF) ;
	ALLOC_OR_RETNULL(new, frif_t) ;
	new->head = HEADER_INIT(T_FRIF, D_frif, C_frif_copy) ;
	new->e_true = C_obj_copy(((if_t *)ifexpr)->expr_true) ;
	new->e_false = C_obj_copy(((if_t *)ifexpr)->expr_false) ;
	new->env = env_inc_ref(env) ;
	return (obj_t *)new ;
}

obj_t * C_frif_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, frif_t) ;
	new->head = HEADER_INIT(T_FRIF, D_frif, C_frif_copy) ;
	new->e_true = C_obj_copy(((frif_t *)old)->e_true) ;
	new->e_false = C_obj_copy(((frif_t *)old)->e_false) ;
	new->env = env_inc_ref(((frif_t *)old)->env) ;
	return (obj_t *)new ;
}

void D_frif(obj_t ** frif_ptr) {
	assert(frif_ptr) ;
	assert(*frif_ptr) ;
	frif_t * frif = *(frif_t **)frif_ptr ;
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
	new->head = HEADER_INIT(T_FRAPP, D_frapp, C_frapp_copy) ;
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
	new->head = HEADER_INIT(T_FRAPP, D_frapp, C_frapp_copy) ;
	new->vals = olist_init_copy(((frapp_t *)old)->vals) ;
	new->exprs = olist_init_copy(((frapp_t *)old)->exprs) ;
	/* for (size_t i = 0; i < olist_length(new->exprs); ++i) { */
	new->env = env_inc_ref(((frapp_t *)old)->env) ;
	/* } */
	return (obj_t *)new ;
}
void D_frapp(obj_t ** frapp_ptr) {
	assert(frapp_ptr) ;
	assert(*frapp_ptr) ;
	frapp_t * frapp = *(frapp_t **)frapp_ptr ;
	olist_free(&frapp->vals) ;
	olist_free(&frapp->exprs) ;
	D_OBJ(frapp->env) ;
	free(frapp) ;
	*frapp_ptr = NULL ;
}


obj_t * frapp_pop_expr(obj_t * frapp) {
	assert(obj_typeof(frapp) == T_FRAPP) ;
	/* obj_t * tmp ; */
	return olist_pop_index(((frapp_t *)frapp)->exprs, 0) ;
	/* olist_del(((frapp_t *)frapp)->exprs, 0) ; */
	/* return tmp ; */
}

void frapp_push_value(obj_t * frapp, obj_t * obj) {
	assert(obj_typeof(frapp) == T_FRAPP) ;
	olist_append(((frapp_t *)frapp)->vals, obj) ;
}

bool frapp_has_more_exprs(obj_t * frapp) {
	assert(obj_typeof(frapp) == T_FRAPP) ;
	return !olist_empty(((frapp_t *)frapp)->exprs) ;
}

olist_t * frapp_get_vals(obj_t * frapp) {
	return ((frapp_t *)frapp)->vals ;
}

obj_t * frapp_get_first_value(obj_t * frapp) {
	return olist_get(frapp_get_vals(frapp),0) ;
}

obj_t * C_frret(obj_t * env) {
	ALLOC_OR_RETNULL(new, frret_t) ;
	new->head = HEADER_INIT(T_FRRET, D_frret, C_frret_copy) ;
	new->env = env_inc_ref(env) ;
	return (obj_t *)new ;
}

obj_t * C_frret_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, frret_t) ;
	new->head = HEADER_INIT(T_FRRET, D_frret, C_frret_copy) ;
	new->env = env_inc_ref(((frret_t *)old)->env) ;
	return (obj_t *)new ;
}
void D_frret(obj_t ** frret_ptr) {
	assert(frret_ptr) ;
	assert(*frret_ptr) ;
	D_OBJ(((frret_t*)*frret_ptr)->env) ;
	free(*frret_ptr) ;
	*frret_ptr = NULL ;
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
	new->head = HEADER_INIT(T_ENV, env_dec_ref, C_env_copy) ;
	new->idents = olist_init_copy(old_env->idents) ;
	new->vals = olist_init_copy(old_env->vals) ;
	new->refcnt = 1 ;
	return (obj_t *)new ;
}

void D_env(obj_t ** env_ptr) {
	assert(env_ptr) ;
	assert(*env_ptr) ;
	env_t * env = *(env_t **)env_ptr ;
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
	size_t vallen = olist_length(vals) ;
	static unsigned char index_memo[MAX_BIND_AT_ONCE] ;
	memset(index_memo, 0, MAX_BIND_AT_ONCE) ;
	env_t * env_ = (env_t *)env ;

	if (bindlen > MAX_BIND_AT_ONCE) {
		printf("Exception: Binding too long to handle in one go\n"
				"\tlength: %lu, max: %lu\n",
				bindlen, MAX_BIND_AT_ONCE) ;
		return 1 ;
	}
	if (bindlen != vallen) {
		printf("Exception: Invalid environment binding: \n"
				"\tident count: %lu, differes from value count: %lu\n",
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
				olist_del(env_->vals, j);
				olist_insert(env_->vals,
						C_obj_copy(olist_get(vals, i)),j) ;
				index_memo[i] = 1 ;
				break ;
			}
		}
		if (index_memo[i]) continue ; /* skip overwritten */

		olist_append(env_->idents, C_obj_copy(olist_get(binding, i))) ;
		olist_append(env_->vals, C_obj_copy(olist_get(vals, i))) ;
	}
	return 0 ;
}

int env_bind_direct(obj_t * env, obj_t * ident, obj_t * value) {
	assert(obj_typeof(env) == T_ENV) ;

	env_t * env_ = (env_t *)env ;

	if (!olist_append(env_->idents, ident)) {
		return 1 ;
	}

	obj_t * tmp = value ;

	if (!olist_append(env_->vals, tmp)) {
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
	
	/* obj_t * tmp ; */
	/* for (size_t i = 0; i < olist_length((*(env_t**)env_ptr)->vals); ++i) { */
	/* 	tmp = olist_get((*(env_t**)env_ptr)->vals, i) ; */
	/* 	if (obj_typeof(tmp) == T_ENV) { */
	/* 		env_dec_ref(&tmp) ; */
	/* 	} */
	/* } */

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
	printf("Env: %s\n", env_get_name(env)) ;
	if (!env || env_empty(env)) {
		printf("| (empty)\n") ;
		return ;
	}
	for (size_t i = 0; i < olist_length(((env_t *)env)->idents); ++i) {
		printf("| env(%s) \t= ", ((ident_t *)olist_get(((env_t *)env)->idents, i))->value) ;
		value_print(olist_get(((env_t *)env)->vals,i)) ;
		printf("\n") ;
	}
}

obj_t * C_clo(obj_t * lam, obj_t * env, bool self_bind) {
	ALLOC_OR_RETNULL(new, clo_t) ;
	*new = CLO_INIT(C_obj_copy(lam), C_obj_copy(env)) ;
	/* self-bondage */
	if (self_bind) {
		env_bind_direct(new->env, C_obj_copy(lam_get_recname(lam)), C_clo(lam, env, false)) ;
	}
	new->refcnt++ ;
	/* env_dec_ref((obj_t **)&new->env) ; */
	return (obj_t *)new ;
}

obj_t * C_clo_copy(obj_t * old) {
	clo_t * old_ = (clo_t *)old ;
	ALLOC_OR_RETNULL(new, clo_t) ;
	*new = CLO_INIT(C_obj_copy(old_->lam), C_obj_copy(old_->env_orig)) ;
	env_bind_direct(new->env, C_obj_copy(lam_get_recname(new->lam)), C_clo(new->lam, new->env, false)) ;
	return (obj_t *)new ;
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

int clo_get_ref(obj_t * clo) {
	assert(obj_typeof(clo) == T_CLO) ;
	return ((clo_t *)clo)->refcnt ;
}

void D_clo(obj_t ** clo_ptr) {
	assert(clo_ptr) ;
	assert(*clo_ptr) ;
	clo_t * clo = *(clo_t **)clo_ptr ;
	D_OBJ(clo->lam) ;
	D_OBJ(clo->env) ;
	D_OBJ(clo->env_orig) ;
	/* obj_t * tmp ; */
	/* if (env_get_ref(clo->env) == 1) { /1* unhook the self-reference *1/ */
	/* 	tmp = env_get_val(clo->env, 0) ; */
	/* 	D_OBJ(((clo_t*)tmp)->lam) ; */
	/* 	((clo_t*)tmp)->env = NULL ; */	
	/* 	D_OBJ(clo->env) ; */
	/* 	/1* this is a disgusting hack that doesn't even work *1/ */
	/* } */
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

void frame_print(obj_t * frame) {
	assert(frame) ;
	assert(obj_isframe(frame)) ;
	printf("Frame: %s\n", frame_get_name(frame)) ;
	switch(obj_typeof(frame)) {
	case T_FRAPP:
		printf("| vals: ") ;
		for (size_t i = 0; i < olist_length(((frapp_t *)frame)->vals); ++i) {
			value_print(olist_get(((frapp_t*)frame)->vals, i)) ;
			putchar(' ') ;
		}
		printf("\n| exprs: ") ;
		for (size_t i = 0; i < olist_length(((frapp_t *)frame)->exprs); ++i) {
			expr_print(olist_get(((frapp_t*)frame)->exprs, i)) ;
			putchar(' ') ;
		}
		break ;
	case T_FRIF:
		printf("| e_true: ") ;
		expr_print(((frif_t*)frame)->e_true) ;
		printf("\n| e_false: ") ;
		expr_print(((frif_t*)frame)->e_false) ;
	case T_FRRET: /* printing name is all we do for a frret */
		break ;
	default:
		printf("Big Error! this should not be seen!!!!") ;
	}

	printf("\n") ;
}

/* idents in env */
size_t env_length(obj_t * env) {
	return olist_length(((env_t*)env)->idents) ;
}

/* vals in env */
size_t env_girth(obj_t * env) {
	return olist_length(((env_t*)env)->vals) ;
}

/* INVARIANT: idents are of type indent */
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

obj_t * C_clorf(obj_t * clo) {
	ALLOC_OR_RETNULL(new, clorf_t) ;
	*new = CLORF_INIT((clo_t*)clo_inc_ref(clo)) ;
	return (obj_t *)new ;
}

obj_t * C_clorf_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, clorf_t) ;
	*new = CLORF_INIT((clo_t*)clo_inc_ref(clorf_deref(old))) ;
	return (obj_t *)new ;
}

void D_clorf(obj_t ** clorf_ptr) {
	assert(clorf_ptr) ;
	assert(*clorf_ptr) ;
	clo_dec_ref((obj_t **)&((clorf_t *)*clorf_ptr)->ref) ;
	free(*clorf_ptr) ;
	*clorf_ptr = NULL ;
}

obj_t * clorf_deref(obj_t * clorf) {
	return clo_inc_ref((obj_t *)((clorf_t *)clorf)->ref) ;
}
