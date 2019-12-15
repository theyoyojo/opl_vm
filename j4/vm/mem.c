#include "mem.h"
#include "types.h"
#include <string.h>

static int init_success = -1 ;

static obj_t ** memtab ;
static size_t memtab_size ;
static size_t memtab_capacity ;

#define MEMTAB_CAPACITY_INIT 	64

#define MEMTAB_ADDR_MASK	0xfffffffffff0UL
#define MEMTAB_VALID_BITS	1UL
#define MEMTAB_VALID_SHIFT 	48
#define MEMTAB_VALID_MASK	(MEMTAB_VALID_BITS << MEMTAB_VALID_SHIFT)
#define MEMTAB_DEREF(_index) 	(obj_t *)((unsigned long )memtab[_index] & MEMTAB_ADDR_MASK)
#define MEMTAB_VALID(_index)	(!!(((unsigned long)memtab[_index] & MEMTAB_VALID_MASK) >> MEMTAB_VALID_SHIFT))
#define MEMTAB_SETV(_ix, _nv)	do { memtab[_ix] = (obj_t *)(((unsigned long)memtab[_ix] & ~MEMTAB_VALID_BITS) + ((_nv & MEMTAB_VALID_BITS) << MEMTAB_VALID_SHIFT)) ; } while (0)

#define MEM_SYS_UP()		do { init_success = 0xbeef ; } while (0) ;
#define MEM_SYS_DOWN()		do { init_success = -1	   ; } while (0) ;
#define MEM_SYS_OOPS()		do { init_success = 0xdead ; } while (0) ;
#define MEM_SYS_VALIDATE()	(!!(init_success == 0xbeef))

int mem_system_init(void) {
	
	memtab_capacity = MEMTAB_CAPACITY_INIT ;
	if (!(memtab = (obj_t **)malloc(sizeof(obj_t *) * memtab_capacity))) {
		return -1 ; }

	memtab_size  = 0 ;
	memset(memtab, 0, memtab_capacity) ;

	MEM_SYS_UP() ;
	return 0 ;
	
}

int memtab_valid(size_t index) {
	return MEMTAB_VALID(index) ;
}

obj_t * mem_alloc_init(obj_t * obj) {
	obj_t * tmp ;
	obj_t ** tmp_ptr ;

	if (!MEM_SYS_VALIDATE()) {
		MEM_SYS_OOPS() ;
		return C_abort(C_str("Exception: memory system in invalid state")) ; }

	/* if we get close to the limit, square it */
	if (memtab_size >= memtab_capacity - 10) {
		memtab_capacity *= memtab_capacity ;
		tmp_ptr = (obj_t **)realloc(memtab, memtab_capacity) ;
		if (!tmp_ptr) {
			MEM_SYS_OOPS() ;
			memtab_capacity /= memtab_capacity ;
			return C_abort(C_str("Exception: memory system in invalid state")) ; }
		memtab = tmp_ptr ; }

	tmp = C_obj_copy(obj) ;
	if (!tmp) {
		MEM_SYS_OOPS() ;
		return C_abort(C_str("Exception: unable to allocate memory")) ; }
	else {
		memtab[memtab_size++] = tmp ;
		MEMTAB_SETV(memtab_size - 1, 1) ;
		return C_ptr((void *)(memtab_size - 1), obj_sizeof(MEMTAB_DEREF(memtab_size - 1))) ; }
}


obj_t * mem_deref(obj_t * ptr) {
	size_t index ;

	if (!MEM_SYS_VALIDATE()) {
		MEM_SYS_OOPS() ;
		return C_abort(C_str("Exception: memory system in invalid state")) ; }

	if (obj_typeof(ptr) != T_PTR) {
		return C_abort(C_str("Exception: illegal memory access attempt with non-pointer value")) ; }
		/* invalid ptr deref, launch the missles at russia */ 

	index = (size_t)ptr_addr(ptr) ;

	if (index >= memtab_size || !MEMTAB_VALID(index)) {
		return C_abort(C_app(3, C_prim("+"), C_str("Exception: illegal memory access at address value: "), C_stringify(ptr))) ; }
	else {
		return C_obj_copy(MEMTAB_DEREF(index)) ; }
}

obj_t * mem_set(obj_t * ptr, obj_t * newval) {
	size_t index ;
	obj_t * tmp, * tmp2 ;

	if (!MEM_SYS_VALIDATE()) {
		MEM_SYS_OOPS() ;
		return C_abort(C_str("Exception: memory system in invalid state")) ; }

	if (obj_typeof(ptr) != T_PTR) {
		return C_abort(C_str("Exception: illegal memory access attempt with non-pointer value")) ; }
		/* invalid ptr deref, launch the missles at russia */

	index = (size_t)ptr_addr(ptr) ;

	if (index >= memtab_size || !MEMTAB_VALID(index)) {
		return C_abort(C_app(3, C_prim("+"), C_str("Exception: illegal memory access at address value: "), C_stringify(ptr))) ; }
	else {
		if (!(tmp = C_obj_copy(newval))) {
			MEM_SYS_OOPS() ;
			return C_abort(C_str("Exception: unable to allocate memory")) ; }
		tmp2 = MEMTAB_DEREF(index) ;
		D_OBJ(tmp2) ;
		memtab[index] = tmp ;
		MEMTAB_SETV(index, 1) ;
		return C_obj_copy(ptr) ; }
}

int mem_gc(obj_t * code, obj_t * env, obj_t * stack) {
	/* the trash heap strategy: we just pile it on until we have to get rid of it */
	return 0 ;
}

int mem_system_free(void) {
	obj_t * tmp ;
	if (!MEM_SYS_VALIDATE()) {
		MEM_SYS_OOPS() ;
		return -1 ; }

	for (size_t i = 0; i < memtab_size; ++i) {
		if (MEMTAB_VALID(i)) {
			tmp = MEMTAB_DEREF(i) ;
			D_OBJ(tmp) ; } }

	free(memtab) ;

	MEM_SYS_DOWN() ;
	return 0 ;
}
