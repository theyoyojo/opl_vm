#include "mem.h"
#include "types.h"
#include "stack.h"
#include <string.h>

static int init_success = -1 ;

static obj_t ** memtab ;
static size_t memtab_size ;
static size_t memtab_capacity ;

#define MEMTAB_CAPACITY_INIT 	64

#define MEMTAB_ADDR_MASK	0xfffffffffff0UL
#define MEMTAB_VALID_BITS	1UL
#define MEMTAB_REACH_BITS	1UL
#define MEMTAB_VALID_SHIFT 	48
#define MEMTAB_REACH_SHIFT	49
#define MEMTAB_VALID_MASK	(MEMTAB_VALID_BITS << MEMTAB_VALID_SHIFT)
#define MEMTAB_REACH_MASK	(MEMTAB_REACH_BITS << MEMTAB_REACH_SHIFT)
#define MEMTAB_DEREF(_index) 	(obj_t *)((unsigned long )memtab[_index] & MEMTAB_ADDR_MASK)
#define MEMTAB_VALID(_index)	(!!(((unsigned long)memtab[_index] & MEMTAB_VALID_MASK) >> MEMTAB_VALID_SHIFT))
#define MEMTAB_REACH(_index)	(!!(((unsigned long)memtab[_index] & MEMTAB_REACH_MASK) >> MEMTAB_REACH_SHIFT))
#define MEMTAB_SETV(_ix, _nv)	do { memtab[_ix] = (obj_t *)(((unsigned long)memtab[_ix] & ~MEMTAB_VALID_MASK) + ((_nv & MEMTAB_VALID_BITS) << MEMTAB_VALID_SHIFT)) ; } while (0)
#define MEMTAB_SETR(_ix, _nv)	do { memtab[_ix] = (obj_t *)(((unsigned long)memtab[_ix] & ~MEMTAB_REACH_MASK) + ((_nv & MEMTAB_REACH_BITS) << MEMTAB_REACH_SHIFT)) ; } while (0)

#define MEM_SYS_UP()		do { init_success = 0xbeef ; } while (0) ;
#define MEM_SYS_DOWN()		do { init_success = -1	   ; } while (0) ;
#define MEM_SYS_OOPS()		do { init_success = 0xdead ; } while (0) ;
#define MEM_SYS_VALIDATE()	(!!(init_success == 0xbeef))

#define RQ_CAPACITY_INIT	64

static obj_t ** rq ;
static size_t rq_last ;
static size_t rq_first ;
static size_t rq_size ;
static size_t rq_capacity ;


int rq_push(obj_t * obj) {
	obj_t ** tmp_ptr ;
	if (rq_size >= rq_capacity - 10) {
		rq_capacity *= 2 ;
		if (!(tmp_ptr = (obj_t **)realloc(rq, rq_capacity))) {
			rq_capacity /= 2 ;
			return -1 ; }
		rq = tmp_ptr ;
	}

	/* printf("writing to rq[last=%zu, size=%zu]\n", rq_last, rq_size) ; */
	rq[rq_last] = obj ;

	rq_last = (rq_last + 1) % rq_capacity ;
	++rq_size ;
	return 0 ;
}

obj_t * rq_pop(void) {
	obj_t * tmp ;
	if (rq_size <= 0) {
		return NULL ; }

	/* printf("reading from rq[first=%zu, size=%zu]\n", rq_first, rq_size) ; */
	tmp = rq[rq_first] ;
	rq_first = (rq_first + 1) % rq_capacity ;
	--rq_size ;
	return tmp ;
}

int mem_sys_up(void) {
	
	memtab_capacity = MEMTAB_CAPACITY_INIT ;
	if (!(memtab = (obj_t **)malloc(sizeof(obj_t *) * memtab_capacity))) {
		MEM_SYS_OOPS() ;
		return -1 ; }

	memtab_size  = 0 ;
	memset(memtab, 0, memtab_capacity) ;

	rq_capacity = RQ_CAPACITY_INIT ;
	if (!(rq = (obj_t **)malloc(sizeof(obj_t *) * rq_capacity))) {
		free(memtab) ;
		MEM_SYS_OOPS() ;
		return -1 ; }

	rq_size = rq_last = rq_first = 0 ;

	MEM_SYS_UP() ;
	return 0 ;
	
}

void mem_ptr_set_reachable(obj_t * ptr, int reachable) {
	size_t index ;

	index = (size_t)ptr_addr(ptr) ;
	
	if (index >= memtab_size) {
		return ; }

	MEMTAB_SETR(index, reachable) ;
}

void mem_ptr_mark(obj_t * ptr) {
	mem_ptr_set_reachable(ptr, 1) ;
}

void mem_ptr_unmark(obj_t * ptr) {
	mem_ptr_set_reachable(ptr, 0) ;
}

void mem_index_unmark(size_t index) {
	if (index >= memtab_size) {
		return ; }
	MEMTAB_SETR(index, 0) ;
}

int memtab_valid(size_t index) {
	return MEMTAB_VALID(index) ;
}

int memtab_reachable(size_t index) {
	return MEMTAB_REACH(index) ;
}

obj_t * mem_alloc_init(obj_t * obj) {
	obj_t * tmp ;
	obj_t ** tmp_ptr ;

	if (!MEM_SYS_VALIDATE()) {
		MEM_SYS_OOPS() ;
		return C_abort(C_str("Exception: memory system in invalid state")) ; }

	/* if we get close to the limit, square it */
	if (memtab_size >= memtab_capacity - 10) {
		/* printf("RESIZE: %zu --> %zu\n", memtab_capacity, memtab_capacity * memtab_capacity) ; */
		memtab_capacity *= memtab_capacity ;
		tmp_ptr = (obj_t **)realloc(memtab, sizeof(obj_t *) * memtab_capacity) ;
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
		if (MEMTAB_VALID(index)) {
			tmp2 = MEMTAB_DEREF(index) ;
			D_OBJ(tmp2) ; }
		memtab[index] = tmp ;
		MEMTAB_SETV(index, 1) ;
		return C_obj_copy(ptr) ; }
}

int mem_gc(obj_t * code, obj_t * env, obj_t * stack) {
#ifdef NOGC
	/* the trash heap strategy: we just pile it on until we have to get rid of it */
	return 0 ;
#else 
	/* a last-ditch attempt at mark and sweep garbage collection */

	obj_t 		* tmp,
			* tmp2 ;
	olist_t 	* tmplist ;
	size_t 		i ;

	MEM_printf("====[G C]====\n") ;

	/* mark */
	rq_push(code) ;		/*  t h e  */
	rq_push(env) ;  	/* r o o t */
	rq_push(stack) ;    	/*  s e t  */

	while ((tmp = rq_pop())) {
		switch (obj_typeof(tmp)) {
		case T_APP:
			tmplist = app_get_expr_list(tmp) ;
			for (i = 0; i < olist_length(tmplist); ++i) {
				rq_push(olist_get(tmplist, i)) ; }
			break ;
		case T_IF:
			rq_push(if_get_pred(tmp)) ;
			rq_push(if_get_true(tmp)) ;
			rq_push(if_get_false(tmp)) ;
			break ;
		case T_CCC:
			rq_push(ccc_get_expr(tmp)) ;	
			break ;
		case T_BOOL:
			/* no-op */
			break ;
		case T_PRIM:
			/* no-op */
			break ;
		case T_NUM:
			/* no-op */
			break ;
		case T_UNIT:
			/* no-op */
			break ;
		case T_PAIR:
			rq_push(pair_first(tmp)) ;
			rq_push(pair_second(tmp)) ;
			break ;
		case T_IDENT:
			/* no-op */
			break ;
		case T_PTR:
			/* the money */
			mem_ptr_mark(tmp) ;
			MEM_printf("gc: MARK reachable '%s'\n", obj_repr(tmp)) ;
			break ;
		case T_STR:
			/* no-op */
			break ;
		case T_LAM:
			/* the only part of a lam that may have ptrs */
			rq_push(lam_get_expr(tmp)) ;
			break ;
		case T_ABORT:
			rq_push(abort_expr(tmp)) ;
			break ;
		case T_STACK:
			tmplist = stack_data(tmp) ;
			for (i = 0; i < olist_length(tmplist); ++i) {
				rq_push(olist_get(tmplist, i)) ; }
			break ;
		case T_FRIF:
			rq_push(frif_true(tmp)) ;
			rq_push(frif_false(tmp)) ;
			break ;
		case T_FRAPP:
			tmplist = frapp_get_vals(tmp) ;
			for (i = 0; i < olist_length(tmplist); ++i) {
				rq_push(olist_get(tmplist, i)) ; }
			tmplist = frapp_get_exprs(tmp) ;
			for (i = 0; i < olist_length(tmplist); ++i) {
				rq_push(olist_get(tmplist, i)) ; }
			break ;
		case T_FRRET:
			/* no-op */
			break ;
		case T_FRFR:
			rq_push(frfr_get_fr(tmp)) ;
			break ;
		case T_ENV:
			/* we don't care about the list of idents */
			for (i = 0; i < env_girth(tmp); ++i) {
				rq_push(env_get_val(tmp, i)) ; }
			break ;
		case T_CLO:
			rq_push(clo_get_lam(tmp)) ;
			tmp2 = clo_get_env_noref(tmp) ;
			tmp = lam_get_recname(clo_get_lam(tmp)) ;

			for (i = 0; i < env_girth(tmp2); ++i) {
				if (ident_cmp(tmp, (obj_t *)env_get_ident(tmp2, i))) {
					rq_push(env_get_val(tmp2, i)) ; } }

			break ;
		}
	}

	/* sweep */

	for (i = 0; i < memtab_size; ++i) {
		if (!MEMTAB_VALID(i)) { 
			continue ; }
		else if (!MEMTAB_REACH(i)) {
			MEM_printf("gc: SWEEP %zu/%-zu unreachable: deleting...", i, memtab_size - 1) ;
			tmp = MEMTAB_DEREF(i) ;
			D_OBJ(tmp) ;
			MEMTAB_SETV(i, 0) ; }
		else {
			MEM_printf("gc: SWEEP %zu/%-zu REACHABLE...", i, memtab_size - 1) ;
			mem_index_unmark(i) ; } }

	return 0 ;
#endif
}

int mem_sys_down(void) {
	obj_t * tmp ;
	if (!MEM_SYS_VALIDATE()) {
		MEM_SYS_OOPS() ;
		return -1 ; }

	for (size_t i = 0; i < memtab_size; ++i) {
		if (MEMTAB_VALID(i)) {
			MEM_printf("sys_down: free remaining valid address '%lu'\n", i) ;
			tmp = MEMTAB_DEREF(i) ;
			D_OBJ(tmp) ; }
		else if (MEMTAB_REACH(i)) {
			MEM_printf("sys_down: error: reachable but invalid memory exists at sys_down\n") ;
			MEM_SYS_OOPS() ;
			} }

	free(memtab) ;
	free(rq) ;

	if (MEM_SYS_VALIDATE()) {
		MEM_SYS_DOWN() ;
		return 0 ; }
	else {
		return -1 ; }
}
