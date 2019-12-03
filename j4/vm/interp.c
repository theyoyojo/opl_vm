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


#define endlessly_repeat while (++cycle_count || 1)
#define ARBITRARY_STACK_HEIGHT_LIMIT 1000

/* the cool uncle of interpret */
obj_t * exec(obj_t * program) {
	obj_t * code 	= C_obj_copy(program),  /* C */
	      * env 	= C_env(),  		/* E */
  	      * stack 	= C_stack(),   		/* K */
	      * tmp1 	= NULL ;		/* 0 machine */
	/* olist_t * tmplist = NULL ; */
	size_t cycle_count = 0 ;

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
		if (stack_height(stack) > ARBITRARY_STACK_HEIGHT_LIMIT) {
			printf("Exception: stack overflow\n") ;	
			stack_trace(stack) ;
		}
		switch(obj_typeof(code)) {
		case T_IDENT:
			if (env_maps(env, code)) {
				code = env_subst(env, code) ;
				continue ;
			} else {
				printf("Exception: unbound identifier %s\n",
						ident_get_name(code)) ;
				stack_trace(stack) ;
			}
		case T_NUM:
		case T_BOOL:
		case T_PRIM:
		case T_CLO:
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
				/* D_OBJ(code) ; */
				D_OBJ(env) ;
				if (frapp_has_more_exprs(stack_top(stack))) {
					code = frapp_pop_expr(stack_top(stack)) ;
					env = stack_top_env(stack) ; /* get a new ref to the env */
				} else if (obj_typeof(tmp1 = frapp_get_first_value(stack_top(stack))) == T_CLO) {
					/* the business section */
					/* restore the closure's environment and copy it 
					 * if I were to simply get a new reference to it,
					 * I would have dynamic scope. */
					env = C_obj_copy(clo_get_env_noref(tmp1)) ;
					/* copying an env will copy each element,
					 * including the closure, which increments the recfnt
					 * of the original env */

					/* the code continues in the lambda's expr */
					code = C_obj_copy(lam_get_expr(clo_get_lam(tmp1))) ;
					/* get the list of values in frapp */
					/* tmplist = olist_init_copy(frapp_get_vals(stack_top(stack))) ; */
					/* remove the closure itself from the value list */
					/* olist_del(tmplist, 0) ; */
					/* extend the enviroment (this may need backend work)
					 * I don't overwrite old values, I just append I think */
					/* this has been fixed, but not for single */
					env_bind(env, lam_get_binding(clo_get_lam(tmp1)),
							frapp_get_vals(stack_top(stack))) ;
					
					/* list is coipied so we remove the local instance */
					/* olist_free(&tmplist) ; */
					/* tmp1 = clo_get_env_noref(tmp1) ; */
					/* D_OBJ(tmp1) ; */
					/* we don't free the closure because it is free'd in stack_chop */
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
		case T_LAM:
			/* env freeze at closure creation time */
			tmp1 = C_clo(code, env, true) ;
			D_OBJ(code) ;
			code = tmp1 ;
			continue ;
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
		default :
			printf("Exception: mystery code\n") ;
			stack_trace(stack) ;
			
		}
	}


success:
	D_OBJ	(tmp1) 	;	/* machine 0 */
	D_OBJ	(stack) ;		/* K */
	D_OBJ	(env) 	;		/* E */
	return  (code) 	;		/* C */
panic:
	printf("Exception: the code has gone missing\n") ;
	stack_trace(stack) ;
	return NULL ; /* this line is never executed */
}

