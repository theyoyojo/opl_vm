#include "../../lil_test/src/lil_test.h"
#include "olist.h"
#include "types.h"
#include "delta.h"
#include "interp.h"

#define ALLOC_OR_FAIL(id, type) type * id = (type *)malloc(sizeof(type)) ;\
					    ASSERT(id)

TEST_SET(simple_exprs,
	TEST_CASE(simple_number,
		obj_t * x = C_num(4) ;
		ASSERT(((num_t *)x)->value == 4) ;
		D_num(&x) ;
	) ;
	TEST_CASE(simple_bool,
		obj_t * x = C_bool(true);
		ASSERT(((bool_t *)x)->value == true) ;
		D_bool(&x) ;
	) ;
	
	TEST_CASE(simple_prim,
		obj_t * x = C_prim(">=");
		ASSERT(((prim_t *)x)->value == PRIM_GTEQ) ;
		ASSERT(((prim_t *)x)->value != PRIM_LT) ;
		D_prim(&x) ;
	) ;
	TEST_CASE(simple_if,
		obj_t * x = C_bool(true);
		obj_t * y = C_num(4);
		obj_t * z = C_prim("+");

		obj_t * a = C_if(x,y,z) ;

		D_if(&a) ;
	) ;
	TEST_CASE(number_in_list,
		olist_t * list = olist_init() ;
		obj_t * x = C_num(4) ;
		olist_append(list, x) ;
		olist_free(&list) ;
	) ;
	TEST_CASE(simple_app,	
		obj_t * x = C_bool(true);
		obj_t * y = C_num(4);
		obj_t * z = C_prim("+");
		obj_t * a = C_app(3, x,y,z) ;

		D_app(&a) ;
	) ;
	TEST_CASE(app_list,
		obj_t * x = C_bool(true);
		obj_t * y = C_num(4);
		obj_t * z = C_prim("+");
		obj_t * a = C_app(3, x,y,z) ;

		obj_t * p = C_bool(true);
		obj_t * q = C_num(4);
		obj_t * r = C_prim("+");
		obj_t * b = C_app(3, p,q,r) ;

		olist_t * list = olist_init() ;
		olist_append(list, a) ;
		olist_append(list, b) ;

		olist_free(&list) ;
	) ;
	TEST_CASE(olist_pop_empty_is_null,
		olist_t * list = olist_init() ;
		ASSERT(!olist_pop(list)) ;
		olist_free(&list) ;
	) ;
	TEST_CASE(ident,
		obj_t * ident = C_ident("test") ;
		ASSERT(!strcmp("test", ((ident_t *)ident)->value)) ;
		obj_t * ident2 = C_obj_copy(ident) ;
		ASSERT(!strcmp("test", ((ident_t *)ident2)->value)) ;
		D_OBJ(ident) ;
		D_OBJ(ident2) ;
	) ;
	TEST_CASE(bind,
		obj_t * f = C_ident("f") ;
		obj_t * x = C_ident("x") ;
		obj_t * y = C_ident("y") ;
		olist_t * bind = olist_init_data(3, f,x,y) ;
		ASSERT(!strcmp("x", ((ident_t *)olist_get(bind,1))->value)) ;
		olist_free(&bind) ;
	) ;
	TEST_CASE(func,
		obj_t * f = C_ident("f") ;
		obj_t * x = C_ident("x") ;
		obj_t * y = C_ident("y") ;
		olist_t * bind = olist_init_data(3, f,x,y) ;
		obj_t * plus = C_prim("+") ;
		obj_t * x2 = C_obj_copy(x) ;
		obj_t * y2 = C_obj_copy(y) ;
		obj_t * app = C_app(3, plus, x2, y2) ;
		obj_t * func = C_func(bind, app) ;
		ASSERT(!strcmp("x", ((ident_t *)olist_get(
					((func_t *)func)->binding,1))->value)) ;
		ASSERT(!strcmp(((ident_t *)olist_get(((app_t *)((func_t *)
					func)->expr)->expr_list,2))->value, "y")) ;
		ASSERT(obj_typeof(olist_get(((app_t *)((func_t *)
					func)->expr)->expr_list,0)) == T_PRIM ) ;
		ASSERT(!strcmp(func_get_name(func), "f")) ;
		D_OBJ(func) ;
	) ;
	TEST_CASE(prog,
		obj_t * f = C_ident("f") ;
		obj_t * x = C_ident("x") ;
		obj_t * y = C_ident("y") ;
		olist_t * bind = olist_init_data(3, f,x,y) ;
		obj_t * plus = C_prim("+") ;
		obj_t * x2 = C_obj_copy(x) ;
		obj_t * y2 = C_obj_copy(y) ;
		obj_t * app = C_app(3, plus, x2, y2) ;
		obj_t * func = C_func(bind, app) ;

		obj_t * call = C_ident("f") ;
		obj_t * one = C_num(66) ;
		obj_t * two = C_num(600) ;
		obj_t * app2 = C_app(3, call, one, two) ;

		obj_t * top =  C_prog() ;
		prog_append(top, func) ;
		prog_append(top, app2) ;

		obj_t * res = interpret(top) ;
		printf("%lg\n", ((num_t *)res)->value) ;

		D_OBJ(res) ;
		D_OBJ(top) ;

	 ) ;

	 TEST_CASE(env,
		obj_t * env = C_env() ;
		obj_t * f = C_ident("f") ;
		obj_t * x = C_ident("x") ;
		olist_t * binding = olist_init_data(2, f, x) ;
		obj_t * one = C_num(1) ;
		olist_t * call = olist_init_data(2, C_obj_copy(f), one) ;
		/* bind does NOT consume the data passed to it */
		env_bind(env, binding, call) ;
		olist_free(&binding) ;
		olist_free(&call) ;
		
		obj_t * one2 = env_subst(env, C_ident("x")) ;
		ASSERT(((num_t *)one2)->value == 1) ;

		obj_t * env2 = C_obj_copy(env) ;
		obj_t * one3 = env_subst(env2, C_ident("x")) ;
		ASSERT(((num_t *)one3)->value == 1) ;

		D_OBJ(one2) ;
		D_OBJ(one3) ;
		
		D_OBJ(env) ;
		D_OBJ(env2) ;
	) ;
) ;

TEST_SET(deltas,
	TEST_CASE(plus,
		obj_t * op = C_prim("+") ;
		obj_t * a = C_num(4) ;
		obj_t * b = C_num(7) ;
		obj_t * app = C_app(3, op, a, b) ;
		obj_t * c = delta(app) ;
		ASSERT(((num_t *)c)->value == 11) ;
		c->head.D_obj(&c) ;
		app->head.D_obj(&app) ;
	) ;
	TEST_CASE(mult,
		obj_t * op = C_prim("*") ;
		obj_t * a = C_num(4) ;
		obj_t * b = C_num(7) ;
		obj_t * app = C_app(3, op, a, b) ;
		obj_t * c = delta(app) ;
		ASSERT(((num_t *)c)->value == 28) ;
		c->head.D_obj(&c) ;
		app->head.D_obj(&app) ;
	) ;
	TEST_CASE(div,
		obj_t * op = C_prim("/") ;
		obj_t * a = C_num(4) ;
		obj_t * b = C_num(7) ;
		obj_t * app = C_app(3, op, a, b) ;
		obj_t * c = delta(app) ;
		ASSERT(((num_t *)c)->value == 4.0/7) ;
		c->head.D_obj(&c) ;
		app->head.D_obj(&app) ;
	) ;
	TEST_CASE(div_by_zero_retnull,
		obj_t * op = C_prim("/") ;
		obj_t * a = C_num(4) ;
		obj_t * b = C_num(0) ;
		obj_t * app = C_app(3, op, a, b) ;
		obj_t * c = delta(app) ;
		ASSERT(!c)
		app->head.D_obj(&app) ;
	) ;
	TEST_CASE(sub,
		obj_t * op = C_prim("-") ;
		obj_t * a = C_num(4) ;
		obj_t * b = C_num(7) ;
		obj_t * app = C_app(3, op, a, b) ;
		obj_t * c = delta(app) ;
		ASSERT(((num_t *)c)->value == -3) ;
		c->head.D_obj(&c) ;
		app->head.D_obj(&app) ;
	) ;

	TEST_CASE(gt,
		obj_t * op = C_prim(">") ;
		obj_t * a = C_num(4) ;
		obj_t * b = C_num(7) ;
		obj_t * app = C_app(3, op, a, b) ;
		obj_t * c = delta(app) ;
		ASSERT(((bool_t*)c)->value == false) ;
		c->head.D_obj(&c) ;
		app->head.D_obj(&app) ;
	) ;

	TEST_CASE(gteq,
		obj_t * op = C_prim(">=") ;
		obj_t * a = C_num(4) ;
		obj_t * b = C_num(7) ;
		obj_t * app = C_app(3, op, a, b) ;
		obj_t * c = delta(app) ;
		assert(((bool_t*)c)->value == false) ;
		c->head.D_obj(&c) ;
		app->head.D_obj(&app) ;
	) ;
	TEST_CASE(eq,
		obj_t * op = C_prim("=") ;
		obj_t * a = C_num(4) ;
		obj_t * b = C_num(7) ;
		obj_t * app = C_app(3, op, a, b) ;
		obj_t * c = delta(app) ;
		ASSERT(((bool_t*)c)->value == false) ;
		c->head.D_obj(&c) ;
		app->head.D_obj(&app) ;
	) ;
	TEST_CASE(lt,
		obj_t * op = C_prim("<") ;
		obj_t * a = C_num(4) ;
		obj_t * b = C_num(7) ;
		obj_t * app = C_app(3, op, a, b) ;
		obj_t * c = delta(app) ;
		ASSERT(((bool_t*)c)->value == true) ;
		c->head.D_obj(&c) ;
		app->head.D_obj(&app) ;
	) ;
	TEST_CASE(lteq,
		obj_t * op = C_prim("<=") ;
		obj_t * a = C_num(4) ;
		obj_t * b = C_num(7) ;
		obj_t * app = C_app(3, op, a, b) ;
		obj_t * c = delta(app) ;
		ASSERT(((bool_t*)c)->value == true) ;
		c->head.D_obj(&c) ;
		app->head.D_obj(&app) ;
	) ;
) ;

TEST_SET(copy,
	TEST_CASE(copy_number,
		obj_t * x = C_num(4) ;
		obj_t * y = C_num_copy(x) ;
		ASSERT(((num_t *)y)->value == 4) ;
		D_obj(x)(&x) ;
		D_obj(y)(&y) ;
	) ;
	TEST_CASE(copy_bool,
		obj_t * x = C_bool(false);
		obj_t * y = C_bool_copy(x) ;
		ASSERT(((bool_t *)y)->value == false) ;
		D_obj(x)(&x) ;
		D_obj(y)(&y) ;
	) ;
	
	TEST_CASE(copy_prim,
		obj_t * x = C_prim(">=");
		obj_t * y = C_obj_copy(x) ;
		ASSERT(((prim_t *)y)->value == PRIM_GTEQ) ;
		ASSERT(((prim_t *)y)->value != PRIM_LT) ;
		D_obj(x)(&x) ;
		D_obj(y)(&y) ;
	) ;
	TEST_CASE(copy_if,
		obj_t * x = C_bool(true);
		obj_t * y = C_num(4);
		obj_t * z = C_prim("+");

		obj_t * a = C_if(x,y,z) ;
		obj_t * b = C_obj_copy(a) ;

		D_obj(a)(&a) ;
		ASSERT(((num_t *)((if_t *)b)->expr_true)->value == 4) ;
		ASSERT(((prim_t *)((if_t *)b)->expr_false)->value == PRIM_PLUS) ;
		ASSERT(((bool_t *)((if_t *)b)->expr_pred)->value == true) ;

		D_obj(b)(&b) ;
	) ;
	TEST_CASE(copy_app,	
		obj_t * x = C_bool(true);
		obj_t * y = C_num(4);
		obj_t * z = C_prim("+");
		obj_t * a = C_app(3, x,y,z) ;
		obj_t * b = C_obj_copy(a) ;

		ASSERT(((num_t *)olist_get(((app_t *)b)->expr_list,1))->value == 4) ;

		D_obj(a)(&a) ;
		D_obj(b)(&b) ;
	) ;
) ;

TEST_SET(interpretation,
	TEST_CASE(degenerate,
		obj_t * x = C_num(4);
		obj_t * y = interpret(x) ;
		printf("result: %lg\n", ((num_t *)y)->value) ;	
		D_OBJ(x) ;
		D_OBJ(y) ;
	) ;

	TEST_CASE(add,
		obj_t * w = C_prim("+");
		obj_t * x = C_num(4);
		obj_t * y = C_num(7);
		obj_t * a = C_app(3,w,x,y) ;
		obj_t * z = interpret(a) ;
		printf("result: %lg\n", ((num_t *)z)->value) ;	
		D_OBJ(a) ;
		D_OBJ(z) ;
	) ;

	TEST_CASE(if_true,
		obj_t * x = C_bool(true);
		obj_t * y = C_num(7);
		obj_t * w = C_num(4);
		obj_t * a = C_if(x,y,w) ;
		obj_t * z = interpret(a) ;
		printf("result: %lf\n", ((num_t *)z)->value) ;	
		D_OBJ(a) ;
		D_OBJ(z) ;
	) ;

	TEST_CASE(generated,
		obj_t * _o2 = C_num(1.0) ;
		obj_t * _o3 = C_num(7.0) ;
		obj_t * _o4 = C_num(4.0) ;
		obj_t * _o1 = C_if(_o2, _o3, _o4) ;

		obj_t * result = interpret(_o1) ;
		printf("%lg\n", ((num_t *)result)->value) ;
		D_OBJ(result) ;
		D_OBJ(_o1) ;
	) ;
	TEST_CASE(generated2,
		obj_t * _o2 = C_num(1.0) ;
		obj_t * _o4 = C_prim("+") ;
		obj_t * _o5 = C_num(3.0) ;
		obj_t * _o7 = C_prim("*") ;
		obj_t * _o8 = C_num(-1.0) ;
		obj_t * _o9 = C_num(8.0) ;
		obj_t * _o6 = C_app(3, _o7, _o8, _o9) ;
		obj_t * _o3 = C_app(3, _o4, _o5, _o6) ;
		obj_t * _o10 = C_num(4.0) ;
		obj_t * _o1 = C_if(_o2, _o3, _o10) ;

		obj_t * result = interpret(_o1) ;
		printf("%lg\n", ((num_t *)result)->value) ;
		D_OBJ(result) ;

		D_OBJ(_o1) ;
	) ;
	TEST_CASE(prog,
		obj_t * f = C_ident("f") ;
		obj_t * x = C_ident("x") ;
		olist_t * bind = olist_init_data(2, f,x) ;
		obj_t * mult = C_prim("*") ;
		obj_t * x2 = C_obj_copy(x) ;
		obj_t * x3 = C_obj_copy(x) ;
		obj_t * app = C_app(3, mult , x2, x3) ;
		obj_t * func = C_func(bind, app) ;

		obj_t * call = C_ident("f") ;
		obj_t * one = C_num(4) ;
		obj_t * app2 = C_app(2, call, one) ;

		obj_t * top =  C_prog() ;
		prog_append(top, func) ;
		prog_append(top, app2) ;

		obj_t * res = interpret(top) ;
		printf("%lg\n", ((num_t *)res)->value) ;

		D_OBJ(res) ;
		D_OBJ(top) ;

	 ) ;
	TEST_CASE(prog_if,
		obj_t * f = C_ident("f") ;
		obj_t * x = C_ident("x") ;
		olist_t * bind = olist_init_data(2, f,x) ;
		obj_t * one = C_num(1) ;
		obj_t * two = C_num(2) ;
		obj_t * x2 = C_obj_copy(x) ;
		obj_t * ifex = C_if(x2, one, two) ;
		obj_t * func = C_func(bind, ifex) ;

		obj_t * call = C_ident("f") ;
		obj_t * four = C_num(0) ;
		obj_t * app2 = C_app(2, call, four) ;

		obj_t * top =  C_prog() ;
		prog_append(top, func) ;
		prog_append(top, app2) ;

		obj_t * res = interpret(top) ;
		printf("%lg\n", ((num_t *)res)->value) ;

		D_OBJ(res) ;
		D_OBJ(top) ;

	 ) ;

	TEST_CASE(prog_if_x,
		obj_t * f = C_ident("f") ;
		obj_t * x = C_ident("x") ;
		olist_t * bind = olist_init_data(2, f,x) ;
		obj_t * eq = C_prim("=") ;
		obj_t * one = C_num(1) ;
		obj_t * x2 = C_obj_copy(x) ;
		obj_t * pred = C_app(3, eq, x2, one) ;
		obj_t * two = C_num(2) ;
		obj_t * three = C_num(3) ;
		obj_t * ifex = C_if(pred, two, three) ;
		obj_t * func = C_func(bind, ifex) ;

		obj_t * call = C_ident("f") ;
		obj_t * four = C_num(1) ;
		obj_t * app2 = C_app(2, call, four) ;

		obj_t * top =  C_prog() ;
		prog_append(top, func) ;
		prog_append(top, app2) ;

		obj_t * res = interpret(top) ;
		printf("%lg\n", ((num_t *)res)->value) ;

		D_OBJ(res) ;
		D_OBJ(top) ;

	 ) ;
	 TEST_CASE(sum,

		obj_t * f = C_ident("f") ;

		obj_t * x = C_ident("x") ;
		obj_t * one = C_num(1) ;
		obj_t * eq = C_prim("=") ;
		obj_t * pred = C_app(3, eq, x, one) ;
		
		obj_t * one2 = C_num(1) ;

		obj_t * one3 = C_num(1) ;
		obj_t * sub = C_prim("-") ;
		obj_t * x3 = C_ident("x") ;
		obj_t * decr = C_app(3, sub, x3, one3) ;

		obj_t * f2 = C_ident("f") ;
		obj_t * recur = C_app(2, f2, decr) ;

		obj_t * x4 = C_ident("x") ;
		obj_t * plus = C_prim("+") ;
	 	obj_t * sum = C_app(3, plus, x4, recur) ;

		obj_t * business = C_if(pred,one2, sum) ;

		obj_t * x2 = C_ident("x") ;
		olist_t * bind = olist_init_data(2, f, x2) ;
		obj_t * func = C_func(bind, business) ;

		obj_t * two = C_num(100) ;
		obj_t * f3 = C_ident("f") ;
		obj_t * call = C_app(2, f3, two) ;

		obj_t * top =  C_prog() ;
		prog_append(top, func) ;
		prog_append(top, call) ;

		obj_t * res = interpret(top) ;
		printf("%lg\n", ((num_t *)res)->value) ;
		ASSERT(((num_t *)res)->value == 5050) ;

		D_OBJ(res) ;
		D_OBJ(top) ;
	 ) ;


) ;


TEST_MAIN();
