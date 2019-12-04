import j4.core.expression as e
import j4.core.value as v
import j4.core.sexpr as s
import j4.core.top as t

import sys

class NotSugar(Exception):
    pass

def salt():
        raise NotSugar("Something is salty with this sexpr...")

def is_number(value):
    try:
        float(value)
    except ValueError:
        return False
    else:
        return True

def desugar_plus(sexpr):
    if sexpr.length() == 1:
        return v.Number(0)
    elif sexpr.length() == 2:
        return desugar(sexpr.rest().first())
    # Recursive case
    elif sexpr.length() > 2:
        return e.Application(desugar(sexpr.first()), desugar(sexpr.second()), desugar(s.Cons(sexpr.first(), sexpr.right.rest())))
    else:
        salt()

def desugar_mult(sexpr):
    if sexpr.length() == 1:
        return v.Number(1)
    elif sexpr.length() == 2:
        return desugar(sexpr.rest().first())
    # Recursive case
    elif sexpr.length() > 2:
        return e.Application(desugar(sexpr.first()), desugar(sexpr.second()), desugar(s.Cons(sexpr.first(), sexpr.right.rest())))
    else:
        salt()

def desugar_minus(sexpr):
    # # Identity case?
    # if sexpr.length() == 1:
    #     return v.Number(0)
    # # Unary case
    # elif sexpr.length() == 2:
    #     return e.Application(v.Primitive("*"), v.Number(-1), desugar(sexpr.second()))
    # # Binary case
    # elif sexpr.length() > 2:
    #     return e.Application(v.Primitive("+"), desugar(sexpr.second()),
    #         e.Application(v.Primitive("*"), v.Number(-1), desugar(sexpr.right.rest())))
    # else:
    #     salt()
    if sexpr.length() == 1:
        return v.Number(0)
    elif sexpr.length() == 2:
        return desugar(sexpr.rest().first())
    # Recursive case
    elif sexpr.length() > 2:
        return e.Application(desugar(sexpr.first()), desugar(sexpr.second()), desugar(s.Cons(sexpr.first(), sexpr.right.rest())))
    else:
        salt()

def desugar_div(sexpr):
    if sexpr.length() == 1:
        return v.Number(1)
    elif sexpr.length() == 2:
        return desugar(sexpr.rest().first())
    # Recursive case
    elif sexpr.length() > 2:
        return e.Application(desugar(sexpr.first()), desugar(sexpr.second()), desugar(s.Cons(sexpr.first(), sexpr.right.rest())))
    else:
        salt()
    # if sexpr.length() == 1:
    #     return v.Number(1)
    # elif sexpr.length() == 2:
    #     return desugar(sexpr.rest())
    # # Recursive case
    # elif sexpr.length() > 2:
    #     return e.Application(desugar(sexpr.first()), desugar(sexpr.second()), desugar(s.Cons(sexpr.first(), sexpr.right.rest())))
    # else:
    #     salt()

def sexpr_to_tuple(sexpr):
    datalist = []
    while not isinstance(sexpr, s.Nil):
        # print(sexpr.first().value, sexpr.rest())
        datalist.append(desugar(sexpr.first()))
        sexpr = sexpr.rest()
    # print("datalist: ")
    # for jv in datalist:
    #     print(jv.repr())
        
    return tuple(datalist)

def desugar_generic_app(sexpr):
    # print(sexpr.first().repr() in v.Prims)
    # print(desugar(sexpr.first()).expressions)
    return e.Application(desugar(sexpr.first()), *sexpr_to_tuple(sexpr.rest()))

def desugar_if(sexpr):
    return e.If(*sexpr_to_tuple(sexpr.rest()))

def desugar_binding(sexpr):
    # 0ary case:
    if isinstance(sexpr.first(), s.Nil):
        return []
    siter = sexpr
    args = []
    while not isinstance(siter, s.Nil):
        args.append(desugar(siter.first()))
        siter = siter.rest()
    return args
    
def desugar_lambda(sexpr):
    sexpr = sexpr.rest()
    if sexpr.length() > 2:
        recname = desugar(sexpr.first())
        sexpr = sexpr.rest()
        return v.Lambda(desugar_binding(sexpr.first()),desugar(sexpr.rest().first()), recname)
    # first->bindings, rest->def
    return v.Lambda(desugar_binding(sexpr.first()),desugar(sexpr.rest().first()))

def desugar_let(sexpr):
    sexpr = sexpr.rest()
    binditer = sexpr.first()
    binding = []
    call = []
    while not isinstance(binditer, s.Nil):
        # binditer.pp()
        # binditer.first().pp()
        # binditer.first().first().pp()
        binding.append(desugar(binditer.first().first()))
        # print("yhello")
        call.append(desugar(binditer.first().second()))
        binditer = binditer.rest()

    return e.Application(v.Lambda(binding, desugar(sexpr.rest().first())), *call)

def desugar_let_star(sexpr):
    body = sexpr.rest().rest()
    # print("ghello")
    bind1 = sexpr.rest().first()
    newbind = s.Cons(bind1.first(), s.Nil())


    if bind1.length() == 1:
        # print("hello")
        return desugar(s.Cons(s.Atom("let"), s.Cons(bind1, body)))
    else:
        # print("fhello")
        bind2 = bind1.rest()
        return desugar(s.Cons(s.Atom("let"), s.Cons(newbind, s.Cons( s.Cons(
            s.Atom("let*"), s.Cons(bind2, body)), s.Nil() ))))

def desugar_pair(sexpr):
    return v.Pair(desugar(sexpr.rest().first()), desugar(sexpr.rest().rest().first()))

antirecipies = { \
        "-":        desugar_minus,
        "+":        desugar_plus,
        "*":        desugar_mult,
        "/":        desugar_div,
        "if":       desugar_if,
        "lambda":   desugar_lambda,
        "$":        desugar_lambda, # Lambdas are money
        "let":      desugar_let,
        "let*":     desugar_let_star,
        "pair":     desugar_pair,
        # "fst":      desugar_fst,
        # "snd":      desugar_snd,
        }


def desugar_atom(sexpr):
    if not isinstance(sexpr, s.Atom):
        desugar(sexpr)

    if is_number(sexpr.repr()):
        return v.Number(float(sexpr.repr()))
    elif sexpr.repr() in v.Prims:
        return v.Primitive(sexpr.repr())
    # Sugarbools
    elif sexpr.repr() == "#t":
        return v.Bool(1)
    elif sexpr.repr() == "#f":
        return v.Bool(0)
    # Must be an id then
    else:
        return v.ID(sexpr.repr())


def desugar_cons(sexpr):
    action = sexpr.first().repr()
    if action in antirecipies:
        return antirecipies[action](sexpr)
    elif sexpr.length() == 1:
        return desugar(sexpr.first())
    # Might be function call or something
    else:
        return e.Application(*sexpr_to_tuple(sexpr))

def desugar(sexpr):
    # sexpr.pp()
    if isinstance(sexpr, s.Atom):
        return desugar_atom(sexpr)


    if isinstance(sexpr, s.Nil):
        return e.Application(v.Number(0))         # invalid

    if isinstance(sexpr, s.Atom):
        return desugar_atom(sexpr)

    action = sexpr.first().repr()
    if action in antirecipies:
        return antirecipies[action](sexpr)

    else:
        return desugar_generic_app(sexpr)
