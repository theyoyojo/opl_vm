#ifndef MEM_H
#define MEM_H

#include "types.h"

/* define NOGC to disable garbage collection */
#ifdef DEBUG
/* define MEM_DEBUG to enable verbose output from memory system */
#define MEM_DEBUG
#else
#undef MEM_DEBUG
#endif

#ifdef MEM_DEBUG
#define MEM_printf(...) printf( __VA_ARGS__)
#else
#define MEM_printf(...) (void)0
#endif


int mem_sys_up(void) ;

obj_t * mem_alloc_init(obj_t * obj) ;

obj_t * mem_deref(obj_t * ptr) ;

obj_t * mem_set(obj_t * ptr, obj_t * newval) ;

int mem_gc(obj_t * code, obj_t * env, obj_t * stack) ;

int mem_sys_down(void) ;

#endif /* MEM_H */
