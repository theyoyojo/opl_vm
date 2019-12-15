#include "delta.h"

#include "mem.h"
#include <string.h>


obj_t * delta_plus(num_t * first, num_t * second) {
	return C_num(first->value + second->value) ;
}

obj_t * delta_mult(num_t * first, num_t * second) {
	return C_num(first->value * second->value) ;
}

obj_t * delta_div(num_t * first, num_t * second) {
	/* Divison by zero is undefined */
	if (second->value == 0) {
		return C_abort(C_str("Exception: divide by zero")) ;
	}
	return C_num(first->value / second->value) ;
}

obj_t * delta_sub(num_t * first, num_t * second) {
	return C_num(first->value - second->value) ;
}

obj_t * delta_mod(num_t * first, num_t * second) {
	return C_num((double)((long long)first->value % (long long)second->value)) ;
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
	return C_bool(first->value > second->value) ;
}

obj_t * delta_gteq(num_t * first, num_t * second) {
	return C_bool(first->value >= second->value) ;
}

obj_t * (*dtable_binary_num_num[])(num_t *, num_t *) = {
	[PRIM_PLUS] = delta_plus,
	[PRIM_MULT] = delta_mult,
	[PRIM_DIV]  = delta_div,
	[PRIM_SUB]  = delta_sub,
	[PRIM_MOD]  = delta_mod,
	[PRIM_LTEQ] = delta_lteq,
	[PRIM_LT]   = delta_lt,
	[PRIM_EQ]   = delta_eq,
	[PRIM_GT]   = delta_gt,
	[PRIM_GTEQ] = delta_gteq,
} ;

obj_t * delta_fst(obj_t * pair) {
	return C_obj_copy(pair_first(pair)) ;
}

obj_t * delta_snd(obj_t * pair) {
	return C_obj_copy(pair_second(pair)) ;
}

obj_t * delta_box(obj_t * obj) {
	return mem_alloc_init(obj) ;
}

obj_t * delta_unbox(obj_t * ptr) {
	return mem_deref(ptr) ;
}

obj_t * delta_print(obj_t * obj) {
	printf("%s\n", obj_repr(obj)) ;
	return C_unit() ;
}

obj_t * (*dtable_unary[])(obj_t *) = {
	[PRIM_FST]  	= delta_fst,
	[PRIM_SND]  	= delta_snd,
	[PRIM_BOX] 	= delta_box,
	[PRIM_UNBOX] 	= delta_unbox,
	[PRIM_PRINT]	= delta_print,
} ;

obj_t * delta_set_box(obj_t * ptr, obj_t * newval) {
	return mem_set(ptr, newval) ;
}

obj_t * delta_strcat(obj_t * str1, obj_t * str2) {
	if (obj_typeof(str1) != T_STR && obj_typeof(str2) != T_STR) {
		return C_abort(C_str("Error: cannot concat non-str objects")) ;
	}
	else {
		size_t newsize = str_size(str1) + str_size(str2) ;
		char * newstr = (char *)malloc(newsize + 1) ;
		strcpy(newstr, str_get(str1)) ;
		strcat(newstr, str_get(str2)) ;
		obj_t * new = C_str(newstr) ;
		free(newstr) ;
		return new ;
	}
}

obj_t * (*dtable_binary[])(obj_t *, obj_t *) = {
	[PRIM_SETBOX] 	= delta_set_box,
	[PRIM_PLUS]	= delta_strcat,	/* no type validation here btw */
} ;

obj_t * delta_frapp(obj_t * obj) {
	frapp_t * frapp ;
	prim_t * prim ;
	num_t * first, * second ;
	/* All J4 deltas are binary operations on the 
	 * second and third _value_ elements of applicaiton lists 
	 * reprsented as frapps with empty expr lists */
	if (obj_typeof(obj) != T_FRAPP) {
		return NULL ;
	}

	/* yes this allows bad operands to primitives but I'm not going to do any
	 * more sophisticated exception stuff until I implement exceptions
	 */

	frapp = (frapp_t *)obj ;
	/* primitives required */
	if (obj_typeof(olist_get(frapp->vals, 0)) != T_PRIM) {
		return NULL ;
	}

	switch (olist_length(frapp->vals)) {
	case 2:
		return dtable_unary[((prim_t *)olist_get(frapp->vals,0))->value]
				(olist_get(frapp->vals, 1)) ;

	case 3:
		if (obj_typeof(olist_get(frapp->vals, 1)) == T_NUM  &&
			 obj_typeof(olist_get(frapp->vals, 2)) == T_NUM) {
			prim = (prim_t *)olist_get(frapp->vals,0) ;
			first = (num_t *)olist_get(frapp->vals,1) ;
			second = (num_t *)olist_get(frapp->vals,2) ;
			/* now at last, we can lookup the specific delta function and call it */
			return dtable_binary_num_num[prim->value](first, second) ;
		}
		else {
			return dtable_binary[prim_get_val(olist_get(frapp->vals, 0))](olist_get(frapp->vals, 1), olist_get(frapp->vals, 2)) ;
		}
	default:
		return NULL ;
	}
}
