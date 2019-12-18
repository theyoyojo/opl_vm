#include "mem.h"
#include "types.h"
#include "stack.h"
#include "interpinfo.h"
#include <string.h>

/* #ifdef NOGC */
/* static enum mem_mode mem_mode = MM_NOGC ; */
/* #else */
/* #ifdef SCGC */
/* static enum mem_mode mem_mode = MM_SCGC ; */
/* #else */
/* static enum mem_mode mem_mode = MM_MSGC ; */
/* #endif /1* ifdef SCGC *1/ */

/* #endif /1* ifdef NOGC *1/ */

static int init_success = -1 ;

static obj_t ** memtab ;
static size_t 	memtab_size ;
static size_t 	memtab_capacity ;

#ifndef NOGC
/* #define SCGC */
#undef SCGC
#endif 

#ifdef SCGC

static obj_t ** memtab_1 ;
static size_t 	memtab_1_size ;
static size_t 	memtab_1_capacity ;

static obj_t ** memtab_2 ;
static size_t 	memtab_2_size ;
static size_t 	memtab_2_capacity ;

static size_t 	memtab_current = 0 ;

#endif 

struct tmplist {
	obj_t * tmp ;
#ifdef SCGC
	unsigned long addr ;
	unsigned long new_addr ;
#endif
	struct tmplist * next ;
} ;


#ifdef SCGC
#define MEMTAB_CAPACITY_INIT 	16	/* low to trigger gc to demonstrate */
#else
#define MEMTAB_CAPACITY_INIT 	64
#endif

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
	size_t i, j, old_capacity ;
	if (rq_size >= rq_capacity - 10) {
		old_capacity = rq_capacity ;
		rq_capacity *= rq_capacity ;
		if (!(tmp_ptr = (obj_t **)malloc(sizeof(obj_t *) * rq_capacity))) {
			rq_capacity = old_capacity ;
			return -1 ; }
		MEM_printf("rq_push: rootqueue capacity enlarged: %zu -> %zu\n", old_capacity, rq_capacity) ;
		MEM_printf("rq_push: move rq %p -> %p\n", rq, tmp_ptr) ;
		/* copy queue into new mem so it starts from zero */
		for (i = rq_first, j = 0; j < rq_size; i = (i + 1) % old_capacity, ++j) {
			tmp_ptr[j] = rq[i] ; }
		rq_first = 0 ;
		rq_last = rq_size - 1 ;
		free(rq) ;
		rq = tmp_ptr ;
	}

	/* printf("writing to rq[%p, last=%zu, size=%zu]\n", rq + rq_last, rq_last, rq_size) ; */
	rq[rq_last] = obj ;

	rq_last = (rq_last + 1) % rq_capacity ;
	++rq_size ;
	return 0 ;
}

obj_t * rq_pop(void) {
	obj_t * tmp ;
	if (rq_size <= 0) {
		return NULL ; }

	/* printf("reading from rq[%p, first=%zu, size=%zu]\n", rq + rq_first, rq_first, rq_size) ; */
	tmp = rq[rq_first] ;
	rq_first = (rq_first + 1) % rq_capacity ;
	--rq_size ;
	return tmp ;
}

#ifdef SCGC
static inline void mem_switch_memtab(void) {
	switch(memtab_current) {
	case 1:
		memtab_1_size 		= memtab_size ;
		memtab_1_capacity 	= memtab_capacity ;
		memtab 			= memtab_2 ;
		memtab_size 		= memtab_2_size ;
		memtab_capacity 	= memtab_2_capacity ;
		memtab_current 		= 2 ;
	case 2:
		memtab_2_size 		= memtab_size ;
		memtab_2_capacity 	= memtab_capacity ;
		memtab 			= memtab_1 ;
		memtab_size 		= memtab_1_size ;
		memtab_capacity 	= memtab_1_capacity ;
		memtab_current 		= 1 ;
	case 0:
	default:
		return ;
	}
}
#endif

int mem_sys_up(void) {
#ifdef SCGC
	memtab_1_capacity = MEMTAB_CAPACITY_INIT ;
	if (!(memtab_1 = (obj_t **)malloc(sizeof(obj_t *) * memtab_1_capacity))) {
		MEM_SYS_OOPS() ;
		return -1 ; }
	memtab_1_size  = 0 ;

	memtab_2_capacity = MEMTAB_CAPACITY_INIT ;
	if (!(memtab_2 = (obj_t **)malloc(sizeof(obj_t *) * memtab_2_capacity))) {
		MEM_SYS_OOPS() ;
		return -1 ; }
	memtab_2_size  = 0 ;

	memset(memtab_1, 0, memtab_1_capacity) ;
	memset(memtab_2, 0, memtab_2_capacity) ;

	memtab 		= memtab_1 ;
	memtab_size 	= memtab_1_size ;
	memtab_capacity = memtab_1_capacity ;
	memtab_current = 1 ;
#else

	memtab_capacity = MEMTAB_CAPACITY_INIT ;
	if (!(memtab = (obj_t **)malloc(sizeof(obj_t *) * memtab_capacity))) {
		MEM_SYS_OOPS() ;
		return -1 ; }

	memtab_size  = 0 ;
	memset(memtab, 0, memtab_capacity) ;
#endif

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

#ifdef SCGC
	if (memtab_size >= memtab_capacity) {
		if (interp_running()) {
			mem_gc(interp_get(I_CODE), interp_get(I_ENV), interp_get(I_KONT)) ;
			if (memtab_size >= memtab_capacity) {
				goto gc_did_not_help_just_run_realloc ; }
			else {
				goto gc_did_a_great_job_and_made_some_space ; } }
gc_did_not_help_just_run_realloc:

#else
	/* if we get close to the limit, square it */
	if (memtab_size >= memtab_capacity - 10) {
#endif
		/* printf("RESIZE: %zu --> %zu\n", memtab_capacity, memtab_capacity * memtab_capacity) ; */
		memtab_capacity *= memtab_capacity ;
#ifndef SCGC
		tmp_ptr = (obj_t **)realloc(memtab, sizeof(obj_t *) * memtab_capacity) ;
#else
		obj_t ** tmp_ptr_1, ** tmp_ptr_2 ;
		
		tmp_ptr_1 = (obj_t **)realloc(memtab_1, sizeof(obj_t *) * memtab_capacity) ;
		
		if (tmp_ptr_1) {	
			tmp_ptr_2 = (obj_t **)realloc(memtab_2, sizeof(obj_t *) * memtab_capacity) ;
			if (tmp_ptr_2) {
				memtab_1 		= tmp_ptr_1 ;
				memtab_1_capacity 	= memtab_capacity ;
				memtab_2 		= tmp_ptr_2 ;
				memtab_2_capacity 	= memtab_capacity ;
				switch(memtab_current) {
				case 1:
					memtab = tmp_ptr = memtab_1 ;
					break ;
				case 2:
					memtab = tmp_ptr = memtab_2 ;
					break ;
				}

			} else {
				free(tmp_ptr_1) ;
				tmp_ptr = NULL ;
			}
		} else {
			tmp_ptr = NULL ;
		}
#endif
		if (!tmp_ptr) {
			MEM_SYS_OOPS() ;
			if (memtab_capacity) {
				memtab_capacity /= memtab_capacity ; }
			return C_abort(C_str("Exception: memory system in invalid state")) ; }
		memtab = tmp_ptr ; }
#ifdef SCGC
gc_did_a_great_job_and_made_some_space:
#endif

	tmp = C_obj_copy(obj) ;
	if (!tmp) {
		MEM_SYS_OOPS() ;
		return C_abort(C_str("Exception: unable to allocate memory")) ; }
	else {
		memtab[memtab_size++] = tmp ;
		MEMTAB_SETV(memtab_size - 1, 1) ;
#ifdef SCGC
		return C_ptr((void *)(memtab_size - 1), 0x4) ; }
#else
#ifndef NOGC
		return C_ptr((void *)(memtab_size - 1), 0x2) ; }
		/* honestly the ptr size field is pretty meaningless */
#else
		return C_ptr((void *)(memtab_size - 1), 0x1) ; }
#endif /* ifndef NOGC */
#endif /* ifdef SCGC */
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
	/* all allocated memory will be reported as free'd in MEM_DEBUG mode by sys_down */
	return 0 ;
#else 
	MEM_printf("====[G C]====\n") ;

	obj_t 		* tmp,
			* tmp2 ;
	olist_t 	* tmplist ;
	size_t 		i ;
	struct tmplist  * head,
			* tmplist_tmp;

	head = tmplist_tmp = NULL ;

#ifdef SCGC /* maybe I can do this one too... */
	MEM_printf("===[S & C]===\n") ;
	unsigned long addr_current ;
	if (memtab_size >= memtab_capacity/2) { /* artificially low to see effect */
		
#else
	MEM_printf("===[M & S]===\n") ;
	/* mark */
#endif


	rq_push(code) ;		/*  t h e  */
	rq_push(env) ;  	/* r o o t */
	rq_push(stack) ;    	/*  s e t  */

	MEM_printf("Searching roots") ;
	while ((tmp = rq_pop())) {
		MEM_printf(".") ;
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
#ifdef SCGC

#else
			mem_ptr_mark(tmp) ;
			MEM_printf("gc: MARK reachable '%s'\n", obj_repr(tmp)) ;
#endif
			/* we need to search at the address of the pointer for other pointers,
			 * but doing so requires we copy the object from memory. mem_gc is
			 * respobsible for this temporarily allocated memory.
			 */
#ifdef SCGC
			addr_current = (unsigned long)ptr_addr(tmp) ;
			for (tmplist_tmp = head; tmplist_tmp; tmplist_tmp = tmplist_tmp->next) {
				if (tmplist_tmp->addr == addr_current) {
					goto fwd_ptr_installed; } }
			goto first_ptr_visit ;
fwd_ptr_installed:
			MEM_printf("gc: FWD addr from[%lx] -> to[%lx]\n",
					(unsigned long)ptr_addr(tmp), tmplist_tmp->new_addr) ;
			((ptr_t *)tmp)->addr = (void *)tmplist_tmp->new_addr ;
			break ;
first_ptr_visit:

#endif
			if (!(tmplist_tmp = (struct tmplist *)malloc(sizeof(struct tmplist)))) {
				fprintf(stderr, "gc: Critical allocaiton failure\n") ;
				exit(1) ; }
			tmplist_tmp->tmp = mem_deref(tmp) ;
#ifdef SGCG
			tmplist_tmp->addr = (unsigned long)ptr_addr(tmp) ;
			mem_switch_memtab() ; /* now in tospace */
			tmp2 = mem_alloc_init(tmplist_tmp->tmp) ;
			MEM_printf("gc: COPY from[%lx] -> to[%lx]\n", tmplist_tmp->addr,
					(unsigned long)ptr_addr(tmp2)) ;
			tmplist_tmp->new_addr = (unsigned long)ptr_addr(tmp2) ;
			D_OBJ(tmp2) ;
			mem_switch_memtab() ; /* back in fromspace */
#endif
			tmplist_tmp->next = head ;
			head = tmplist_tmp ;
			rq_push(head->tmp) ;
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
	MEM_printf("\n") ;


#ifndef SCGC
	/* sweep */
	for (i = 0; i < memtab_size; ++i) {
		if (!MEMTAB_VALID(i)) { 
			continue ; }
		else if (!MEMTAB_REACH(i)) {
			MEM_printf("gc: SWEEP %zu/%-zu unreachable: deleting...\n", i, memtab_size - 1) ;
			tmp = MEMTAB_DEREF(i) ;
			D_OBJ(tmp) ;
			MEMTAB_SETV(i, 0) ; }
		else {
			MEM_printf("gc: SWEEP %zu/%-zu REACHABLE...\n", i, memtab_size - 1) ;
			mem_index_unmark(i) ; } }
#else
	for (i = 0; i < memtab_size; ++i) {
		if (MEMTAB_VALID(i)) {
			tmp = MEMTAB_DEREF(i) ;
			D_OBJ(tmp) ;
			MEMTAB_SETV(i, 0) ; } }	/* invalidate entire fromspace */
	mem_switch_memtab() ; /* finish in tospace which becomes new fromspace */
	}
#endif /* ifndef SCGC */


	for (;head;) {
		tmplist_tmp = head->next ;
		D_OBJ(head->tmp) ;
		free(head) ;
		head = tmplist_tmp ; }

	if (rq_size != 0) {
		fprintf(stderr, "Error: garbage collector broke\n") ;
		exit(1) ; }


	return 0 ;
#endif /* ifdef NOGC */
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

#ifdef SCGC
	free(memtab_1) ;
	free(memtab_2) ;
	memtab = NULL ;
	memtab_current = 0 ;
#else
	free(memtab) ;
#endif
	free(rq) ;

	if (MEM_SYS_VALIDATE()) {
		MEM_SYS_DOWN() ;
		return 0 ; }
	else {
		return -1 ; }
}
