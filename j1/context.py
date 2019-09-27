import j1.expression as e
import j1.value as v

def BadErrorException(Exception):
    pass

def get_mad():
    raise BadErrorException("Something went wrong >:(")

class Context:
    def __init__(self, *args):
        self.expressions = []
        for ex in args:
            self.expressions.append(ex)

    def repr(self):
        if issubclass(type(self), EContext):
            context_label = "E" + type(self).__name__[0:3]
        else:
            context_label = "C" # This should not happen
        return context_label + "[" + ' '.join(exp.repr() for exp in self.expressions) + "]"

    def pp(self):
        print(self.repr())

class EContext:
    pass

class Hole(Context, EContext):
    def plug(self, program):
        return program

    def repr(self):
        return "[HOLE]"

    def hole(self):
        return self

class IfContext(Context):
    def __init__(self, pred, true, false):
        super().__init__(pred, true, false)


class IfContext0(IfContext, EContext):
    def __init__(self, pred, true, false):
        if not isinstance(pred, Context):
            get_mad()
        super().__init__(pred, true, false)

    def pred(self):
        return self.expressions[0]

    def true(self):
        return self.expressions[1]

    def false(self):
        return self.expressions[2]

    def plug(self, program):
         return e.If(self.expressions[0].plug(program), self.expressions[1], self.expressions[2])

    def hole(self):
        return self.pred()

    def holeswap(self, context):
        self.expressions[0] = context
    
    def repr(self):
        return "EIfC[" + ' '.join(exp.repr() for exp in self.expressions) + "]"
            
# Need vs before hole, es after
class AppContext(Context, EContext):
    def __init__(self, *args):
        self.values = []
        self.expressions = []
        after_hole = False
        for e in args:
            if issubclass(type(e),Context):
                self.apphole = e
                after_hole = True
            elif after_hole:
                self.expressions.append(e)
            else:
                self.values.append(e)

    def plug(self, program):
        return e.Application(*tuple(self.values + [self.apphole.plug(program)] + self.expressions))

    def hole(self):
        return self.apphole

    def repr(self):
        return "EAppC[" + ' '.join(val.repr() for val in self.values) + ' ' + \
            self.hole().repr() + ' ' +  \
            ' '.join(exp.repr() for exp in self.expressions) + "]"

    def holeswap(self, context):
        self.apphole = context
