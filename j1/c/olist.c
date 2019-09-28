#include "olist.h"

#include <stdalign.h>
#include "obj.h"

#define ALLOC_OR_RETNULL(id, type) type * id = (type *)malloc(sizeof(type)) ; \
					if(!id) return NULL

enum dummy { DONT_CARE } ;

/* Can fail */
olist_t * olist_init(void) {
	ALLOC_OR_RETNULL(new, olist_t) ;
	*new = OLIST_INIT ;
	return new ;
}

bool olist_empty(olist_t * list) {
	/* no nonsense with null ptrs */
	assert(list) ;
	return list->head == NULL ;
}

olist_node_t * make_olist_node(obj_t * obj) {
	ALLOC_OR_RETNULL(new, olist_node_t) ;
	*new = OLIST_NODE_INIT ;
	new->obj = obj ;
	return new ;
}

void free_olist_node(olist_node_t ** node_ptr, bool free_obj, bool free_next) {
	assert(node_ptr) ;
	assert(*node_ptr) ;
	olist_node_t * node = *node_ptr ;
	if (free_obj) {
		/* This is a hack to get around not knowing the layout of obj_t in olist context */
		/* i.e. in memory at obj address:
		 * | type_t  | 	     | <D_func>
		 * 0        4        8 <- aligned to word boundary
		 */
		/* (*(void (**)(obj_t **))(((void *)node->obj)+8))(&node->obj) ; */
		node->obj->head.D_func(&node->obj) ;
	}
	if (free_next) {
		free(node->next) ;
	}
	free(node) ;
	*node_ptr = NULL ;
}

/* Can fail */
bool olist_append(olist_t * list, obj_t * new) {
	/* object better be valid here */
	assert(list), assert(new) ;

	olist_node_t * iter = list->head ;
	/* case: empty list */
	if (iter == NULL) {
		list->head = make_olist_node(new) ;
		if (!list->head) {
			/* we have failed in node allocation */
			return false ;
		}
		else {
			list->length_cache++ ;
			return true ;
		}
	}
	/* case: non-empty list */
	while (iter->next != NULL) {
		iter = iter->next ;
	}
	/* iter->next is now the end of the list */
	iter->next = make_olist_node(new) ;
	if (!iter->next) {
		/* we have failed in node allocation */
		return false ;
	}
	else {
		list->length_cache++ ;
		return true ;
	}
}


/* Can fail */
bool olist_insert(olist_t * list, obj_t * new, size_t index) {
	assert(list), assert(new) ;
	/* bounds check */
	if (olist_length(list) < index) {
		return false ;
	}	
	else {
		/* inserting to end? just use append then */
		/* this function will do a null ptr deref otherwise */
		if (olist_length(list) == index) {
			return olist_append(list, new) ;
		}
		/* assumptions from here on out: we are inserting into
		 * the middle of a non empty list */
		size_t i ;
		olist_node_t * iter,
			     * new_tmp,
			     * next_tmp ;

		new_tmp = make_olist_node(new) ;
		if (!new_tmp) {
			/* alloc fail, the whole thing is cancelled */
			return false ;
		}
		for (i = 0, iter = list->head; i < index; iter = iter->next, ++i) ;
		next_tmp = iter->next ;
		iter->next = new_tmp ;
		iter->next->next = next_tmp ;
		list->length_cache++ ;
		return true ;
			
	}

}

obj_t * olist_pop_index(olist_t * list, size_t index)  {
	assert(list) ;

	/* bounds check */
	if (olist_length(list) <= index) {
		/* everything off the end of the list is invalid */
		/* this should also handle the case of the empty list */
		return NULL ;
	}

	/* the get */
	obj_t * get ;
	olist_node_t * next_tmp ;

	/* case: pop first */
	if (index == 0) {
		get = list->head->obj ;
		next_tmp = list->head->next ;
		free_olist_node(&list->head, false, false) ;
		list->head = next_tmp ;
		list->length_cache-- ;
	}	
	/* case: pop rest */
	else {
		size_t i ;
		olist_node_t * iter = list->head ;
		/* go to one before the popped to do the popping */
		for (i = 0, iter = list->head; i < index - 1; iter = iter->next, ++i) ;
		next_tmp = iter->next->next ;
		get = iter->next->obj ;
		free_olist_node(&iter->next, false, false) ;
		iter->next = next_tmp ;
		list->length_cache-- ;
	}

	return get ;	
}

obj_t * olist_pop(olist_t * list) {
	return olist_pop_index(list, olist_length(list) - 1) ;
}

void olist_del(olist_t * list, size_t index) {
	assert(list) ;

	/* bounds check */
	if (olist_length(list) <= index) {
		/* everything off the end of the list is invalid */
		/* this should also handle the case of the empty list */
		/* TODO: should this function indicate failure?
		 * or is the deletion of a node past the end of the list
		 * already a sucess because it never existed in the
		 * first place?
		 */
		return ;
	}

	olist_node_t * next_tmp ;

	/* case: del first */
	if (index == 0) {
		next_tmp = list->head->next ;
		free_olist_node(&list->head, true, false) ;
		list->head = next_tmp ;
		list->length_cache-- ;
	}	
	/* case: del rest */
	else {
		size_t i ;
		olist_node_t * iter = list->head ;
		/* go to one before the deleted to do the deleting */
		for (i = 0, iter = list->head; i < index - 1; iter = iter->next, ++i) ;
		next_tmp = iter->next->next ;
		free_olist_node(&iter->next, true, false) ;
		iter->next = next_tmp ;
		list->length_cache-- ;
	}

}

void olist_del_all(olist_t * list) {
	while(!olist_empty(list)) {
		olist_del(list, 0) ;
	}
}

obj_t * olist_get(olist_t * list, size_t index) {
	if (olist_length(list) <= index) {
		return NULL ;
	}
	obj_t * get ;
	/* case: the degenerate get */
	if (index == 0) {
		get = list->head->obj ;
	}
	/* case: the delayed get */
	else {
		size_t i ;
		olist_node_t * iter ;
		for (i = 0, iter = list->head; i < index; iter = iter->next, ++i) ;
		get = iter->obj ;
	}
	return get ;
}

size_t olist_length(olist_t * list) {
	if (list->cache_valid) {
		return list->length_cache ;
	}
	list->length_cache = 0 ;
	olist_node_t * iter  = list->head ;
	while(iter != NULL) {
		list->length_cache++ ;
		iter = iter->next ;	
	}
	list->cache_valid = true ;
	return list->length_cache ;
}


void olist_free(olist_t ** list_ptr) {
	assert(list_ptr) ;
	assert(*list_ptr) ;
	olist_del_all(*list_ptr) ;
	free(*list_ptr) ;
	list_ptr = NULL ;
}
