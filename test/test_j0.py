from .context import j0
import j0.value as v
import j0.expression as e

def test_number_value():
    x = v.Number(4)
    assert x.number== 4

def test_add_nocrash():
    x = e.Add(v.Number(4),v.Number(7))
    x.pretty_print()

def test_mult_nocrash():
    x = e.Mult(v.Number(4),v.Number(7))
    x.pretty_print()
