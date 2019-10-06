#include "interp.h"

#include <string.h>

#include "stack.h"
#include "types.h"
#include "delta.h"

void boolify_if_not_already_bool(obj_t ** code, obj_t ** tmp) {
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

/* must consume original expr */
/* (e ... ID:x ...)[ID:x <- v]) => (e ... v ...) */
obj_t * subst_expr(obj_t * expr, obj_t * ident, obj_t * value) {
	assert(obj_isvalue(value)) ;
	assert(obj_typeof(ident) == T_IDENT) ;

	obj_t * tmp1,
	      * tmp2,
	      * tmp3 ;
	olist_t * tmp_list ;
	switch(obj_typeof(expr)) {
		case T_IDENT:
			/* x[x <- v] => v  */
			if (!ident_cmp(ident, expr)) {
				D_OBJ(expr) ;
				return C_obj_copy(value) ;
			}
			/* y[x <- v] => y*/
			else {
				/* no change is the same as consumption */
				tmp1 = C_obj_copy(expr) ;
				D_OBJ(expr) ;
				return tmp1;
			}
		case T_NUM:
		case T_BOOL:
		case T_PRIM:
			/* v'[x <- v] => v' */
			tmp1 = C_obj_copy(expr) ;
			D_OBJ(expr) ;
			return tmp1;
		case T_IF:
			tmp1 = C_obj_copy(if_get_pred(expr)) ;
			tmp1 = subst_expr(tmp1, ident, value) ;
			tmp2 = C_obj_copy(if_get_true(expr)) ;
			tmp2 = subst_expr(tmp2, ident, value) ;
			tmp3 = C_obj_copy(if_get_false(expr)) ;
			tmp3 = subst_expr(tmp3, ident, value) ;
			tmp1 = C_if(tmp1, tmp2, tmp3) ;
			D_OBJ(expr) ;
			return tmp1 ;
		case T_APP:
			/* here we use the list we delared above */
			tmp_list = olist_init() ;
			for (size_t i = 0; i < olist_length(app_get_expr_list(expr)); ++i) {
				tmp1 = C_obj_copy(olist_get(app_get_expr_list(expr), i)) ;
				tmp1 = subst_expr(tmp1, ident, value) ;
				/* tmp1 is now owned by the list, later the new app */
				olist_append(tmp_list, tmp1) ;
			}
			/* we have gotten all the exprs out of the list so we can delete it */
			D_OBJ(expr) ;
			return C_app_list(tmp_list) ;
		default:
			/* error ? */
			printf("Something went wrong in subst_expr, pls debug\n") ;
			assert(0) ;
	}
}

/* (define (f x ...) e), (f v ...) => e'[x... <- v...] */
obj_t * subst_func(obj_t * func, obj_t * call) {
	obj_t * tmp_arg,
	      * tmp_ident ;
	olist_t * binding = func_get_binding(func) ;

	/* must copy expr because we will be overwriting it */
	obj_t * expr = C_obj_copy(func_get_expr(func)) ;
	olist_t * args = frapp_get_vals(call) ;

	if (olist_length(binding) != olist_length(args)) {
		printf("Error: arity mismatch at call to %s\n"
				"\texpected: %lu, got: %lu\n",
				func_get_name(func),
				olist_length(binding) - 1,
				olist_length(args) - 1) ;
		assert(0) ;
	}

	for (size_t i = 1; i < olist_length(binding); ++i) {
		tmp_arg = olist_get(args, i) ;
		tmp_ident = olist_get(binding, i) ;
		expr = subst_expr(expr, tmp_ident, tmp_arg) ;
	}
	return expr ;
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

/* CK1 Interpreter */
obj_t * interpret(obj_t * program) {
	obj_t * stack = C_stack() ;
	obj_t * code = C_obj_copy(program) ;
	obj_t * tmp = NULL ;
	olist_t * sigma = olist_init() ;
	size_t depth = 0 ;

	while (1) {
		/* <v, KRet> => halt */
		if (obj_isvalue(code)) {
			if (stack_empty(stack)) {
				/* It's over. */
				break  ;
			}
			else if (obj_typeof(stack_top(stack)) == T_FRIF) {
				/* If the code isn't a bool, make it so */
				boolify_if_not_already_bool(&code, &tmp) ;
				if (((bool_t *)code)->value) {
					/* <true, Kif[Et, Ef, K] => <Et,K> */
					tmp = frif_copy_true(stack_top(stack)) ;
				}
				else {
					/* <false, Kif[Et, Ef, K] => <Ef,K> */
					tmp = frif_copy_false(stack_top(stack)) ;

				}
				stack_chop(stack) ;
				--depth ;
				D_OBJ(code) ;
				code = tmp ;
			}
			else if (obj_typeof(stack_top(stack)) == T_FRAPP) {
				if (frapp_has_more_exprs(stack_top(stack))) {
				/* <Vn, Kapp[V1,..., En,En+1..., K]> =>  <En, Kapp[V1,...,Vn,En+1,..., K]> */
					frapp_push_value(stack_top(stack), code) ;
					D_OBJ(code) ;
					code = frapp_pop_expr(stack_top(stack)) ;
			   	}
				else {
				/* <Vn, Kapp[V1,..Vn, K]> => <delta(V1,... Vn), K> */
					frapp_push_value(stack_top(stack), code) ;
					D_OBJ(code) ;
					tmp = sigma_lookup(sigma, frapp_get_first_value(stack_top(stack))) ;
					if (tmp) {
						code = subst_func(tmp, stack_top(stack)) ;
					}
					else {
						code = delta_frapp(stack_top(stack)) ;
					}
					stack_chop(stack) ;
					--depth ;
				}
				
			}
			else {
				/* we should never reach this */
				printf("Mystery error???? :)\n") ;
				break ;
			}

		}
		/* <(if Ec Et Ef), K> => <Ec, Kif[Et, Ef, K]> */
		else if (obj_typeof(code) == T_IF) {
			tmp = if_copy_pred(code) ;
			stack_push(stack, C_frif(code)) ;
			++depth ;
			D_OBJ(code) ;
			code = tmp ;
		}
		/* <(E1 E2 ..), K> => <E1, Kapp[E2, ..., K]> */
		else if (obj_typeof(code) == T_APP) {
			stack_push(stack, C_frapp(code)) ;
			++depth ;
			D_OBJ(code) ;
			code = frapp_pop_expr(stack_top(stack)) ;
		}
		/* sigma[f] = (define (f x ..) e ) */
		else if (obj_typeof(code) == T_FUNC) {
			/* if (depth > 0) { */
				printf("Error: enountered non-top level function\n") ;
				/* break ; */
			/* } */
			/* else { */
			/* 	olist_append(sigma, code) ; */
			/* } */
		/* } */
		}
		else if (obj_typeof(code) == T_PROG) {
			/* add all funtion defs to sigma */
			for (size_t i = 0; olist_length(prog_get_topforms(code)); ++i) {
				tmp = olist_get(prog_get_topforms(code),i) ;
				if (obj_typeof(tmp) == T_FUNC) {
					olist_append(sigma, C_obj_copy(tmp)) ;
				}
				else {
					/* reach expr, set code to that and trash the prog structure */
					tmp = C_obj_copy(tmp) ;	
					D_OBJ(code) ;
					code = tmp ;
					break ;
				}

			}
		}
		else {
			/* we should never reach this */
			printf("Mystery error???? :)\n") ;
			break ;
		}
	}

	D_OBJ(stack) ;
	olist_free(&sigma) ;

	return code ;
}
