#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdarg.h>

#include "olist.h"
#include "obj.h"

typedef enum {
	PRIM_INVALID,
	PRIM_PLUS,
	PRIM_MULT,
	PRIM_DIV,
	PRIM_SUB,
	PRIM_LTEQ,
	PRIM_LT,
	PRIM_EQ,
	PRIM_GT,
	PRIM_GTEQ,
	PRIM_PAIR,
	PRIM_FST,
	PRIM_SND,
	PRIM_BOX,
	PRIM_UNBOX,
	PRIM_SETBOX,
} prim_val_t ;


typedef struct _lam {
	header_t head ;
	obj_t * recname ;
	olist_t * binding ;
	obj_t * expr ;
	int refcnt ;
} lam_t ;

#define LAM_INIT(_recname, _binding, _expr) (lam_t) { \
	.head = HEADER_INIT(T_LAM, lam_t, lam_dec_ref, lam_inc_ref), \
	.recname = _recname, \
	.binding = _binding, \
	.expr = _expr }

obj_t * C_lam(obj_t * recname, olist_t * binding, obj_t * expr) ;
obj_t * C_lam_copy(obj_t * old) ;
void D_lam(obj_t ** lam_ptr) ;
olist_t * lam_get_binding(obj_t * lam) ;
obj_t * lam_get_expr(obj_t * lam) ;
obj_t * lam_get_recname(obj_t * lam) ;

obj_t * lam_inc_ref(obj_t * lam) ;
void lam_dec_ref(obj_t ** lam_ptr) ;

typedef struct _ident {
	header_t head ;
	char * value ;
	size_t length ;
	int refcnt ;
} ident_t ;

obj_t * C_ident(char * name) ;
obj_t * C_ident_copy(obj_t * old) ;
void D_ident(obj_t ** ident_ptr) ;
char * ident_get_name(obj_t * ident) ;
int ident_cmp(obj_t * first, obj_t * second) ;

obj_t * ident_inc_ref(obj_t * ident) ;
void ident_dec_ref(obj_t ** ident_ptr) ;

#define PTR_INIT(_addr, _size) (ptr_t) { \
	.head = HEADER_INIT(T_PTR, ptr_t, D_ptr, C_ptr_copy), \
	.addr = _addr, \
	.size = _size }

/* ptrs will be handles by the mem subsystem */
typedef struct _ptr {
	header_t head ;
	void * addr ;
	size_t size ;
} ptr_t ;

obj_t * C_ptr(void * addr, size_t size) ;
obj_t * C_ptr_copy(obj_t * old) ;
void D_ptr(obj_t ** ptr_ptr) ;
void * ptr_addr(obj_t * ptr) ;
size_t ptr_size(obj_t * ptr) ;


#define ABORT_INIT(_expr) (abort_t) { \
	.head = HEADER_INIT(T_ABORT, abort_t, D_abort, C_abort_copy), \
	.expr = _expr }

typedef struct _abort {
	header_t head ;
	obj_t * expr ;
} abort_t ;

obj_t * C_abort(obj_t * expr) ;
obj_t * C_abort_copy(obj_t * old) ;
void D_abort(obj_t ** abort_ptr) ;
obj_t * abort_expr(obj_t * abort) ;

typedef struct _app {
	header_t head ;
	olist_t * expr_list ;
} app_t ;

obj_t * C_app(size_t count, ...) ;
obj_t * C_app_copy(obj_t * old) ;
obj_t * C_app_list(olist_t * expr_list) ;
void D_app(obj_t ** app_ptr) ;
olist_t * app_get_expr_list(obj_t * app) ;

void app_print(obj_t * app) ;

typedef struct _if {
	header_t head ;
	obj_t * expr_pred,
	       * expr_true,
	       * expr_false ;
} if_t  ;

#define IF_INIT(e_pred, e_true, e_false) (if_t) { \
	.head = HEADER_INIT(T_IF, if_t, D_if, C_if_copy), \
	.expr_pred = (obj_t *)e_pred, \
	.expr_true = (obj_t *)e_true, \
	.expr_false = (obj_t *)e_false }

obj_t * C_if(obj_t * e_pred, obj_t * e_true, obj_t * e_false) ;
obj_t * C_if_copy(obj_t * old) ;
void D_if(obj_t ** if_ptr) ;
obj_t * if_copy_pred(obj_t * ifexpr) ;

obj_t * if_get_pred(obj_t * ifexpr) ;
obj_t * if_get_true(obj_t * ifexpr) ;
obj_t * if_get_false(obj_t * ifexpr) ;

typedef struct _pair {
	header_t head ;
	obj_t * first ;
	obj_t * second ;
} pair_t ;

#define PAIR_INIT(_fst, _snd) (pair_t) { \
	.head = HEADER_INIT(T_PAIR, pair_t, D_pair, C_pair_copy), \
	.first = _fst, \
	.second = _snd, }

obj_t * C_pair(obj_t * first, obj_t * second) ;
obj_t * C_pair_copy(obj_t * old) ;
void D_pair(obj_t ** pair_ptr) ;
obj_t * pair_first(obj_t * pair) ;
obj_t * pair_second(obj_t * pair) ;

void pair_overwrite_first(obj_t * pair, obj_t * new) ;
void pair_overwrite_second(obj_t * pair, obj_t * new) ;

typedef struct _val {
	header_t head ;
} val_t ;

typedef struct _prim {
	header_t head  ;
	prim_val_t value ;
} prim_t ;

#define PRIM_INIT(_primitive) (prim_t) { \
	.head = HEADER_INIT(T_PRIM, prim_t, D_prim, C_prim_copy), \
	.value = prim_stov(_primitive)} \
	
char * prim_vtos(prim_val_t prim_val) ;
prim_val_t prim_stov(char * prim) ;

obj_t * C_prim(char * prim) ;
obj_t * C_prim_copy(obj_t * old) ;
void D_prim(obj_t ** prim_ptr) ;

prim_val_t prim_get_val(obj_t * prim) ;

typedef struct _bool {
	header_t head ;
	bool value ;
} bool_t ;

#define BOOL_INIT(_boolean) (bool_t) { \
	.head = HEADER_INIT(T_BOOL, bool_t, D_bool, C_bool_copy), \
	.value = (!!_boolean) } \

obj_t * C_bool(bool value) ;
obj_t * C_bool_copy(obj_t * old) ;
void D_bool(obj_t ** bool_ptr) ;

typedef struct _num {
	header_t head ;
	double value ;
} num_t ;

#define NUM_INIT(_number) (num_t) { \
	.head = HEADER_INIT(T_NUM, num_t, D_num, C_num_copy), \
	.value = _number } \

obj_t * C_num(double value) ;
obj_t * C_num_copy(obj_t * old) ;
void D_num(obj_t ** num_ptr) ;

/* There is actually just one constant unit object that is statically allocated */
obj_t * C_unit() ;
obj_t * C_unit_copy(obj_t * old) ;
void D_unit(obj_t ** unit_ptr) ;


void value_print(obj_t * value) ;
void expr_print(obj_t * value) ;

#endif /* !TYPES_H */
