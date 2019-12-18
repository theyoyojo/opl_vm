#ifndef INTERPINFO_H
#define INTERPINFO_H

#include "obj.h"

typedef enum interp_obj_tag {
	I_CODE = 0,
	I_ENV,
	I_KONT,
	I_STACK,
} interp_obj_tag_t ;

typedef struct interp_objtab_setup_entry {
	interp_obj_tag_t tag ;
	obj_t * (*func)(void) ;
} interp_objtab_setup_entry_t ;

enum interp_state { IST_OFF, IST_RUNNING, IST_ERROR } ;


obj_t * interp_get(interp_obj_tag_t tag) ;
int interp_objtab_setup(interp_objtab_setup_entry_t * setuptab, size_t setuptab_size) ;

int interp_running(void) ;

void interp_set_state(enum interp_state state) ;


#endif /* INTERPINFO_H */
