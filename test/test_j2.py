import j2.core.value as v
import j2.core.expression as e
import j2.interp.interp as i

def test_number():
    x = v.Number(2)

def test_expr():
    x = e.Application(v.Primitive("+"),v.Number(4), v.Number(7))

    y = i.big_interp(x)
    assert y.value == 11
