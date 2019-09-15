import j1.value as v
from j1.error import *

class Expression:
    def __init__(self, args):
        # First, validate argument count
        if len(args) != self.attrs["args_expected"]:
            raise  BadArgumentsCount(self.attrs["args_expected"], len(args))
        # Now, we can safely validate the content
        self.validate_operands(args)

    def pp(self):
        print(self.repr())

class BinaryExpression(Expression):
    attrs = {
            "args_expected": 2
            }
    def __init__(self, args):
        super().__init__(args)
        self.left = args[0]
        self.right = args[1]

    def validate_operands(self, args):
        if not isinstance(args[0], v.Number) and not issubclass(type(args[0]), Expression):
            raise BadArgumentsContent("Number",args[0])
        if not isinstance(args[1], v.Number) and not issubclass(type(args[1]), Expression):
            raise BadArgumentsContent("Number",args[1])

class Mult(BinaryExpression):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(* " + self.left.repr() + " " + self.right.repr() + ")"

    def binterp(self):
        return v.Number(self.left.binterp().value * self.right.binterp().value)

class Add(BinaryExpression):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(+ " + self.left.repr() + " " + self.right.repr() + ")"

    def binterp(self):
        return v.Number(self.left.binterp().value + self.right.binterp().value)

class Div(BinaryExpression):
    def __init__(self, *args):
        super().__init__(args)
        if self.right == 0:
            raise ValueError("Undefined behavior: divison by zero")

    def repr(self):
        return "(+ " + self.left.repr() + " " + self.right.repr() + ")"

    def binterp(self):
        return v.Number(self.left.binterp().value / self.right.binterp().value)

class LessThan(BinaryExpression):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(< " + self.left.repr() + " " + self.right.repr() + ")"

    def binterp(self):
        return v.Bool(self.left.binterp().value < self.right.binterp().value)

class LessThanOrEqualTo(BinaryExpression):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(<= " + self.left.repr() + " " + self.right.repr() + ")"

    def binterp(self):
        return v.Bool(self.left.binterp().value <= self.right.binterp().value)

class GreaterThan(BinaryExpression):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(> " + self.left.repr() + " " + self.right.repr() + ")"

    def binterp(self):
        return v.Bool(self.left.binterp().value > self.right.binterp().value)

class GreaterThanOrEqualTo(BinaryExpression):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(>= " + self.left.repr() + " " + self.right.repr() + ")"

    def binterp(self):
        return v.Bool(self.left.binterp().value >= self.right.binterp().value)

class EqualTo(BinaryExpression):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(= " + self.left.repr() + " " + self.right.repr() + ")"

    def binterp(self):
        return v.Bool(self.left.binterp().value == self.right.binterp().value)


class Cond(Expression):
    attrs = {
            "args_expected": 3
            }

    def validate_operands(self, args):
        pass

    def __init__(self, *args):
        super().__init__(args)
        self.pred = args[0]
        self.true = args[1]
        self.false = args[2]

    def repr(self):
        return "(if " + self.pred.repr() + " then " + self.true.repr() + \
                " else " + self.false.repr() + ")"

    def binterp(self):
        if self.pred.binterp().value:
            return self.true.binterp()
        else:
            return self.false.binterp()
