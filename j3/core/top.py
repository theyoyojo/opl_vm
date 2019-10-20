from j3.core.error import *
import copy

# program := expression

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
