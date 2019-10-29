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
        return desugar(sexpr.rest())
    # Recursive case
    elif sexpr.length() > 2:
        return e.Application(desugar(sexpr.first()), desugar(sexpr.second()), desugar(s.Cons(sexpr.first(), sexpr.right.rest())))
    else:
        salt()

def desugar_mult(sexpr):
    if sexpr.length() == 1:
        return v.Number(1)
    elif sexpr.length() == 2:
        return desugar(sexpr.rest())
    # Recursive case
    elif sexpr.length() > 2:
        return e.Application(desugar(sexpr.first()), desugar(sexpr.second()), desugar(s.Cons(sexpr.first(), sexpr.right.rest())))
    else:
        salt()

def desugar_minus(sexpr):
    # Identity case?
    if sexpr.length() == 1:
        return v.Number(0)
    # Unary case
    elif sexpr.length() == 2:
        return e.Application(v.Primitive("*"), v.Number(-1), desugar(sexpr.second()))
    # Binary case
    elif sexpr.length() > 2:
        return e.Application(v.Primitive("+"), desugar(sexpr.second()),
            e.Application(v.Primitive("*"), v.Number(-1), desugar(sexpr.right.rest())))
    else:
        salt()

def desugar_div(sexpr):
    if sexpr.length() == 1:
        return v.Number(1)
    elif sexpr.length() == 2:
        return desugar(sexpr.rest())
    # Recursive case
    elif sexpr.length() > 2:
        return e.Application(desugar(sexpr.first()), desugar(sexpr.second()), desugar(s.Cons(sexpr.first(), sexpr.right.rest())))
    else:
        salt()

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
        return v.Lambda(desugar_binding(sexpr.first()),desugar(sexpr.rest()), recname)
    # first->bindings, rest->def
    return v.Lambda(desugar_binding(sexpr.first()),desugar(sexpr.rest()))

def desugar_let(sexpr):
    sexpr = sexpr.rest()
    binditer = sexpr.first()
    binding = []
    call = []
    while not isinstance(binditer, s.Nil):
        binding.append(desugar(binditer.first().first()))
        call.append(desugar(binditer.first().second()))
        binditer = binditer.rest()

    return e.Application(v.Lambda(binding, desugar(sexpr.rest())), *call)

antirecipies = { \
        "-": desugar_minus,
        "+": desugar_plus,
        "*": desugar_mult,
        "/": desugar_div,
        "if": desugar_if,
        "lambda": desugar_lambda,
        "$": desugar_lambda, # Lambdas are money
        "let": desugar_let
        }


def desugar_atom(sexpr):
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
    if isinstance(sexpr, s.Cons):
        return desugar_cons(sexpr)
    elif isinstance(sexpr, s.Atom):
        return desugar_atom(sexpr)
    elif isinstance(sexpr, s.Nil):
        return e.Application(v.Number(0))
    else:
        salt()


# def desugar_top(sexpr):
#     exprs = []
#     siter = sexpr 
#     while not isinstance(siter, s.Nil):
#         exprs.append(desugar(siter.first()))
#         siter = siter.rest()
#     return t.Program(*tuple(exprs))
