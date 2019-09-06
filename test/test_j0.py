from .context import j0
import j0.value as v
import j0.expression as e

"""
x represents an arbitrary program
"""

def test_number_value():
    x = v.Number(4)
    x.pp()
    assert x.number== 4

def test_add_nocrash():
    x = e.Add(v.Number(4),v.Number(7))
    x.pp()
    assert x.left.number == 4 and x.right.number == 7

def test_mult_nocrash():
    x = e.Mult(v.Number(4),v.Number(7))
    x.pp()
    assert x.left.number == 4 and x.right.number == 7


def test_add_mult_number_left_sag():
    x = e.Add(e.Mult(v.Number(4), v.Number(7)),v.Number(5))
    x.pp()
    assert x.left.left.number == 4 and x.left.right.number == 7 and x.right.number == 5

def test_add_mult_number_right_sag():
    x = e.Add(v.Number(4), e.Mult(v.Number(7), v.Number(5)))
    x.pp()
    assert x.left.number == 4 and x.right.left.number == 7 and x.right.right.number == 5

def test_mult_add_number_left_sag():
    x = e.Mult(e.Add(v.Number(4), v.Number(7)),v.Number(5))
    x.pp()
    assert x.left.left.number == 4 and x.left.right.number == 7 and x.right.number == 5

def test_mult_add_number_right_sag():
    x = e.Mult(v.Number(4), e.Add(v.Number(7), v.Number(5)))
    x.pp()
    assert x.left.number == 4 and x.right.left.number == 7 and x.right.right.number == 5

def test_add_add_add_balanced():
    x = e.Add( \
            e.Add(v.Number(4), v.Number(7)), \
            e.Add(v.Number(5), v.Number(2)))
    x.pp()
    assert x.left.left.number == 4 and x.left.right.number == 7 \
        and x.right.left.number == 5 and x.right.right.number == 2

def test_mult_mult_mult_balanced():
    x = e.Mult( \
            e.Mult(v.Number(4), v.Number(7)), \
            e.Mult(v.Number(5), v.Number(2)))
    x.pp()
    assert x.left.left.number == 4 and x.left.right.number == 7 \
        and x.right.left.number == 5 and x.right.right.number == 2

def test_add_mult_add_left_sag():
    x = e.Add(e.Mult(e.Add( \
            v.Number(4), v.Number(7)), \
            v.Number(5)), v.Number(2))
    x.pp()
    assert x.left.left.left.number == 4 and x.left.left.right.number == 7 \
        and x.left.right.number == 5 and x.right.number == 2

def test_mult_add_mult_left_sag():
    x = e.Mult(e.Add(e.Mult( \
            v.Number(4), v.Number(7)), \
            v.Number(5)), v.Number(2))
    x.pp()
    assert x.left.left.left.number == 4 and x.left.left.right.number == 7 \
        and x.left.right.number == 5 and x.right.number == 2

def test_add_add_add_add_left_sag():
    x = e.Add(e.Add(e.Add(e.Add( \
        v.Number(4),v.Number(7)), \
        v.Number(5)),v.Number(2)), \
        v.Number(6))

    x.pp()
    assert x.left.left.left.left.number == 4 and x.left.left.left.right.number == 7 \
        and x.left.left.right.number == 5 and x.left.right.number == 2 and x.right.number == 6
