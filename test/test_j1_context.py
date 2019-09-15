from .context import j1
import j1.value as v
import j1.expression as e
import j1.sexpr as s
import j1.undefined as u
import j1.context as c
from j1.desugar import desugar
import pytest

def test_find_redex():
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
    redex, context = (c.find_redex(y))

    redex.pp()
    interp_redex = redex.binterp()
    context.pp()

    new_context = c.plug(interp_redex, context)
    new_context.pp()

    z = new_context.binterp()
    z.pp()

    # print(c.find_redex(y))
    # print(c.find_redex(y))


def test_small_interp_style_math():

    math = s.Cons(s.Atom("*"), s.Cons(s.Atom("2"),s.Cons(
        s.Cons(s.Atom("+"),s.Cons(s.Atom("3"),s.Cons(s.Atom("5"),s.Nil()))),s.Nil() )))

    y = desugar(math)
    y.pp()
    redex, context = (c.find_redex(y))

    redex.pp()
    interp_redex = redex.binterp()
    context.pp()

    new_context = c.plug(interp_redex, context)
    new_context.pp()

    z = new_context.binterp()
    z.pp()

    # print(c.find_redex(y))
    # print(c.find_redex(y))

def test_plug_hole():
    hole = c.Hole()

    x = v.Number(4)

    program = c.plug(x, hole)

    program.pp()

def test_find_redex_identity():


    x = v.Number(4)

    redex, context = c.find_redex(x)

    redex.pp()
    context.pp()

def test_find_redex_identity_math():
    x = s.Cons(s.Atom("-"),s.Cons(s.Atom("3"),s.Cons(s.Atom("8"), s.Nil())))


    redex, context = c.find_redex(x)

    redex.pp()
    context.pp()

def test_small_interp():
    y = s.Cons(s.Atom("-"),s.Cons(s.Atom("3"),s.Cons(s.Atom("8"), s.Nil())))

    z = desugar(y)

    x = c.sinterp(z)
    x.pp()

    
def test_small_interp_math():
    math = s.Cons(s.Atom("*"), s.Cons(s.Atom("2"),s.Cons(
        s.Cons(s.Atom("+"),s.Cons(s.Atom("3"),s.Cons(s.Atom("5"),s.Nil()))),s.Nil() )))

    z = desugar(math)

    x = c.sinterp(z)
    x.pp()

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

    x = c.sinterp(z)
    x.pp()

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
                      s.Cons(true,false)))

    z = desugar(x)

    print("AAAAAAAAAAAAAAAAAAAAAAAAAAA")
    x = c.sinterp(z)
    x.pp()
