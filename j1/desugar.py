import j1.expression as e
import j1.value as v
import j1.undefined as u
import j1.sexpr as s

class NotSugar(Exception):
    pass

def salt():
        raise NotSugar("Something is salty with this sexpr...")

def is_number(string):
    try:
        float(string)
        return True
    except ValueError:
        return False


def desugar_minus(sexpr):
    # Unary case
    if sexpr.length() == 1:
        return e.Mult(v.Number(-1), desugar(sexpr.first()))
    # Binary case
    elif sexpr.length() == 2:
        return e.Add(e.Mult(v.Number(-1), desugar(sexpr.second())), desugar(sexpr.first()))
    elif sexpr.length() > 2:
        return e.Add(e.Mult(v.Number(-1), desugar(sexpr.rest())), desugar(sexpr.first()))
    else:
        salt()

def desugar_plus(sexpr):
    if sexpr.length() == 1:
        return e.Add(desugar(sexpr.first()))
    # Base case
    if sexpr.length() == 2:
        return e.Add(desugar(sexpr.first()), desugar(sexpr.second()))
    # Recursive case
    elif sexpr.length() > 2:
        return e.Add(desugar(sexpr.first()), s.Cons(s.Atom("+"), desugar(sexpr.rest())))
    else:
        salt()
        

def desugar_mult(sexpr):
    if sexpr.length() == 1:
        return e.Mult(desugar(sexpr.first()))
    # Base case
    if sexpr.length() == 2:
        return e.Mult(desugar(sexpr.first()), desugar(sexpr.second()))
    # Recursive case
    elif sexpr.length() > 2:
        return e.Mult(desugar(sexpr.first()), s.Cons(s.Atom("*"), desugar(sexpr.rest())))
    # Error case (basement case)
    else:
        salt()

def desugar_div(sexpr):
    if sexpr.length() == 1:
        return e.Div(desugar(sexpr.first()))
    # Base case
    if sexpr.length() == 2:
        return e.Div(desugar(sexpr.first()), desugar(sexpr.second()))
    # Recursive case
    elif sexpr.length() > 2:
        return e.Div(desugar(sexpr.first()), s.Cons(s.Atom("/"), desugar(sexpr.rest())))
    # Error case (basement case)
    else:
        salt()

def desugar_less_than(sexpr):
    if sexpr.length() == 1:
        return e.LessThan(desugar(sexpr.first()))
    if sexpr.length() == 2:
        return e.LessThan(desugar(sexpr.first()), desugar(sexpr.second()))
    elif sexpr.length() > 2:
        return e.LessThan(desugar(sexpr.first()), desugar(sexpr.rest()))
    else:
        salt()

def desugar_less_than_or_equal_to(sexpr):
    if sexpr.length() == 1:
        return e.LessThanOrEqualTo(desugar(sexpr.first()))
    if sexpr.length() == 2:
        return e.LessThanOrEqualTo(desugar(sexpr.first()), desugar(sexpr.second()))
    elif sexpr.length() > 2:
        return e.LessThanOrEqualTo(desugar(sexpr.first()), desugar(sexpr.rest()))
    else:
        salt()

def desugar_greater_than(sexpr):
    if sexpr.length() == 1:
        return e.GreaterThan(desugar(sexpr.first()))
    if sexpr.length() == 2:
        return e.GreaterThan(desugar(sexpr.first()), desugar(sexpr.second()))
    elif sexpr.length() > 2:
        return e.GreaterThan(desugar(sexpr.first()), desugar(sexpr.rest()))
    else:
        salt()

def desugar_greater_than(sexpr):
    if sexpr.length() == 1:
        return e.GreaterThan(desugar(sexpr.first()))
    if sexpr.length() == 2:
        return e.GreaterThan(desugar(sexpr.first()), desugar(sexpr.second()))
    elif sexpr.length() > 2:
        return e.GreaterThan(desugar(sexpr.first()), desugar(sexpr.rest()))
    else:
        salt()

def desugar_greater_than_or_equal_to(sexpr):
    if sexpr.length() == 1:
        return e.GreaterThanOrEqualTo(desugar(sexpr.first()))
    if sexpr.length() == 2:
        return e.GreaterThanOrEqualTo(desugar(sexpr.first()), desugar(sexpr.second()))
    elif sexpr.length() > 2:
        return e.GreaterThanOrEqualTo(desugar(sexpr.first()), desugar(sexpr.rest()))
    else:
        salt()

def desugar_equal_to(sexpr):
    if sexpr.length() == 1:
        return e.EqualTo(desugar(sexpr.first()))
    if sexpr.length() == 2:
        return e.EqualTo(desugar(sexpr.first()), desugar(sexpr.second()))
    elif sexpr.length() > 2:
        return e.EqualTo(desugar(sexpr.first()), desugar(sexpr.rest()))
    else:
        salt()

def desugar_if(sexpr):
    if sexpr.length() == 1:
        return e.Cond(desugar(sexpr.first()))
    elif sexpr.length() == 2:
        return e.Cond(desugar(sexpr.first()), desugar(sexpr.second()))
    elif sexpr.length() == 3:
        return e.Cond(desugar(sexpr.first()), desugar(sexpr.second()), desugar(sexpr.rest().second()))
    elif sexpr.length() > 3:
        # Well yeah this produces garbage by design but this is extra garbage
        return e.Cond(desugar(sexpr.first()), desugar(sexpr.second()), desugar(sexpr.right.rest()))
    else:
        salt()

antirecipies = { \
        "-": desugar_minus,
        "+": desugar_plus,
        "*": desugar_mult,
        "/": desugar_div,
        "<": desugar_less_than,
        "<=": desugar_less_than_or_equal_to,
        ">": desugar_greater_than,
        ">=": desugar_greater_than_or_equal_to,
        "=": desugar_equal_to,
        "if": desugar_if
        }

def desugar_cons(sexpr):
    action = sexpr.first().repr()
    if action in antirecipies:
        return antirecipies[action](sexpr.rest())
    elif sexpr.length() == 1:
        return desugar(sexpr.first())
    else:
        return u.Undefined(sexpr)

def desugar_atom(sexpr):
    if is_number(sexpr.repr()):
        return v.Number(float(sexpr.repr()))
    else:
        return u.Undefined(sexpr)

def desugar(sexpr):
    if isinstance(sexpr, s.Cons):
        return desugar_cons(sexpr)
    elif isinstance(sexpr, s.Atom):
        return desugar_atom(sexpr)
    elif isinstance(sexpr, s.Nil):
        return u.Undefined(sexpr)
    else:
        salt()
