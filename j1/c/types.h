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


typedef struct _expr {
	header_t head ;
} expr_t ;

typedef struct _app {
	header_t head ;
	olist_t * expr_list ;
} app_t ;

obj_t * C_app(size_t count, ...) ;
void D_app(obj_t ** app) ;

typedef struct _if {
	header_t head ;
	obj_t * expr_pred,
	       * expr_true,
	       * expr_false ;
} if_t  ;

#define IF_INIT(e_pred, e_true, e_false) (if_t) { \
	.head = HEADER_INIT(T_IF, D_if), \
	.expr_pred = (obj_t *)e_pred, \
	.expr_true = (obj_t *)e_true, \
	.expr_false = (obj_t *)e_false }

obj_t * C_if(obj_t * e_pred, obj_t * e_true, obj_t * e_false) ;
void D_if(obj_t ** if_ptr) ;

typedef struct _val {
	header_t head ;
} val_t ;

typedef struct _prim {
	header_t head  ;
	prim_val_t value ;
} prim_t ;

#define PRIM_INIT(_primitive) (prim_t) { \
	.head = HEADER_INIT(T_PRIM, D_prim), \
	.value = prim_stov(_primitive)} \
	
char * prim_vtos(prim_val_t prim_val) ;
prim_val_t prim_stov(char * prim) ;

obj_t * C_prim(char * prim) ;
void D_prim(obj_t ** prim_ptr) ;

typedef struct _bool {
	header_t head ;
	bool value ;
} bool_t ;

#define BOOL_INIT(_boolean) (bool_t) { \
	.head = HEADER_INIT(T_BOOL, D_bool), \
	.value = _boolean } \

obj_t * C_bool(bool value) ;
void D_bool(obj_t ** bool_ptr) ;

typedef struct _num {
	header_t head ;
	double value ;
} num_t ;

#define NUM_INIT(_number) (num_t) { \
	.head = HEADER_INIT(T_NUM, D_num), \
	.value = _number } \

obj_t * C_num(double value) ;
void D_num(obj_t ** num_ptr) ;


type_t obj_typeof(obj_t * obj) ;

#endif /* !TYPES_H */
