#ifndef INTERP_H
#define INTERP_H

#include "obj.h"

obj_t * subst_expr(obj_t * expr, obj_t * ident, obj_t * value) ;

obj_t * subst_func(obj_t * func, obj_t * call) ;

obj_t * interpret(obj_t * program) ;

#endif /* !INTERP_H */
