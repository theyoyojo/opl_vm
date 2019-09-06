from .context import j0
import j0.sexpr as s

def test_empty():
    x = s.Nil()
    x.pp()

