#include "interp.h"

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

/* CK0 Interpreter */
obj_t * interpret(obj_t * program) {
	obj_t * stack = C_stack() ;
	obj_t * code = C_obj_copy(program) ;
	obj_t * tmp = NULL ;

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
					code = delta_frapp(stack_top(stack)) ;
					stack_chop(stack) ;
				}
				
			}
			else {
				/* we should never reach this */
				assert(false) ;
			}

		}
		/* <(if Ec Et Ef), K> => <Ec, Kif[Et, Ef, K]> */
		else if (obj_typeof(code) == T_IF) {
			tmp = if_copy_pred(code) ;
			stack_push(stack, C_frif(code)) ;
			D_OBJ(code) ;
			code = tmp ;
		}
		/* <(E1 E2 ..), K> => <E1, Kapp[E2, ..., K]> */
		else if (obj_typeof(code) == T_APP) {
			stack_push(stack, C_frapp(code)) ;
			D_OBJ(code) ;
			code = frapp_pop_expr(stack_top(stack)) ;
		}
		else {
			/* we should never reach this */
			assert(false) ;
		}
	}

	/* if (tmp) { */
	/* 	D_OBJ(tmp) ; */
	/* } */
	D_OBJ(stack) ;

	return code ;
}
