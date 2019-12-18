#include "interpinfo.h"

static obj_t * (*interp_objtab[])(void) = {
	[I_CODE] 	= NULL,
	[I_ENV] 	= NULL,
	[I_KONT] 	= NULL,
	[I_STACK] 	= NULL,
} ;

size_t interp_objtab_size = sizeof(interp_objtab) / sizeof(obj_t (*)(void)) ;

static enum interp_state _interp_state = IST_OFF ;

int interp_running(void) {
	return _interp_state == IST_RUNNING ;
}

void interp_set_state(enum interp_state state) {
	_interp_state = state ;
}

obj_t * interp_get(interp_obj_tag_t tag) {
	return interp_objtab[tag]() ;
}

int interp_objtab_setup(interp_objtab_setup_entry_t * setuptab, size_t setuptab_size) {
	size_t i ;
	interp_objtab_setup_entry_t * iter ;

	iter = NULL ;

	for (i = 0, iter = setuptab; i < setuptab_size; ++iter, ++i) {
		if (i >= interp_objtab_size) {
			return -1 ;
		}
		interp_objtab[iter->tag] = iter->func ;
	}

	return 0 ;
}
