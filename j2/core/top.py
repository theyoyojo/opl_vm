from j2.core.error import *
import copy

# program := expression | topform ... expression
# topform := define | expression

class Program:
    def __init__(self, *args):
        self.sigma = {}
        self.exprs = []
        for expr in args:
            self.exprs.append(expr)

    def repr(self):
        return "Program[" + str(len(self.exprs)) + "]"

    def pp(self):
        print(self.repr())


class Define:
    def __init__(self, binding, expr):
        self.binding = binding
        # for b in self.binding:
        #     b.pp()
        self.expr = expr

    def repr(self):
        return "def " + self.id().repr() + "(" + ' '.join(ident.repr() for ident in self.binding[1:]) + ")[" + self.expr.repr() + "]"

    def id(self):
        return self.binding[0]

    def pp(self):
        print(self.repr())

    def subst(self, args):
        expr = copy.deepcopy(self.expr)
        # print("bindlen", len(self.binding), "arglen", len(args))
        if len(args) != len(self.binding):
            raise BadArgumentsCount(len(args) - 1, len(self.binding) - 1)
        for index in range(len(args)):
            if index == 0:
                continue
            expr = expr.subst(self.binding[index], args[index])
        # expr.pp()
        return expr

