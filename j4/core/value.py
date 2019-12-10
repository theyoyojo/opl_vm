from j4.core.error import *

class Value:
    def __init__(self, *args):
        pass

    def pp(self):
        print(self.repr())

    def isvalue(self):
        return True

class Number(Value):
    attrs = {
            "args_expected": 1
            }

    def validate_operands(self, args):
        try:
            float(args[0])
        except ValueError:
            raise BadArgumentsContent(type(self).__name__ , args[0])
        
    def __init__(self, *args):
        super().__init__(args)
        self.value = float(args[0])

    def repr(self):
        return str(self.value)

BoolReps = \
{
        True:   "<TRUE>",
        False:  "<FALSE>"
}
class Bool(Value):
    attrs = {
            "args_expected": 1
            }

    def validate_operands(self, args):
        # anything can be converted to a bool for now
        pass

    def __init__(self, *args):
        super().__init__(args)
        self.value = bool(args[0])

    def repr(self):
        return BoolReps[self.value]

Prims = [
       "+",
       "*",
       "/",
       "-",
       "<=",
       "<",
       "=",
       ">",
       ">=",
       "pair",
       "fst",
       "snd",
       "box",
       "unbox",
       "set-box!",
       "print",
       ]

class Primitive(Value):
    attrs = {
            "args_expected": 1
            }

    def validate_operands(self, args):
        if args[0] not in Prims:
            raise BadArgumentsContent(type(self).__name__, args[0])

    def __init__(self, *args):
        super().__init__(args)
        self.value = args[0]

    def repr(self):
        return self.value

class ID(Value):
    attrs = {
            "args_expected": 1
            }
    def __init__(self, *args):
        super().__init__(args)
        self.value = args[0]

    def repr(self):
        return "ID:" + self.value

class Lambda(Value):
    def __init__(self, binding, expr, rec=ID("rec")):
        self.rec = rec
        self.binding = binding
        self.expr = expr

    def repr(self):
        return "(lambda/" + self.rec.value + "(" + ' '.join(ident.repr() for ident in self.binding[:]) + ")[" + self.expr.repr() + "])"


class Pair(Value):
    def __init__(self, first, second):
        self.first = first
        self.second = second

    def repr(self):
        return "<{}, {}>".format(self.first.repr(), self.second.repr())

class Unit(Value):
    def __init__(self):
        pass

    def repr(self):
        return "{UNIT}"

class String(Value):
    def __init__(self, string):
        self.value = string
    
    def repr(self):
        return self.value

