#include "stack.h"
#include "types.h"

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

obj_t * stack_top(obj_t * stack) {
	assert(obj_typeof(stack) == T_STACK) ;
	return olist_get(((stack_t *)stack)->data, 0) ;
}

obj_t * C_frif(obj_t * ifexpr) {
	assert(obj_typeof(ifexpr) == T_IF) ;
	ALLOC_OR_RETNULL(new, frif_t) ;
	new->head = HEADER_INIT(T_FRIF, D_frif, C_frif_copy) ;
	new->e_true = C_obj_copy(((if_t *)ifexpr)->expr_true) ;
	new->e_false = C_obj_copy(((if_t *)ifexpr)->expr_false) ;
	return (obj_t *)new ;
}

obj_t * C_frif_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, frif_t) ;
	new->head = HEADER_INIT(T_FRIF, D_frif, C_frif_copy) ;
	new->e_true = C_obj_copy(((frif_t *)old)->e_true) ;
	new->e_false = C_obj_copy(((frif_t *)old)->e_false) ;
	return (obj_t *)new ;
}

void D_frif(obj_t ** frif_ptr) {
	assert(frif_ptr) ;
	assert(*frif_ptr) ;
	frif_t * frif = *(frif_t **)frif_ptr ;
	D_obj(frif->e_true)(&frif->e_true) ;
	D_obj(frif->e_false)(&frif->e_false) ;
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

obj_t * C_frapp(obj_t * app) {
	assert(obj_typeof(app) == T_APP) ;
	ALLOC_OR_RETNULL(new, frapp_t) ;
	new->head = HEADER_INIT(T_FRAPP, D_frapp, C_frapp_copy) ;
	new->vals = olist_init() ;
	new->exprs = olist_init_copy(((app_t *)app)->expr_list) ;
	return (obj_t *)new ;
}
obj_t * C_frapp_copy(obj_t * old) {
	ALLOC_OR_RETNULL(new, frapp_t) ;
	new->head = HEADER_INIT(T_FRAPP, D_frapp, C_frapp_copy) ;
	new->vals = olist_init_copy(((frapp_t *)old)->vals) ;
	new->exprs = olist_init_copy(((frapp_t *)old)->exprs) ;
	return (obj_t *)new ;
}
void D_frapp(obj_t ** frapp_ptr) {
	assert(frapp_ptr) ;
	assert(*frapp_ptr) ;
	frapp_t * frapp = *(frapp_t **)frapp_ptr ;
	olist_free(&frapp->vals) ;
	olist_free(&frapp->exprs) ;
	free(frapp) ;
	*frapp_ptr = NULL ;
}


obj_t * frapp_pop_expr(obj_t * frapp) {
	assert(obj_typeof(frapp) == T_FRAPP) ;
	obj_t * tmp ;
	tmp = C_obj_copy(olist_get(((frapp_t *)frapp)->exprs, 0)) ;
	olist_del(((frapp_t *)frapp)->exprs, 0) ;
	return tmp ;
}

void frapp_push_value(obj_t * frapp, obj_t * obj) {
	assert(obj_typeof(frapp) == T_FRAPP) ;
	olist_append(((frapp_t *)frapp)->vals,C_obj_copy(obj)) ;
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

obj_t * C_env(void) {
	ALLOC_OR_RETNULL(new, env_t) ;
	*new = ENV_INIT() ;
	return (obj_t *)new ;

}

obj_t * C_env_copy(obj_t * old) {
	env_t * old_env = (env_t *)old ;
	ALLOC_OR_RETNULL(new, env_t) ;
	new->head = HEADER_INIT(T_ENV, D_env, C_env_copy) ;
	new->idents = olist_init_copy(old_env->idents) ;
	new->vals = olist_init_copy(old_env->vals) ;
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
/* can fail by arity mismatch */
int env_bind(obj_t * env, olist_t * binding, olist_t * vals) {
	assert(env) ;
	size_t bindlen = olist_length(binding) ;
	size_t vallen = olist_length(vals) ;
	env_t * env_ = (env_t *)env ;
	/* Arity mismatch */
	if (bindlen != vallen) {
		printf("Exception: arrity mismatch in call to %s\n"
				"\t Expected: %lu, Got: %lu\n",
				func_get_name(olist_get(binding, 1)),
				bindlen, vallen) ;
		return 1 ;
	}
	/* Include function name for possible usage later as env name*/
	for (size_t i = 0; i < bindlen; ++i) {
		olist_append(env_->idents, C_obj_copy(olist_get(binding, i))) ;
		olist_append(env_->vals, C_obj_copy(olist_get(vals, i))) ;
	}
	return 0 ;
}
/* check if an environment maps a variable to a value */
bool env_maps(obj_t * env, obj_t * ident) {
	assert(env) ;
	env_t * env_ = (env_t *)env ;
	ident_t * tmp ;
	/* start at one because zero is the func name */
	for (size_t i = 1; i < olist_length(env_->idents); ++i) {
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
	for (size_t i = 1; i < olist_length(env_->idents); ++i) {
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
