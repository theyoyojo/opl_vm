from j1.error import *
import j1.context as c
import j1.value as v

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

    def find_redex(self):
        # print()
        # print(type(program),program)
        for i in range(len(self.expressions)):
            # print(attr, value)
            if issubclass(type(self.expressions[i]),Expression) and \
                not issubclass(type(self.expressions[i]),v.Value):
                redex = self.expressions[i]
                context = self.make_context(i)
                return context, redex
            # If everything else is a value, the last thing must redex'd
            elif i == len(self.expressions) - 1:
                redex = self.expressions[i]
                context = self.make_context(i)
                return context, redex

        return self, c.Hole()

    def make_context(self, redex_index):
        ecopy = self.expressions
        ecopy[redex_index] = c.Hole()
        return self.attrs["context"](*tuple(ecopy))


class Application(Expression):
    attrs = {
            "context": c.AppContext
            }

    def validate_operands(self, args):
        for arg in args:
            if not issubclass(type(arg), Expression):
                raise BadArgumentsContent("Expression", type(arg).__name__)

    def repr(self):
        return "(" + " ".join(exp.repr() for exp in self.expressions) + ")"

    # def make_context(self, redex_index):
    #     ecopy = self.expressions
    #     ecopy[redex_index] = c.Hole()
    #     return c.AppContext(*tuple(ecopy))

    def __init__(self, *args):
        self.expressions = []
        for e in args:
            self.expressions.append(e)

class If(Expression):
    attrs = {
            "args_exprected": 3,
            "context": c.IfContext0
            }
    #expressions = []

    def validate_operands(self, args):
        for arg in args:
            if not issubclass(type(arg), Expression):
                raise BadArgumentsContent("Expression", type(arg).__name__)

    def repr(self):
        return "(if " + self.pred().repr() + " then " + self.true().repr() + \
                " else " + self.false().repr() + ")"

    # def make_context(self, redex_index):
    #     # ecopy = list(self.__dict__.values())
    #     ecopy = self.expressions
    #     ecopy[redex_index] = c.Hole()
    #     # I bet this will never be a problem
    #     return c.IfContext0(*tuple(ecopy))

    def pred(self):
        return self.expressions[0]

    def true(self):
        return self.expressions[1]

    def false(self):
        return self.expressions[2]

    def __init__(self, *args):
        super().__init__(args)
        self.expressions = []
        for e in args:
            self.expressions.append(e)
        
