from .context import j0
import j0.sexpr as s
import pytest

# Desugar -> print -> interpret -> print -> assert value equals x
def dp_ip_aeq(sexpr,value):
    sexpr.pp()
    sexpr.desu()
    program = sexpr.desu()
    program.pp()
    program_value = program.ibig()
    assert program_value.number == value

def test_empty():
    x = s.Nil()
    x.pp()
    with pytest.raises(s.SaltySyntax):
        x.desu()

def test_number_value():
    x = s.Cons(s.Atom("4"), s.Nil())
    dp_ip_aeq(x, 4)

def test_nil():
    pass

def test_add_nocrash():
    x = s.Cons(s.Atom("+"), s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"), s.Nil())))
    dp_ip_aeq(x, 11)

def test_sub_binary():
    x = s.Cons(s.Atom("-"), s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"), s.Nil())))
    dp_ip_aeq(x, -3)

def test_sub_unary():
    x = s.Cons(s.Atom("-"), s.Cons(s.Atom("4"), s.Nil()))
    dp_ip_aeq(x, -4)

def test_mult():
    x = s.Cons(s.Atom("*"), s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"), s.Nil())))
    dp_ip_aeq(x, 28)

def test_mult_nocrash():
    # x = e.Mult(v.Number(4),v.Number(7))
    x = s.Cons(s.Atom("*"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil())))
    dp_ip_aeq(x, 28)

def test_add_mult_number_left_sag():
    x = s.Cons(s.Atom("+"),s.Cons(s.Cons(
        s.Atom("*"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("5"),s.Nil())))
    dp_ip_aeq(x, 33)

def test_add_mult_number_right_sag():
    x = s.Cons(s.Atom("+"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("*"),s.Cons(s.Atom("7"),
        s.Cons(s.Atom("5"),s.Nil())))))
    dp_ip_aeq(x, 39)

def test_mult_add_number_left_sag():
    x = s.Cons(s.Atom("*"),s.Cons(s.Cons(
        s.Atom("+"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("5"),s.Nil())))
    dp_ip_aeq(x, 55)

def test_mult_add_number_right_sag():
    x = s.Cons(s.Atom("*"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("+"),s.Cons(s.Atom("7"),
            s.Cons(s.Atom("5"),s.Nil())))))
    dp_ip_aeq(x, 48)

def test_add_add_add_balanced():
    x = s.Cons(s.Atom("+"),s.Cons(s.Cons(
        s.Atom("+"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("+"),s.Cons(s.Atom("5"),
            s.Cons(s.Atom("2"),s.Nil())))))
    dp_ip_aeq(x, 18)

def test_mult_mult_mult_balanced():
    x = s.Cons(s.Atom("*"),s.Cons(s.Cons(
        s.Atom("*"),s.Cons(s.Atom("4"),
        s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("*"),s.Cons(s.Atom("5"),
        s.Cons(s.Atom("2"),s.Nil())))))
    dp_ip_aeq(x, 280)

def test_add_mult_add_left_sag():
    x = s.Cons(s.Atom("+"),s.Cons(s.Cons(s.Atom("*"),s.Cons(
        s.Cons(s.Atom("+"),s.Cons(s.Atom("4"),s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("5"),s.Nil()))),s.Cons(s.Atom("2"),s.Nil())))
    dp_ip_aeq(x, 57)

def test_mult_add_mult_left_sag():
    x = s.Cons(s.Atom("*"),s.Cons(s.Cons(s.Atom("+"),s.Cons(
        s.Cons(s.Atom("*"),s.Cons(s.Atom("4"),s.Cons(s.Atom("7"),s.Nil()))),
        s.Cons(s.Atom("5"),s.Nil()))),s.Cons(s.Atom("2"),s.Nil())))
    dp_ip_aeq(x, 66)

def test_add_add_add_add_left_sag():
    x = s.Cons(s.Atom("+"),s.Cons(s.Cons(s.Atom("+"),s.Cons(s.Cons(s.Atom("+"),s.Cons(
            s.Cons(s.Atom("+"),s.Cons(s.Atom("4"),s.Cons(s.Atom("7"),s.Nil()))),
            s.Cons(s.Atom("5"),s.Nil()))),s.Cons(s.Atom("2"),s.Nil()))),
            s.Cons(s.Atom("6"),s.Nil())))
    dp_ip_aeq(x, 24)

def test_subtraction():
    x = s.Cons(s.Atom("-"), s.Cons(s.Atom("4"), s.Cons(s.Atom("7"), s.Nil())))
    dp_ip_aeq(x, -3)


def test_add_unary():
    x = s.Cons(s.Atom("+"), s.Cons(s.Atom("4"), s.Nil()))
    dp_ip_aeq(x, 4)

def test_add_add_add_sugar():
    x = s.Cons(s.Atom("+"), s.Cons(s.Atom("4"), s.Cons(s.Atom("7"),
        s.Cons(s.Atom("5"), s.Nil()))))
    dp_ip_aeq(x, 16)

