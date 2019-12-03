#include "olist.h"

#include <stdalign.h>
#include "obj.h"


enum dummy { DONT_CARE } ;

#define OLIST_HANDLE_MASK	0xffffff0

#define OLIST_HEAD(___handle) ((((olist_t *)((size_t)___handle & OLIST_HANDLE_MASK)))->head)



#define OLIST_LENGTH_SHIFT 	28
#define OLIST_LENGTH_BITS	16
#define OLIST_LENGTH_MASK	0xffff << OLIST_LENGTH_SHIFT


#define OLIST_LENGTH_GET(___list) (((size_t)___list) & OLIST_LENGTH_MASK) \
	>> OLIST_LENGTH_SHIFT

#define OLIST_LENGTH_SET(___list_ptr, ___newlength) do { \
	*___list_ptr = (olist_t *)(((___newlength) << OLIST_LENGTH_SHIFT) + \
	((size_t)*___list_ptr & OLIST_HANDLE_MASK)) ; } while (0) ;

void olist_length_inc(olist_t *** list) ;
inline void olist_length_inc(olist_t *** list) {
	OLIST_LENGTH_SET(*list, olist_length(**list) + 1) ;
}

void olist_length_dec(olist_t *** list) ;
inline void olist_length_dec(olist_t *** list) {
	OLIST_LENGTH_SET(*list, olist_length(**list) - 1) ;
}

/* Can fail */
olist_t * olist_init(void) {
	ALLOC_OR_RETNULL(new, olist_t) ;
	new->head = NULL ;
	return new ;
}

olist_t * olist_init_copy(olist_t * old) {
	olist_t * new = olist_init() ;
	obj_t * current ;
	for (size_t i = 0; i < olist_length(old); ++i) {
		current = olist_get(old, i) ;
		olist_append(&new, C_obj_copy(current)) ;
	}

	return new ;
}

/* not sure if there is a use case for this */
olist_t * olist_init_data(size_t count, ...) {
	ALLOC_OR_RETNULL(new, olist_t) ;
	new->head = NULL ;

	va_list arglist ;
	size_t i ;
	
	va_start(arglist, count) ;
	for (i = 0; i < count; ++i) {
		olist_append(&new, va_arg(arglist, obj_t *)) ;
	}
	va_end(arglist) ;

	return new ;
}

inline bool olist_empty(olist_t * list) {
	return !list || olist_length(list) == 0 ;
}

olist_node_t * make_olist_node(obj_t * obj) {
	ALLOC_OR_RETNULL(new, olist_node_t) ;
	*new = OLIST_NODE_INIT ;
	new->obj = obj ;
	return new ;
}

olist_node_t * free_olist_node(olist_node_t ** node_ptr, bool free_obj, bool free_next) {
	assert(node_ptr) ;
	assert(*node_ptr) ;

	olist_node_t * ret = NULL ;
	olist_node_t * node = *node_ptr ;
	if (free_obj) {
		D_OBJ(node->obj) ;
	}
	if (free_next) {
		free(node->next) ;
	}
	else {
		ret = node->next ;
	}
	free(node) ;
	*node_ptr = NULL ;
	return ret ;
}

/* Can fail */
bool olist_append(olist_t ** list, obj_t * new) {
	/* object better be valid here */
	assert(list), assert(new) ;

	olist_node_t * iter = OLIST_HEAD(*list) ;
	/* case: empty list */
	if (iter == NULL) {
		OLIST_HEAD(*list) = make_olist_node(new) ;
		if (!OLIST_HEAD(*list)) {
			/* we have failed in node allocation */
			return false ;
		}
		else {
			olist_length_inc(&list) ;
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
		olist_length_inc(&list) ;
		return true ;
	}
}


/* Can fail */
bool olist_insert(olist_t ** list, obj_t * new, size_t index) {
	assert(list), assert(new) ;
	/* bounds check */
	if (olist_length(*list) < index) {
		return false ;
	}	
	else {
		/* inserting to end? just use append then */
		/* this function will do a null ptr deref otherwise */
		if (olist_length(*list) == index) {
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
		/* case: push_front */
		if (index == 0) {
			next_tmp = OLIST_HEAD(*list) ;
			OLIST_HEAD(*list) = new_tmp ;
			OLIST_HEAD(*list)->next = next_tmp ;
			olist_length_inc(&list) ;
			return true ;
		}
		for (i = 0, iter = OLIST_HEAD(*list); i < index; iter = iter->next, ++i) ;
		next_tmp = iter->next ;
		iter->next = new_tmp ;
		iter->next->next = next_tmp ;
		olist_length_inc(&list) ;
		return true ;
			
	}

}

obj_t * olist_pop_index(olist_t ** list, size_t index)  {
	assert(list) ;

	/* bounds check */
	if (olist_length(*list) <= index) {
		/* everything off the end of the list is invalid */
		/* this should also handle the case of the empty list */
		return NULL ;
	}

	/* the get */
	obj_t * get ;
	olist_node_t * next_tmp ;

	/* case: pop first */
	if (index == 0) {
		get = OLIST_HEAD(*list)->obj ;
		next_tmp = OLIST_HEAD(*list)->next ;
		free_olist_node(&OLIST_HEAD(*list), false, false) ;
		OLIST_HEAD(*list) = next_tmp ;
		olist_length_dec(&list) ;
	}	
	/* case: pop rest */
	else {
		size_t i ;
		olist_node_t * iter = OLIST_HEAD(*list) ;
		/* go to one before the popped to do the popping */
		for (i = 0, iter = OLIST_HEAD(*list); i < index - 1; iter = iter->next, ++i) ;
		next_tmp = iter->next->next ;
		get = iter->next->obj ;
		free_olist_node(&iter->next, false, false) ;
		iter->next = next_tmp ;
		olist_length_dec(&list) ;
	}

	return get ;	
}

obj_t * olist_pop(olist_t ** list) {
	return olist_pop_index(list, olist_length(*list) - 1) ;
}

void olist_del(olist_t ** list, size_t index) {
	assert(list) ;

	/* bounds check */
	if (olist_length(*list) <= index) {
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
		next_tmp = free_olist_node(&OLIST_HEAD(*list), true, false) ;
		OLIST_HEAD(*list) = next_tmp ;
		olist_length_dec(&list) ;
	}	
	/* case: del rest */
	else {
		size_t i ;
		olist_node_t * iter = OLIST_HEAD(*list) ;
		/* go to one before the deleted to do the deleting */
		for (i = 0, iter = OLIST_HEAD(*list); i < index - 1; iter = iter->next, ++i) ;
		/* next_tmp = iter->next->next ; */
		next_tmp = free_olist_node(&iter->next, true, false) ;
		iter->next = next_tmp ;
		olist_length_dec(&list) ;
	}

}

void olist_del_all(olist_t ** list) {
	while(!olist_empty(*list)) {
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
		get = OLIST_HEAD(list)->obj ;
	}
	/* case: the delayed get */
	else {
		size_t i ;
		olist_node_t * iter ;
		for (i = 0, iter = OLIST_HEAD(list); i < index; iter = iter->next, ++i) ;
		get = iter->obj ;
	}
	return get ;
}

inline size_t olist_length(olist_t * list) {
	return OLIST_LENGTH_GET(list) ;
}


void olist_free(olist_t ** list_ptr) {
	assert(list_ptr) ;
	olist_t * list ;

	olist_del_all(list_ptr) ;

	list = (olist_t *)((size_t)*list_ptr & OLIST_HANDLE_MASK) ;
	assert(list) ;

	free(list) ;
	list_ptr = NULL ;
}
