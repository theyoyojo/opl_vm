#include "delta.h"


obj_t * delta_plus(num_t * first, num_t * second) {
	return C_num(first->value + second->value) ;
}

obj_t * delta_mult(num_t * first, num_t * second) {
	return C_num(first->value * second->value) ;
}

obj_t * delta_div(num_t * first, num_t * second) {
	/* Divison by zero is undefined */
	if (second->value == 0) {
		/* TODO: maybe some better error handling? */
		return NULL ;
	}
	return C_num(first->value / second->value) ;
}

obj_t * delta_sub(num_t * first, num_t * second) {
	return C_num(first->value - second->value) ;
}

obj_t * delta_lteq(num_t * first, num_t * second) {
	return C_bool(first->value <= second->value) ;
}

obj_t * delta_lt(num_t * first, num_t * second) {
	return C_bool(first->value < second->value) ;
}

obj_t * delta_eq(num_t * first, num_t * second) {
	return C_bool(first->value == second->value) ;
}

obj_t * delta_gt(num_t * first, num_t * second) {
	return C_bool(first->value >= second->value) ;
}

obj_t * delta_gteq(num_t * first, num_t * second) {
	return C_bool(first->value > second->value) ;
}

obj_t * (*dtable_binary_num_num[])(num_t *, num_t *) = {
	[PRIM_PLUS] = delta_plus,
	[PRIM_MULT] = delta_mult,
	[PRIM_DIV]  = delta_div,
	[PRIM_SUB]  = delta_sub,
	[PRIM_LTEQ] = delta_lteq,
	[PRIM_LT]   = delta_lt,
	[PRIM_EQ]   = delta_eq,
	[PRIM_GT]   = delta_gt,
	[PRIM_GTEQ] = delta_gteq,
} ;

obj_t * delta(obj_t * obj) {
	app_t * app ;
	/* All j1 deltas are binary operations on the 
	 * second and third _value_ elements of applicaiton lists */
	if (obj->head.type != T_APP) {
		return NULL ;
	}

	app = (app_t *)obj ;
	if (olist_length(app->expr_list) != 3) {
		return NULL ;
	}
	/*  Additonally, the first better be a prim and the latter two nums */
	else if (olist_get(app->expr_list,0)->head.type != T_PRIM ||
		 olist_get(app->expr_list,1)->head.type != T_NUM  ||
		 olist_get(app->expr_list,2)->head.type != T_NUM) {
		return NULL ;
	}
	/* Final validation: the prim must not be invalid */
	else if (((prim_t *)olist_get(app->expr_list,0))->value == PRIM_INVALID) {
		return NULL ;
	}

	/* now at last, we can lookup the specific delta function and call it */
	prim_t * prim = (prim_t *)olist_get(app->expr_list,0) ;
	num_t * first = (num_t *)olist_get(app->expr_list,1) ;
	num_t * second = (num_t *)olist_get(app->expr_list,2) ;
	
	return dtable_binary_num_num[prim->value](first, second) ;
}
