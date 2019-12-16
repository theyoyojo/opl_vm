#ifndef MEM_H
#define MEM_H

#include "types.h"

int mem_sys_up(void) ;

obj_t * mem_alloc_init(obj_t * obj) ;

obj_t * mem_deref(obj_t * ptr) ;

obj_t * mem_set(obj_t * ptr, obj_t * newval) ;

int mem_gc(obj_t * code, obj_t * env, obj_t * stack) ;

int mem_sys_down(void) ;

#endif /* MEM_H */
