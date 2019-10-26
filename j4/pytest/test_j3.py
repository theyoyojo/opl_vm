import context
import j3.core.value as v
import j3.core.expression as e
from j3.core.sexpr import Cons, Atom, Nil
import j3.core.desugar as d
from j3.interp.interp import big_interp, Env

def test_test():
    x = v.Number(4) 
    binding = []

    binding.append(v.ID("a"))
    binding.append(v.ID("b"))
    expr = e.Application(v.Primitive("+"), v.ID("a"), v.ID("b"))

    lam = v.Lambda(binding, expr)
    lam.pp()

def test_desugar_lambda():
    main = Cons(Cons(Atom("lambda"), Cons(Cons(Atom("x"), Nil()), Cons(Atom("4"), Nil()))), Cons(Atom("2"), Nil()))


    x = d.desugar(main)
    x.pp()

    z = big_interp(x, Env())

    z.pp()
