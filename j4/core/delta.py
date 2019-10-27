import j4.core.value as v
from j4.core.error import *

class Delta:
    def __init__(self, args):
        # First, validate argument count
        if len(args) != self.attrs["args_expected"]:
            raise  BadArgumentsCount(self.attrs["args_expected"], len(args))
        # Now, we can safely validate the content
        self.validate_operands(args)

    def pp(self):
        print(self.repr())

class BinaryDelta(Delta):
    attrs = {
            "args_expected": 2
            }
    def __init__(self, args):
        super().__init__(args)
        self.left = args[0]
        self.right = args[1]

    def validate_operands(self, args):
        if not isinstance(args[0], v.Value):
            raise BadArgumentsContent("Number",args[0])
        if not isinstance(args[1], v.Value):
            raise BadArgumentsContent("Number",args[1])

class Mult(BinaryDelta):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(* " + self.left.repr() + " " + self.right.repr() + ")"

    def __call__(self):
        return v.Number(self.left.value * self.right.value)

class Add(BinaryDelta):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(+ " + self.left.repr() + " " + self.right.repr() + ")"

    def __call__(self):
        return v.Number(self.left.value + self.right.value)

class Div(BinaryDelta):
    def __init__(self, *args):
        super().__init__(args)
        if self.right == 0:
            raise ValueError("Undefined behavior: divison by zero")

    def repr(self):
        return "(+ " + self.left.repr() + " " + self.right.repr() + ")"

    def __call__(self):
        return v.Number(self.left.value / self.right.value)

class LessThan(BinaryDelta):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(< " + self.left.repr() + " " + self.right.repr() + ")"

    def __call__(self):
        return v.Bool(self.left.value < self.right.value)

class LessThanOrEqualTo(BinaryDelta):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(<= " + self.left.repr() + " " + self.right.repr() + ")"

    def __call__(self):
        return v.Bool(self.left.value <= self.right.value)

class GreaterThan(BinaryDelta):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(> " + self.left.repr() + " " + self.right.repr() + ")"

    def __call__(self):
        return v.Bool(self.left.value > self.right.value)

class GreaterThanOrEqualTo(BinaryDelta):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(>= " + self.left.repr() + " " + self.right.repr() + ")"

    def __call__(self):
        return v.Bool(self.left.value >= self.right.value)

class EqualTo(BinaryDelta):
    def __init__(self, *args):
        super().__init__(args)

    def repr(self):
        return "(= " + self.left.repr() + " " + self.right.repr() + ")"

    def __call__(self):
        return v.Bool(self.left.value == self.right.value)

class UndefinedFunctionException(Exception):
    pass

class Undefined(Delta):
    def __init__(self, *args):
        # Should I raise an exception here?
        print("Undefined function!\n" \
                "args: " + str(args))
        self.args = args

    def repr(self):
        return "[!]Undefined (function " + str(self.args) + ")[!]"

    def __call__(self):
        raise UndefinedFunctionException("Impossible to interpret undefined function")

primitive_delta_map = {
        "+": Add,
        "*": Mult,
        "/": Div,
        "<": LessThan,
        "<=": LessThanOrEqualTo,
        ">": GreaterThan,
        ">=": GreaterThanOrEqualTo,
        "=": EqualTo
}

def generate_delta(value_list):
    args = tuple(value_list[1:])
    if len(value_list) < 1:
        raise BadArgumentsCount("more than 0", "something else")
    elif value_list[0].value in primitive_delta_map:
        return primitive_delta_map[value_list[0].value](*args)
    else:
        return Undefined(*args)
