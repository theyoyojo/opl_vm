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
	PRIM_PLUS = 0,
	PRIM_MULT,
	PRIM_DIV,
	PRIM_SUB,
	PRIM_LTEQ,
	PRIM_LT,
	PRIM_EQ,
	PRIM_GT,
	PRIM_GTEQ,
	PRIM_INVALID = 99
} prim_val_t ;


typedef struct _lam {
	header_t head ;
	obj_t * recname ;
	olist_t * binding ;
	obj_t * expr ;
} lam_t ;

#define LAM_INIT(_recname, _binding, _expr) (lam_t) { \
	.head = HEADER_INIT(T_LAM, D_lam, C_lam_copy), \
	.recname = _recname, \
	.binding = _binding, \
	.expr = _expr }

obj_t * C_lam(obj_t * recname, olist_t * binding, obj_t * expr) ;
obj_t * C_lam_copy(obj_t * old) ;
void D_lam(obj_t ** lam_ptr) ;
olist_t * lam_get_binding(obj_t * lam) ;
obj_t * lam_get_expr(obj_t * lam) ;
obj_t * lam_get_recname(obj_t * lam) ;

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
	.head = HEADER_INIT(T_IF, D_if, C_if_copy), \
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

typedef struct _val {
	header_t head ;
} val_t ;

typedef struct _prim {
	header_t head  ;
	prim_val_t value ;
} prim_t ;

#define PRIM_INIT(_primitive) (prim_t) { \
	.head = HEADER_INIT(T_PRIM, D_prim, C_prim_copy), \
	.value = prim_stov(_primitive)} \
	
char * prim_vtos(prim_val_t prim_val) ;
prim_val_t prim_stov(char * prim) ;

obj_t * C_prim(char * prim) ;
obj_t * C_prim_copy(obj_t * old) ;
void D_prim(obj_t ** prim_ptr) ;

typedef struct _bool {
	header_t head ;
	bool value ;
} bool_t ;

#define BOOL_INIT(_boolean) (bool_t) { \
	.head = HEADER_INIT(T_BOOL, D_bool, C_bool_copy), \
	.value = (!!_boolean) } \

obj_t * C_bool(bool value) ;
obj_t * C_bool_copy(obj_t * old) ;
void D_bool(obj_t ** bool_ptr) ;

typedef struct _num {
	header_t head ;
	double value ;
} num_t ;

#define NUM_INIT(_number) (num_t) { \
	.head = HEADER_INIT(T_NUM, D_num, C_num_copy), \
	.value = _number } \

obj_t * C_num(double value) ;
obj_t * C_num_copy(obj_t * old) ;
void D_num(obj_t ** num_ptr) ;


void value_print(obj_t * value) ;
void expr_print(obj_t * value) ;

#endif /* !TYPES_H */
