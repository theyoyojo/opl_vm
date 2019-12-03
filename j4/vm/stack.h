#ifndef STACK_H
#define STACK_H

#include "obj.h"
#include "olist.h"
#include "types.h"

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
void stack_trace(obj_t * stack) ;
size_t stack_height(obj_t * stack) ;
obj_t * stack_top_env(obj_t * stack) ;

/* frame for an if */
typedef struct _frif {
	header_t head ;
	obj_t * e_true,
	      * e_false ;
	obj_t * env ;
} frif_t ;


obj_t * C_frif(obj_t * ifexpr, obj_t * env) ;
obj_t * C_frif_copy(obj_t * old) ;
void D_frif(obj_t ** frif_ptr) ;
obj_t * frif_copy_true(obj_t * frif) ;
obj_t * frif_copy_false(obj_t * frif) ;

/* frame for an app */
typedef struct _frapp {
	header_t head ;
	olist_t * vals,
		* exprs ;
	obj_t * env ;
} frapp_t ;

obj_t * C_frapp(obj_t * app, obj_t * env) ;
obj_t * C_frapp_copy(obj_t * old) ;
void D_frapp(obj_t ** frapp_ptr) ;
obj_t * frapp_pop_expr(obj_t * frapp) ;
void frapp_push_value(obj_t * frapp, obj_t * obj) ;
bool frapp_has_more_exprs(obj_t * frapp) ;
olist_t * frapp_get_vals(obj_t * frapp) ;
obj_t * frapp_get_first_value(obj_t * frapp) ;

/* frame for a ret, needed for env handling */
typedef struct _frret {
	header_t head ;
	obj_t * env ;
} frret_t ;

obj_t * C_frret(obj_t * env) ;
obj_t * C_frret_copy(obj_t * old) ;
void D_frret(obj_t ** frret_ptr) ;

/* an enviromemnt for lazy variable substitution */
typedef struct _env {
	header_t head ;
	olist_t * idents ;
	olist_t * vals ;
	int refcnt ;
} env_t ;

#define ENV_INIT() (env_t) { \
	.head = HEADER_INIT(T_ENV, env_dec_ref, C_env_copy), \
	.idents = olist_init(), \
	.vals = olist_init(), \
	.refcnt = 1 } \

obj_t * C_env(void) ;
obj_t * C_env_copy(obj_t * old) ;
void D_env(obj_t ** env_ptr) ;
/* does NOT consume the lists passed to it */
int env_bind(obj_t * env, olist_t * binding, olist_t * vals) ;
int env_bind_direct(obj_t * env, obj_t * ident, obj_t * value) ;
/* check if an environment maps a variable to a value */
bool env_maps(obj_t * env, obj_t * ident) ;
/* Do the substitution, consume the identifier, return a copy of the mapped value */
obj_t * env_subst(obj_t * env, obj_t * ident) ;

bool env_empty(obj_t * env) ;
char * env_get_name(obj_t * env) ;

ident_t * env_get_ident(obj_t * env, size_t index) ;
obj_t * env_get_val(obj_t * env, size_t index) ;

size_t env_length(obj_t * env) ;
size_t env_girth(obj_t * env) ;

/* envs need to be reference counted because they have multiple valid pointers! ahhh! */
/* inc_ref MUST be used for every non-constructor assignment to an env! */
obj_t * env_inc_ref(obj_t * env) ;
void env_dec_ref(obj_t ** env_ptr) ;
int env_get_ref(obj_t * env) ;

void env_print(obj_t * env) ;

typedef struct _clo {
	header_t head ;
	obj_t * lam ;
	obj_t * env ;
	obj_t * env_orig ;
	int refcnt ;
} clo_t ;

#define CLO_INIT(_lam, _env) (clo_t) { \
	.head = HEADER_INIT(T_CLO, clo_dec_ref, clo_inc_ref), \
	.lam =  _lam, \
	.env = _env, \
	.env_orig = _env, \
	.refcnt = 1 }

obj_t * C_clo(obj_t * lam, obj_t * env) ;
void D_clo(obj_t ** clo_ptr) ;

obj_t * clo_inc_ref(obj_t * clo) ;
void clo_dec_ref(obj_t ** clo_ptr) ;
int clo_get_ref(obj_t * clo) ;

obj_t * clo_get_lam(obj_t * clo) ;
obj_t * clo_get_env(obj_t * clo) ;
obj_t * clo_get_env_noref(obj_t * clo) ;

/* frame accessor functions for examining that stack */
char * frame_get_name(obj_t * frame) ;
char * frame_get_env_name(obj_t * frame) ;
void frame_print(obj_t * frame) ;

#endif /* !STACK_H */
