from j1.error import *

# perhaps turn validate_operands function into work done in
# Expression.__init__() via argspec key/[] pair in attrs

class Expression:
    def __init__(self, *args):
        # First, validate argument count
        if "args_exppected" in self.attrs:
            if len(args) != self.attrs["args_expected"]:
                raise  BadArgumentsCount(self.attrs["args_expected"], len(args))
        # Now, we can safely validate the content
        self.validate_operands(*args)

    def pp(self):
        print(self.repr())

class Application(Expression):
    attrs = {}

    def validate_operands(self, args):
        for arg in args:
            if not issubclass(type(arg), Expression):
                raise BadArgumentsContent("Expression", type(arg).__name__)

    def repr(self):
        return "(" + " ".join(exp.repr() for exp in self.expressions) + ")"

    def __init__(self, *args):
        self.expressions = []
        for e in args:
            self.expressions.append(e)

class If(Expression):
    attrs = {
            "args_exprected": 3,
            }
    expressions = []

    def validate_operands(self, args):
        for arg in args:
            if not issubclass(type(arg), Expression):
                raise BadArgumentsContent("Expression", type(arg).__name__)

    def repr(self):
        return "(if " + self.pred.repr() + " then " + self.true.repr() + \
                " else " + self.false.repr() + ")"

    def __init__(self, *args):
        super().__init__(args)
        self.pred = args[0]
        self.true = args[1]
        self.false = args[2]
        
