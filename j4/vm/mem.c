#include "mem.h"
#include "types.h"
#include <string.h>

struct mem {
	void * mem ;
	size_t size ;
	struct mem * next ;
} ;


static struct mem * head ;

int mem_system_init(void) {
	head = NULL ;
	return 0 ;
	
}

obj_t * mem_alloc_init(obj_t * obj) {
	struct mem * new = (struct mem *)malloc(sizeof(struct mem)) ;

	new->size = obj_sizeof(obj) ;
	new->mem = malloc(new->size) ;
	memcpy(new->mem, obj, new->size) ;
	new->next = head ;
	head = new ;

	return C_ptr(new->mem, new->size) ;
}



obj_t * mem_deref(obj_t * ptr) {
	if (obj_typeof(ptr) != T_PTR) {
		return C_abort(C_str("Exception: illegal memory access")) ;
		/* invalid ptr deref, launch the missles at russia */
	}
	else {
		obj_t * new = malloc(ptr_size(ptr)) ;
		memcpy(new, ptr_addr(ptr), ptr_size(ptr)) ; /* this assumes it is never freed */
		return new ;
	}
}

obj_t * mem_set(obj_t * ptr, obj_t * newval) {
	if (obj_typeof(ptr) != T_PTR) {
		return C_abort(C_str("Exception: illegal memory access")) ;
		/* invalid ptr deref, launch the missles at russia */
	}
	else {
		struct mem * iter = head ;
		while (iter && iter->mem != ptr_addr(ptr)) {
			iter = iter->next ;
		}
		if (!iter) {
			return C_abort(C_num(666)) ;
		}
		else {
			if (iter->size > obj_sizeof(newval)) {
				iter->mem = realloc(iter->mem, obj_sizeof(newval)) ;
			}
			memcpy(iter->mem, newval, obj_sizeof(newval)) ;
			return mem_deref(ptr) ;
		}
	}

}

int mem_gc(obj_t * code, obj_t * env, obj_t * stack) {
	/* the trash heap strategy: we just pile it on until we have to get rid of it */
	return 0 ;
}

int mem_system_free(void) {

	struct mem * tmp ;
	while (head) {
		free(head->mem) ;
		tmp = head->next ;
		free(head) ;
		head = tmp ;
	}

	return 0 ;
}
