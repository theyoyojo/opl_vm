from j1.error import *
import j1.context as c
import copy

# perhaps turn validate_operands function into work done in
# Expression.__init__() via argspec key/[] pair in attrs (but this is very unnecessary)

class Expression:
    def __init__(self, *args):
        # First, validate argument count
        # if "args_exppected" in self.attrs:
        #     if len(args) != self.attrs["args_expected"]:
        #         raise  BadArgumentsCount(self.attrs["args_expected"], len(args))
        # # Now, we can safely validate the content
        # self.validate_operands(*args)
        pass

    def pp(self):
        print(self.repr())

    def make_context(self, redex_index):
        ecopy = copy.deepcopy(self.expressions)
        ecopy[redex_index] = c.Hole()
        return self.attrs["context"](*tuple(ecopy))

    def isvalue(self):
        return False


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

    def __init__(self, *args):
        self.expressions = []
        for e in args:
            self.expressions.append(e)

    def find_redex(self):
        i = 0
        for exp in self.expressions:
            if not exp.isvalue():
                print("subpiece: ", exp.repr())
                context = self.make_context(i)
                subcontext, redex = exp.find_redex()
                context.holeswap(subcontext)
                return context, redex
            i +=1
        
        else:
            return c.Hole(), self

class If(Expression):
    attrs = {
            "args_exprected": 3,
            "context": c.IfContext0
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
        
