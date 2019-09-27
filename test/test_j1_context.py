from .context import j1
import j1.value as v
import j1.expression as e
import j1.sexpr as s
import j1.context as c
from j1.interp import *
from j1.desugar import desugar
import pytest

def test_cc0_value():
    value = s.Cons(s.Atom("5"),s.Nil())
    x = desugar(value)
    y = CC0(x)
    y.pp()
    assert y.value == 5

def test_simple_small():
    math = s.Cons(s.Atom("*"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))
    x = desugar(math)
    x.pp()
    y = small_interp(x)
    y.pp()
    assert y.value == 24

def test_math_small():
    math1 = s.Cons(s.Atom("*"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))

    math2 = s.Cons(s.Atom("+"),s.Cons(math1,s.Cons(s.Atom("1"),s.Nil())))

    x = desugar(math2)
    x.pp()
    y = small_interp(x)
    y.pp()
    assert y.value == 25


def test_small_if_true():
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

    z = small_interp(y)
    z.pp()

    assert z.value == 24

def test_small_if_false():
    pred = s.Cons(s.Atom(">"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil())))

    true = s.Cons(s.Atom("*"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))
        
    # hmm
    false = s.Cons(s.Atom("2"), s.Nil())

    x = s.Cons(s.Atom("if"),
               s.Cons(pred,
                      s.Cons(true,false)))


    y = desugar(x)
    y.pp()

    z = small_interp(y)
    z.pp()

    assert z.value == 2


def test_lowcon():
    pred = s.Cons(s.Atom(">"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil())))

    true = s.Cons(s.Atom("*"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))
        
    false = s.Cons(s.Atom("2"), s.Nil())

    x = s.Cons(s.Atom("if"),
               s.Cons(pred,
                      s.Cons(true,false)))
    prog = desugar(x)

    res = CC0(prog)

    assert res.value == 2


def test_small_interp_math_cond_true():
    math = s.Cons(s.Atom("*"), s.Cons(s.Atom("2"),s.Cons(
        s.Cons(s.Atom("+"),s.Cons(s.Atom("3"),s.Cons(s.Atom("5"),s.Nil()))),s.Nil() )))

    pred = s.Cons(s.Atom(">="),s.Cons(math,
        s.Cons(s.Cons(s.Atom("7"),s.Nil()), s.Nil())))

    true = s.Cons(s.Atom("-"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))
    math = s.Cons(s.Atom("-"),s.Cons(s.Atom("3"),s.Cons(s.Atom("8"), s.Nil())))

    # hmm
    false = s.Cons(s.Atom("4"), s.Nil())

    x = s.Cons(s.Atom("if"),
               s.Cons(pred,
                      s.Cons(true,false)))

    z = desugar(x)

    x = CC0(z)
    x.pp()

    y = small_interp(z)

    a = big_interp(z)

    assert x.value == -5 == y.value == a.value

def test_small_interp_math_cond_false():
    math = s.Cons(s.Atom("*"), s.Cons(s.Atom("2"),s.Cons(
        s.Cons(s.Atom("+"),s.Cons(s.Atom("3"),s.Cons(s.Atom("5"),s.Nil()))),s.Nil() )))

    pred = s.Cons(s.Atom("<"),s.Cons(math,
        s.Cons(s.Cons(s.Atom("7"),s.Nil()), s.Nil())))

    true = s.Cons(s.Atom("-"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))
    math = s.Cons(s.Atom("-"),s.Cons(s.Atom("3"),s.Cons(s.Atom("8"), s.Nil())))

    # hmm
    false = s.Cons(s.Atom("*"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))

    x = s.Cons(s.Atom("if"),
               s.Cons(pred,
                      s.Cons(true,s.Cons(false,s.Nil()))))

    z = desugar(x)

    x = CC0(z)
    print("cc0 res: ", x.value)
    z.pp()
    y = big_interp(z)
    print("big res: ", y.value)

    context, redex = z.find_redex()
    context.pp()
    redex.pp()

    subc, subr = redex.find_redex()
    subc.pp()
    subr.pp()

    a = small_interp(z)
    a.pp()

    assert x.value == y.value == a.value

