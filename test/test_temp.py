from .context import j0
import j0.sexpr as s

def test_empty():
    x = s.Nil()
    x.pp()

def test_number_value():
    x = s.Cons(s.Atom("4"), s.Nil())
    x.pp()
    x.desu()
    # y = x.desu()
    # y.pp()
    # z = y.ibig()
    # assert z.number == 4

def test_add_nocrash():
    x = s.Cons(s.Atom("+"), s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"), s.Nil())))
    x.pp()
    x.desu()
    # y = x.desu()
    # y.pp()
    # z = y.ibig()
    # assert z.number == 11
