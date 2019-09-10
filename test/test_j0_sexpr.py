from .context import j0
import j0.sexpr as s

def test_empty():
    x = s.Nil()
    x.pp()

def test_number_value():
    x = s.Cons(s.Atom("4"), s.Nil())
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 4

def test_add_nocrash():
    x = s.Cons(s.Atom("+"), s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"), s.Nil())))
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 11

def test_mult_nocrash():
    # x = e.Mult(v.Number(4),v.Number(7))
    x = s.Cons(s.Atom("*"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil())))
    y = x.desu()
    y.pp()
    z = y.ibig()
    z.pp()
    assert z.number == 28

def test_add_mult_number_left_sag():
    x = s.Cons(s.Atom("+"),s.Cons(s.Cons(
        s.Atom("*"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("5"),s.Nil())))
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 33

def test_add_mult_number_right_sag():
    x = s.Cons(s.Atom("+"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("*"),s.Cons(s.Atom("7"),
        s.Cons(s.Atom("5"),s.Nil())))))
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 39

def test_mult_add_number_left_sag():
    x = s.Cons(s.Atom("*"),s.Cons(s.Cons(
        s.Atom("+"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("5"),s.Nil())))
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 55

def test_mult_add_number_right_sag():
    x = s.Cons(s.Atom("*"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("+"),s.Cons(s.Atom("7"),
            s.Cons(s.Atom("5"),s.Nil())))))
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 48

def test_add_add_add_balanced():
    x = s.Cons(s.Atom("+"),s.Cons(s.Cons(
        s.Atom("+"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("+"),s.Cons(s.Atom("5"),
            s.Cons(s.Atom("2"),s.Nil())))))
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 18

def test_mult_mult_mult_balanced():
    x = s.Cons(s.Atom("*"),s.Cons(s.Cons(
        s.Atom("*"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("*"),s.Cons(s.Atom("5"),
        s.Cons(s.Atom("2"),s.Nil())))))
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 280

def test_add_mult_add_left_sag():
    x = s.Cons(s.Atom("+"),s.Cons(s.Cons(s.Atom("*"),s.Cons(
        s.Cons(s.Atom("+"),s.Cons(s.Atom("4"),s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("5"),s.Nil()))),s.Cons(s.Atom("2"),s.Nil())))
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 57

def test_mult_add_mult_left_sag():
    x = s.Cons(s.Atom("*"),s.Cons(s.Cons(s.Atom("+"),s.Cons(
        s.Cons(s.Atom("*"),s.Cons(s.Atom("4"),s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("5"),s.Nil()))),s.Cons(s.Atom("2"),s.Nil())))
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 66

def test_add_add_add_add_left_sag():
    x = s.Cons(s.Atom("+"),s.Cons(s.Cons(s.Atom("+"),s.Cons(s.Cons(s.Atom("+"),s.Cons(
            s.Cons(s.Atom("+"),s.Cons(s.Atom("4"),s.Cons(s.Atom("7"),s.Nil()))),
            s.Cons(s.Atom("5"),s.Nil()))),s.Cons(s.Atom("2"),s.Nil()))),
            s.Cons(s.Atom("6"),s.Nil())))
    x.pp()
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == 24

def test_subtraction():
    x = s.Cons(s.Atom("-"), s.Cons(s.Atom("4"), s.Cons(s.Atom("7"), s.Nil())))
    y = x.desu()
    y.pp()
    z = y.ibig()
    assert z.number == -3

# def test_subtraction_unary():
#     x = s.Cons(s.Atom("+"),s.Cons(s.Atom("4"),
#             s.Cons(s.Atom("-"), s.Cons(s.Atom("7"),s.Nil()))))
#     y = x.desu()
#     y.pp()
#     z = y.ibig()
#     assert z.number == -3
