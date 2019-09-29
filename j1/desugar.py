import j1.expression as e
import j1.value as v
import j1.sexpr as s


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

antirecipies = { \
        "-": desugar_minus,
        "+": desugar_plus,
        "*": desugar_mult,
        "/": desugar_div,
        "<": desugar_generic_app,
        "<=": desugar_generic_app,
        ">": desugar_generic_app,
        ">=": desugar_generic_app,
        "=": desugar_generic_app,
        "if": desugar_if
        }


def desugar_atom(sexpr):
    if is_number(sexpr.repr()):
        return v.Number(float(sexpr.repr()))
    elif sexpr.repr() in v.Prims:
        return v.Primitive(sexpr.repr())
    else:
        return e.Application(sexpr.repr())


def desugar_cons(sexpr):
    action = sexpr.first().repr()
    if action in antirecipies:
        return antirecipies[action](sexpr)
    elif sexpr.length() == 1:
        return desugar(sexpr.first())
    else:
        return e.Application(sexpr.repr())

def desugar(sexpr):
    if isinstance(sexpr, s.Cons):
        return desugar_cons(sexpr)
    elif isinstance(sexpr, s.Atom):
        return desugar_atom(sexpr)
    elif isinstance(sexpr, s.Nil):
        return e.Application(v.Number(0))
    else:
        salt()

