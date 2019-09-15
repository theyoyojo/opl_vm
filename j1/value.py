from j1.error import *

class Value():
    def __init__(self, args):
        # First, validate argument count
        if len(args) != self.attrs["args_expected"]:
            raise  BadArgumentsCount(self.attrs["args_expected"], len(args))
        # Now, we can safely validate the content
        self.validate_value(args)

    def pp(self):
        print(self.repr())


class Number(Value):
    attrs = {
            "args_expected": 1
            }

    def validate_value(self, args):
        try:
            float(args[0])
        except ValueError:
            raise BadArgumentsContent(type(self).__name__ , args[0])
        
    def __init__(self, *args):
        super().__init__(args)
        self.value = float(args[0])

    def repr(self):
        return str(self.value)

    def binterp(self):
        return Number(self.value)

BoolReps = \
{
        True:   "<TRUE>",
        False:  "<FALSE>"
}
class Bool(Value):
    attrs = {
            "args_expected": 1
            }

    def validate_value(self, args):
        # anything can be converted to a bool for now
        pass

    def __init__(self, *args):
        super().__init__(args)
        self.value = bool(args[0])

    def repr(self):
        return BoolReps[self.value]

    def binterp(self):
        return Bool(self.value)

Prims = [ "+", "*", "/", "-", "<=", "<" "=", ">", ">=" ]

# This is never actually used...
class Primitive(Value):
    attrs = {
            "args_expected": 1
            }

    def validate_value(self, args):
        if args[0] not in Prims:
            raise BadArgumentsContent(type(self).__name__, args[0])

    def __init__(self, *args):
        super().__init__(args)
        self.value = args[0]

    def repr(self):
        return self.symbol
