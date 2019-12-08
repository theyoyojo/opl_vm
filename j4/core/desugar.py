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

def desugar_arith(sexpr):
    if sexpr.length() == 1:
        if sexpr.first().repr() == "+" or sexpr.first.repr() == "-":
            return v.Number(0)
        else:
            return v.Number(1)
    elif sexpr.length() == 2:
        return desugar(sexpr.rest().first())
    # Recursive case
    elif sexpr.length() > 2:
        return e.Application(desugar(sexpr.first()), desugar(sexpr.second()), desugar(s.Cons(sexpr.first(), sexpr.right.rest())))
    else:
        salt()

def sexpr_to_tuple(sexpr):
    datalist = []
    while not isinstance(sexpr, s.Nil):
        datalist.append(desugar(sexpr.first()))
        sexpr = sexpr.rest()
    return tuple(datalist)

def desugar_generic_app(sexpr):
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

def generate_garbage_ident():
    generate_garbage_ident.count += 1
    return "X__foobarbazwhocares" + str(generate_garbage_ident.count)

generate_garbage_ident.count = 0

def recursive_letify(sexpr):
    if sexpr.length() == 1:
        return sexpr.first()
    else:
        return s.Cons(s.Atom("let"), s.Cons(s.Cons(s.Cons(s.Atom(generate_garbage_ident()),
            s.Cons(sexpr.first(), s.Nil())), s.Nil()) , s.Cons(recursive_letify(sexpr.rest()),s.Nil())))
    
def desugar_lambda(sexpr):
    sexpr = sexpr.rest()

    if isinstance(sexpr.first(), s.Atom):
        recname = desugar(sexpr.first())
        sexpr = sexpr.rest()
    else:
        recname = v.ID("rec")
    return v.Lambda(desugar_binding(sexpr.first()), desugar(recursive_letify(sexpr.rest())), recname)

def desugar_let(sexpr):
    sexpr = sexpr.rest()
    binditer = sexpr.first()
    binding = []
    call = []
    while not isinstance(binditer, s.Nil):
        binding.append(desugar(binditer.first().first()))
        call.append(desugar(binditer.first().second()))
        binditer = binditer.rest()

    return e.Application(v.Lambda(binding, desugar(sexpr.rest().first())), *call)

def desugar_let_star(sexpr):
    body = sexpr.rest().rest()
    bind1 = sexpr.rest().first()
    newbind = s.Cons(bind1.first(), s.Nil())

    if bind1.length() == 1:
        return desugar(s.Cons(s.Atom("let"), s.Cons(bind1, body)))
    else:
        bind2 = bind1.rest()
        return desugar(s.Cons(s.Atom("let"), s.Cons(newbind, s.Cons( s.Cons(
            s.Atom("let*"), s.Cons(bind2, body)), s.Nil() ))))

def desugar_pair(sexpr):
    return v.Pair(desugar(sexpr.rest().first()), desugar(sexpr.rest().rest().first()))

def desugar_inx(sexpr):
    if sexpr.first().repr() == "inl":
        first = True
    elif sexpr.first().repr() == "inr":
        first = False 
    else:
        salt()

    return v.Pair(v.Bool(first), desugar(sexpr.rest().first()))

def desugar_case(sexpr):
    sexpr = sexpr.rest()
    switch = desugar(sexpr.first())

    sexpr = sexpr.rest()
    funl =  desugar(sexpr.first())
    funr =  desugar(sexpr.rest().first())
    return e.If(e.Application(v.Primitive("fst"), switch),
            e.Application(funl, e.Application(v.Primitive("snd"), switch)),
                e.Application(funr, e.Application(v.Primitive("snd"), switch)))

def desugar_abort(sexpr):
    sexpr = sexpr.rest()
    return e.Abort(desugar(sexpr.first()))

antirecipies = { \
        "+":        desugar_arith,
        "-":        desugar_arith,
        "*":        desugar_arith,
        "/":        desugar_arith,
        "if":       desugar_if,
        "lambda":   desugar_lambda,
        "$":        desugar_lambda, # Lambdas are money
        "let":      desugar_let,
        "let*":     desugar_let_star,
        "pair":     desugar_pair,
        "inl":      desugar_inx,
        "inr":      desugar_inx,
        "case":     desugar_case,
        "abort":    desugar_abort,
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
    elif sexpr.repr() == "unit":
        return v.Unit()
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
