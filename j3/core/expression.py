from j2.core.error import *
import copy

class Expression:
    def __init__(self, *args):
        pass

    def pp(self):
        print(self.repr())

    def isvalue(self):
        return False

    def subst(self, ident, value):
        for index in range(len(self.expressions)):
            # print(ident.value, index, value.value)
            # print("INTO:",self.expressions[index].repr())
            self.expressions[index] = self.expressions[index].subst(ident, value)
            # self.pp()
        return self

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

    def __init__(self, *args):
        super().__init__(args)
        self.expressions = []
        for e in args:
            self.expressions.append(e)
        
