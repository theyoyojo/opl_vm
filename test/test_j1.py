from .context import j1
import j1.value as v
import j1.expression as e

def test_print_number():
    x = v.Number(4)
    x.pp()

def test_print_bool():
    x = v.Bool(False)
    x.pp()

def test_cond():
    x = e.Cond(v.Bool(True),v.Number(4),v.Number(7))
    x.pp()
