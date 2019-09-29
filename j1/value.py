from j1.error import *
import j1.expression as e
import j1.context as c

class Value:
    def __init__(self, *args):
        pass
        # First, validate argument count
        # if "args_exppected" in self.attrs:
        #     if len(args) != self.attrs["args_expected"]:
        #         raise  BadArgumentsCount(self.attrs["args_expected"], len(args))
        # # Now, we can safely validate the content
        # self.validate_operands(*args)
    def find_redex(self):
        return c.Hole(), self

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
