from .context import j1
import j1.value as v
import j1.expression as e
import j1.sexpr as s
from j1.desugar import desugar
from j1.big_interp import big_interp
from j1.error import *
import pytest

# Desugar -> print -> interpret -> print -> assert value equals x
def dp_ip_aeq(sexpr, value):
    sexpr.pp()
    program = desugar(sexpr)
    program.pp()
    program_value = big_interp(program)
    program_value.pp()
    assert program_value.value == value

def test_less_than_true():
    x = s.Cons(s.Atom("<"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil())))
    dp_ip_aeq(x,True)

def test_less_than_whack():
    with pytest.raises(BadArgumentsCount):
        x = s.Cons(s.Atom("<"),s.Cons(s.Atom("4"),
            s.Cons(s.Atom("7"),s.Cons(s.Atom("2"),s.Nil()))))
        big_interp(desugar(x))

def test_minus_ternary():
    # Basically, this error is triggered by the remaining sexpr
    # being passed to add, but maybe it should be detected elsewhere?
    with pytest.raises(j1.big_interp.QuestionablyInterpretableExpressionException):
        x = s.Cons(s.Atom("-"),s.Cons(s.Atom("4"),
            s.Cons(s.Atom("7"),s.Cons(s.Atom("2"),s.Nil()))))
        big_interp(desugar(x))

def test_if_cond_then_else_true():
    pred = s.Cons(s.Atom("<"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil())))

    true = s.Cons(s.Atom("*"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))
        
    # hmm
    false = s.Cons(s.Atom("2"), s.Nil())

    x = s.Cons(s.Atom("if"),
               s.Cons(pred,
                      s.Cons(true,false)))

    dp_ip_aeq(x,24)


def test_if_cond_then_else_false():
    pred = s.Cons(s.Atom(">"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil())))

    true = s.Cons(s.Atom("*"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))

    # hmm
    false = s.Cons(s.Atom("2"), s.Nil())

    x = s.Cons(s.Atom("if"),
               s.Cons(pred,
                      s.Cons(true,false)))

    dp_ip_aeq(x,2)

def test_if_cond_equal_false():
    pred = s.Cons(s.Atom("="),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil())))

    true = s.Cons(s.Atom("*"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))

    # hmm
    false = s.Cons(s.Atom("4"), s.Nil())

    x = s.Cons(s.Atom("if"),
               s.Cons(pred,
                      s.Cons(true,false)))

    dp_ip_aeq(x,4)

def test_cond_equal_true():
    pred = s.Cons(s.Atom("="),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("4"),s.Nil())))
    dp_ip_aeq(pred,True)

def test_compound_math():
    math = s.Cons(s.Atom("*"), s.Cons(s.Atom("2"),s.Cons(
        s.Cons(s.Atom("+"),s.Cons(s.Atom("3"),s.Cons(s.Atom("5"),s.Nil()))),s.Nil() )))

    x = desugar(math)

    dp_ip_aeq(math, 16)

def test_number_interp():

    n = s.Cons(s.Atom("7"), s.Nil())

    dp_ip_aeq(n, 7)

def test_minus_binary():

    math = s.Cons(s.Atom("-"),s.Cons(s.Atom("3"),s.Cons(s.Atom("8"), s.Nil())))
    dp_ip_aeq(math, -5)

def test_if_cond_equal_false():
    math = s.Cons(s.Atom("*"), s.Cons(s.Atom("2"),s.Cons(
        s.Cons(s.Atom("+"),s.Cons(s.Atom("3"),s.Cons(s.Atom("5"),s.Nil()))),s.Nil() )))

    pred = s.Cons(s.Atom(">="),s.Cons(math,
        s.Cons(s.Cons(s.Atom("7"),s.Nil()), s.Nil())))

    true = s.Cons(s.Atom("-"),s.Cons(s.Atom("3"), s.Cons(s.Atom("8"),s.Nil())))
    math = s.Cons(s.Atom("-"),s.Cons(s.Atom("3"),s.Cons(s.Atom("8"), s.Nil())))

    dp_ip_aeq(true, -5)

    # hmm
    false = s.Cons(s.Atom("4"), s.Nil())

    x = s.Cons(s.Atom("if"),
               s.Cons(pred,
                      s.Cons(true,false)))

    dp_ip_aeq(x,-5)


def test_minus_unary():
    exp = s.Cons(s.Atom("-"),s.Cons(s.Atom("4"),s.Nil()))
    dp_ip_aeq(exp, -4)


def test_less_or_equal():
    x = s.Cons(s.Atom("<="),s.Cons(s.Atom("7"),
        s.Cons(s.Cons(s.Atom("7"),s.Nil()), s.Nil())))

    dp_ip_aeq(x,True)

def test_division():
    math = s.Cons(s.Atom("/"),s.Cons(s.Atom("3"),s.Cons(s.Atom("8"), s.Nil())))

    dp_ip_aeq(math, 0.375)
