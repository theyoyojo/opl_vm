#include "interp.h"

#include <string.h>

#include "stack.h"
#include "types.h"
#include "delta.h"

static inline void boolify_if_not_already_bool(obj_t ** code, obj_t ** tmp) {
	if (obj_typeof(*code) != T_BOOL) {
		if (obj_typeof(*code) == T_NUM) {
			*tmp = C_bool(((num_t *)*code)->value) ;
		}
		else if (obj_typeof(*code) == T_PRIM) {
			*tmp = C_bool(((prim_t *)*code)->value) ;
		}
		D_OBJ(*code) ;
		*code = *tmp ;
	}
}

obj_t * sigma_lookup(olist_t * sigma, obj_t * ident) {
	if (obj_typeof(ident) != T_IDENT) {
		return NULL ;
	}

	obj_t * tmp ;
	for (size_t i = 0; i < olist_length(sigma); ++i) {
		tmp = olist_get(sigma, i) ;
		if (!strcmp(func_get_name(tmp), ident_get_name(ident))) {
			return tmp ;
		}
	}
	return NULL ;
}

#define endlessly_repeat while (++cycle_count || 1)

/* the cool uncle of interpret */
obj_t * exec(obj_t * program) {
	obj_t * code 	= C_obj_copy(program),  /* C */
	      * env 	= C_env(),  		/* E */
  	      * stack 	= C_stack(),   		/* K */
	      * tmp1 	= NULL ;		/* 0 machine */
	olist_t * sigma = olist_init() ;
	size_t cycle_count = 0 ;
	
	/* we need another tmp for dynamic scope */
	obj_t * tmp2 = NULL ;

	stack_push(stack, C_frret(env)) ;

	endlessly_repeat {			
		if (!code) goto panic ;
#ifdef DEBUG
		printf("===[C E K]===\n") ;
		printf("Cycle:\t%ld\n", cycle_count) ;
		env_print(env) ;
		printf("Code : ") ; expr_print(code); putchar('\n') ;
		frame_print(stack_top(stack)) ;
		printf("=============\n") ;
#endif 
		switch(obj_typeof(code)) {
		case T_IDENT:
			/* if in sigma, skip, else code = env(x) */
			if (!sigma_lookup(sigma, code)) {
				if (env_maps(env, code)) {
					code = env_subst(env, code) ;
					/* D_OBJ(env) ; /1* I think we D the env because we have a new ref to it *1/ */
					/* or is this done later anyway */
					continue ;
				} else {
					printf("Exception: unbound identifier %s\n",
							ident_get_name(code)) ;
					stack_trace(stack) ;
				}
			}
		case T_NUM:
		case T_BOOL:
		case T_PRIM:
			switch (obj_typeof(stack_top(stack))) {
			/* <v, 0, kret > => return v */
			case T_FRRET:
				tmp1 = NULL ;
				stack_chop(stack) ; /* there should only be one ref left to the env */
				goto success ;
			/* <t/f, 0, Kif(env, et, ef, K)> => <ef/ef, env, K> */
			case T_FRIF:
				D_OBJ(env) ; /* we don't care about our current env anymore */
				boolify_if_not_already_bool(&code, &tmp1) ;
				tmp1 = (((bool_t *)code)->value)
					? frif_copy_true(stack_top(stack))
					: frif_copy_false(stack_top(stack)) ;
				D_OBJ(code) ;
				code = tmp1 ;
				env = stack_top_env(stack) ; /* restore previous ifexpr env */
				stack_chop(stack) ;
				continue ;	
			case T_FRAPP:
			/* <v, 0, Kapp((v' ...), env, (e e'...), K> => <e, env, Kapp((v' v ...), env, (e' ...), K> */
				frapp_push_value(stack_top(stack), code) ;
				D_OBJ(code) ;
				D_OBJ(env) ;
				if (frapp_has_more_exprs(stack_top(stack))) {
					code = frapp_pop_expr(stack_top(stack)) ;
					env = stack_top_env(stack) ; /* get a new ref to the env */
				} else if ((tmp1 = sigma_lookup(sigma, frapp_get_first_value(stack_top(stack))))) {
					/* definition found in sigma */
					/* create a new environment for the function call */
					env = C_env() ;
					env_bind(env, func_get_binding(tmp1), frapp_get_vals(stack_top(stack))) ;
					/* for dynamic scope, I copy the caller's environment */
					tmp2 = stack_top_env(stack) ;	/* new ref */
					env_bind(env, ((env_t *)tmp2)->idents, ((env_t *)tmp2)->vals) ;
					D_OBJ(tmp2) ; /* delete tmp1's ref */

					/* set the code to point to the function expression object */
					code = C_obj_copy(func_get_expr(tmp1)) ;
					stack_chop(stack) ;
				} else if ((code = delta_frapp(stack_top(stack)))) {
					/* successfuly delta function */
					stack_chop(stack) ;
					env = stack_top_env(stack) ; /* restore previous env */
				} else {
					/* failed delta function */
					printf("Exception: undefined delta function \"") ;
					value_print(frapp_get_first_value(stack_top(stack))) ;
					printf("\"\n") ;
					stack_trace(stack) ;
							
				}
				continue ;
			default:
				printf("Exception: illegal stack object\n") ;
				stack_trace(stack) ;
			}
			/* <e e' ..., env, K> => <e, env, Kapp((), env, (e' ...), K> */
		case T_APP:
			stack_push(stack, C_frapp(code, env)) ; /* new ref to env */
			D_OBJ(code) ;
			code = frapp_pop_expr(stack_top(stack)) ;
			/* env unchanged */
			continue ;
		/* <if c t f, env, K> => <c, env, Kif(env, t,f, K)> */
		case T_IF:
			tmp1 = if_copy_pred(code) ;
			stack_push(stack, C_frif(code, env)) ; /* one new reference to env */
			D_OBJ(code) ;
			code = tmp1 ;
			/* env unchanged */
			continue ;
		case T_PROG:
			for (size_t i = 0; i < olist_length(prog_get_topforms(code)); ++i) {
				switch(obj_typeof(tmp1 = C_obj_copy(olist_get(prog_get_topforms(code), i)))) {
				case T_FUNC:
					olist_append(sigma, tmp1) ;
					continue ;
				default:
					D_OBJ(code) ;
					code = tmp1 ;
					goto doublebreak ;
				}
			}
		doublebreak:
			continue ;

		default :
			printf("Exception: mystery code\n") ;
			stack_trace(stack) ;
			
		}
	}


success:
	olist_free(&sigma) ;
	D_OBJ	(tmp1) 	;	/* machine 0 */
	D_OBJ	(stack) ;		/* K */
	D_OBJ	(env) 	;		/* E */
	return  (code) 	;		/* C */
panic:
	printf("Exception: the code has gone missing\n") ;
	stack_trace(stack) ;
	return NULL ; /* this line is never executed */
}

