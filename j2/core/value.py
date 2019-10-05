from j2.core.error import *

class Value:
    def __init__(self, *args):
        pass

    def pp(self):
        print(self.repr())

    def isvalue(self):
        return True

    def subst(self, ident, value):
        return self

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

Prims = [ "+", "*", "/", "-", "<=", "<", "=", ">", ">=" ]

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

    def subst(self, ident, value):
        if ident.value == self.value:
            return value
        else:
            return self
