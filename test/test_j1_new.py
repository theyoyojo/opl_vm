from .context import j1
import j1.value as v
import j1.expression as e
import j1.sexpr as s
import j1.context as c
from j1.desugar import desugar
from j1.big_interp import big_interp
import pytest


# Desugar -> print -> interpret -> print -> assert value equals x
def dp_ip_aeq(sexpr, value):
    sexpr.pp()
    program = desugar(sexpr)
    program.pp()
    program_value = big_interp(program)
    program_value.pp()
    assert program_value.value == value

def test_basic():
    x = v.Number(4)
    x.pp()


def test_add():
    x = e.Application(v.Primitive("+"),v.Number("4"), v.Number("7"))

    y = big_interp(x)
    y.pp()

    assert y.value == 11
    # Identity test
    z = big_interp(y)
    z.pp()
    assert z.value == 11


def test_layered_math():
    x = e.Application(v.Primitive("+"),v.Number("4"), v.Number("7"))

    y = e.Application(v.Primitive("*"),v.Number("2"), x)


    z = big_interp(y)
    z.pp()

    assert z.value == 22

def test_sexpr_add():
    math = s.Cons(s.Atom("+"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))

    x = desugar(math)
    x.pp()
    y = big_interp(x)
    y.pp()
    assert y.value == 11

def test_sexpr_sub():
    math = s.Cons(s.Atom("-"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))

    x = desugar(math)
    x.pp()
    y = big_interp(x)
    y.pp()
    assert y.value == -5

def test_sexpr_mult():
    math = s.Cons(s.Atom("*"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))

    x = desugar(math)
    x.pp()
    y = big_interp(x)
    y.pp()
    assert y.value == 24

def test_if_cond_then_else_true():
    pred = s.Cons(s.Atom("<"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil())))

    x = desugar(pred)
    x.pp()
    y = big_interp(x)

def test_if_cond_then_else_true():
    pred = s.Cons(s.Atom("<"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil())))

    true = s.Cons(s.Atom("*"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))
        
    # hmm
    false = s.Cons(s.Atom("2"), s.Nil())

    x = s.Cons(s.Atom("if"),
               s.Cons(pred,
                      s.Cons(true,false)))

    y = desugar(x)
    y.pp()
    z = big_interp(y)
    z.pp()

def test_division():
    math = s.Cons(s.Atom("/"),s.Cons(s.Atom("3"),s.Cons(s.Atom("8"), s.Nil())))

    dp_ip_aeq(math, 0.375)

