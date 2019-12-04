#ifndef OLIST_H
#define OLIST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>

typedef struct _obj obj_t ;

typedef struct _olist_node {
	obj_t * obj ;
	struct _olist_node * next ;
} olist_node_t ;

#define OLIST_NODE_INIT (olist_node_t) { \
			.obj = NULL, \
			.next = NULL }

typedef struct _olist {
	olist_node_t * head ;
	olist_node_t * tail ;
} olist_t ;

/* Can fail */
olist_t * olist_init(void) ;

olist_t * olist_init_data(size_t count, ...) ;

olist_t * olist_init_copy(olist_t * old) ;

bool olist_empty(olist_t * list) ;

/* Can fail */
bool olist_append(olist_t ** list, obj_t * new) ;

/* Can fail */
bool olist_insert(olist_t ** list, obj_t * new, size_t index) ;


obj_t * olist_pop_index(olist_t ** list, size_t index) ;

/* pop end */
obj_t * olist_pop(olist_t ** list) ;

void olist_del(olist_t ** list, size_t index) ; 

void olist_del_all(olist_t ** list) ;

obj_t * olist_get(olist_t * list, size_t index) ;

size_t olist_length(olist_t * list) ;

void olist_free(olist_t ** list_ptr) ;

#endif /* OLIST_H */
