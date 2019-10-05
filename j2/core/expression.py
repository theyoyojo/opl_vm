from j2.core.error import *
import copy

# perhaps turn validate_operands function into work done in
# Expression.__init__() via argspec key/[] pair in attrs (but this is very unnecessary)

class Expression:
    def __init__(self, *args):
        pass

    def pp(self):
        print(self.repr())

    def isvalue(self):
        return False


class Application(Expression):

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

    def validate_operands(self, args):
        for arg in args:
            if not issubclass(type(arg), Expression):
                raise BadArgumentsContent("Expression", type(arg).__name__)

    def repr(self):
        return "(if " + self.pred().repr() + " then " + self.true().repr() + \
                " else " + self.false().repr() + ")"

    def pred(self):
        return self.expressions[0]

    def true(self):
        return self.expressions[1]

    def false(self):
        return self.expressions[2]

    def find_redex(self):
        if not self.pred().isvalue():
            print("subpiece: ", self.pred().repr())
            context = self.make_context(0)
            subcontext, redex = self.pred().find_redex()
            context.holeswap(subcontext)
            return context, redex
        else:
            return c.Hole(), self

    def __init__(self, *args):
        super().__init__(args)
        self.expressions = []
        for e in args:
            self.expressions.append(e)
        
