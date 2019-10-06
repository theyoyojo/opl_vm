#ifndef STACK_H
#define STACK_H

#include "obj.h"
#include "olist.h"

typedef struct _stack {
	header_t head ;
	olist_t * data ;
} stack_t ;

obj_t * C_stack() ;
obj_t * C_stack_copy(obj_t * old) ;
void D_stack(obj_t ** stack_ptr) ;

void stack_push(obj_t * stack, obj_t * frame) ;
void  stack_chop(obj_t * stack) ;
bool stack_empty(obj_t * stack) ;
obj_t * stack_top(obj_t * stack) ;

/* frame for an if */
typedef struct _frif {
	header_t head ;
	obj_t * e_true,
	      * e_false ;
} frif_t ;


obj_t * C_frif(obj_t * ifexpr) ;
obj_t * C_frif_copy(obj_t * old) ;
void D_frif(obj_t ** frif_ptr) ;
obj_t * frif_copy_true(obj_t * frif) ;
obj_t * frif_copy_false(obj_t * frif) ;

/* frame for an app */
typedef struct _frapp {
	header_t head ;
	olist_t * vals,
		* exprs ;
} frapp_t ;

obj_t * C_frapp(obj_t * app) ;
obj_t * C_frapp_copy(obj_t * old) ;
void D_frapp(obj_t ** frapp_ptr) ;
obj_t * frapp_pop_expr(obj_t * frapp) ;
void frapp_push_value(obj_t * frapp, obj_t * obj) ;
bool frapp_has_more_exprs(obj_t * frapp) ;
olist_t * frapp_get_vals(obj_t * frapp) ;
obj_t * frapp_get_first_value(obj_t * frapp) ;


#endif /* !STACK_H */
