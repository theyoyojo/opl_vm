from j4.core.error import *
import copy

class Expression:
    def __init__(self, *args):
        pass

    def pp(self):
        print(self.repr())

    def isvalue(self):
        return False

    def __init__(self, *args):
        self.expressions = []
        for e in args:
            self.expressions.append(e)

class Application(Expression):

    def repr(self):
        return "(" + " ".join(exp.repr() for exp in self.expressions) + ")"

class If(Expression):
    attrs = {
            "args_exprected": 3,
            }

    def repr(self):
        return "(if " + self.pred().repr() + " then " + self.true().repr() + \
                " else " + self.false().repr() + ")"

    def pred(self):
        return self.expressions[0]

    def true(self):
        return self.expressions[1]

    def false(self):
        return self.expressions[2]

class Abort(Expression):
    attrs = {
            "args_expected": 1,
            }

    def expr(self):
        return self.expressions[0]

    def repr(self):
        return "(ABORT {})".format(self.expr().repr())

class CallCC(Expression):
    attrs = {
            "args_expected": 1,
            }

    def expr(self):
        return self.expressions[0]

    def repr(self):
        return "(call/cc {})".format(self.expr().repr())
