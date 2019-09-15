from j1.error import *

class UndefinedFunctionException(Exception):
    pass

class Undefined:
    def __init__(self, *args):
        # Should I raise an exception here?
        print("Undefined function!\n" \
                "args: " + str(args))
        self.args = args

    def repr(self):
        return "[!]Undefined (function " + str(self.args) + ")[!]"

    def binterp(self):
        raise UndefinedFunctionException("Impossible to interpret undefined function")
