class JayValueError(ValueError):
    pass

class Value():
    def pp(self):
        print(self.repr())

class Number(Value):
    def __init__(self, number):
        try:
            float(number)
        except ValueError:
            raise JayValueError("Suspicious number '" + number + "'detected")
        self.number = float(number)

    def repr(self):
        return str(self.number)

    def ibig(self):
        return Number(self.number)

BoolReps = \
{
        True:   "<TRUE>",
        False:  "<FALSE>"
}
class Bool(Value):
    def __init__(self,value):
        self.value = bool(value)

    def repr(self):
        return BoolReps[self.value]

Prims = [ "+", "*", "/", "-", "<=", "<" "=", ">", ">=" ]

class Primitive(Value):
    def __init__(self, symbol):
        if symbol not in Prims:
            raise JayValueError("Invalid primitive '" + symbol + "' given")
        self.symbol = symbol

    def repr(self):
        return symbol
