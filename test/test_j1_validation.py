from .context import j1
import j1.value as v
import j1.expression as e
import pytest

def test_print_number():
    x = v.Number(4)
    x.pp()

def test_print_number():
    x = v.Number(4)
    x.pp()

def test_print_bool():
    x = v.Bool(False)
    x.pp()

def test_cond():
    x = e.Cond(v.Bool(False),v.Number(4),v.Number(7))
    x.pp()
    y = x.binterp()
    y.pp()

def test_subclass():
    x = e.Mult(v.Number(1),v.Number(2))
    assert issubclass(type(x),e.Expression)
    
def test_number_invalid():
    with pytest.raises(v.BadArgumentsCount):
        v.Number(1,2)
    with pytest.raises(v.BadArgumentsCount):
        v.Number()
    with pytest.raises(v.BadArgumentsContent):
        v.Number("fizzbuzz") 

def test_prim_invalid():
    with pytest.raises(v.BadArgumentsCount):
        v.Primitive("<", ">")
    with pytest.raises(v.BadArgumentsCount):
        v.Primitive()
    with pytest.raises(v.BadArgumentsContent):
        v.Primitive("Q") 

def test_bool_invalid():
    with pytest.raises(v.BadArgumentsCount):
        v.Bool("<", ">")
    with pytest.raises(v.BadArgumentsCount):
        v.Bool()

def test_cond_invalid():
    with pytest.raises(v.BadArgumentsCount):
        e.Cond("<", ">")
    with pytest.raises(v.BadArgumentsCount):
        e.Cond(1,2,3,4)

def test_add_invalid():
    with pytest.raises(v.BadArgumentsCount):
        e.Add(">")
    with pytest.raises(v.BadArgumentsCount):
        e.Add(2,3,4)
    with pytest.raises(v.BadArgumentsContent):
        e.Add("a",1)
    with pytest.raises(v.BadArgumentsContent):
        e.Add("a","b")
    with pytest.raises(v.BadArgumentsContent):
        e.Add(1,"b")

def test_mult_invalid():
    with pytest.raises(v.BadArgumentsCount):
        e.Mult(">")
    with pytest.raises(v.BadArgumentsCount):
        e.Mult(2,3,4)
    with pytest.raises(v.BadArgumentsContent):
        e.Mult("a",1)
    with pytest.raises(v.BadArgumentsContent):
        e.Mult("a","b")
    with pytest.raises(v.BadArgumentsContent):
        e.Mult(1,"b")
        
