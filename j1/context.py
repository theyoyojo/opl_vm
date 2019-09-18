import j1.expression as e
import j1.value as v

def BadErrorException(Exception):
    pass

def get_mad():
    raise BadErrorException("Something went wrong >:(")

class Context:
    expressions = []
    def __init__(self, *args):
       for expression in args:
           expressions.append(expression)

    def repr(self):
        if issubclass(self, EContext):
            context_label = "E"
        else:
            context_label = "C"
        return context_label + "[" + ' '.join(exp.repr() for exp in expressions) + "]"

    def pp(self):
        print(self.repr())

class EContext:
    pass

class Hole(Context, EContext):
    def repr(self):
        return "[HOLE]"

class IfContext(Context):
    def __init__(self, pred, true, false):
        super().__init__(pred, true, false)


class IfContext0(IfContext, EContext):
    def __init__(self, pred, true, false):
        if not isinstance(pred, Context):
            get_mad()
        super().__init__(pred, true, false)


class IfContext1(IfContext):
    def __init__(self, pred, true, false):
        if not isinstance(true, Context):
            get_mad()
        super().__init__(pred, true, false)

class IfContext2(IfContext):
    def __init__(self, pred, true, false):
        if not isinstance(false, Context):
            get_mad()
        super().__init__(pred, true, false)

class AppContextGeneral(Context):
    def __init__(self, *args):
        have_seen_context = False
        for item in args:
            if isinstance(item, Context):
                have_seen_context = True
        if not have_seen_context:
            raise BadErrorException("AppContextGeneral must contain one context")
        super().__init__(self, *args)
            
class AppContext(Context, EContext):
    pass
    # Need vs before hole, es after

def plug(program, context):
    i = 0
    if isinstance(context, Hole):
        return program
    for e,v in context.__dict__.items():
        if isinstance(v, Hole):
            context.__dict__[e] = program 
            return context
        i = i + 1
    return context

def find_redex(program):
    # print()
    # print(type(program),program)
    for attr, value in program.__dict__.items():
        # print(attr, value)
        if issubclass(type(value),e.Expression):
            redex = value
            program.__dict__[attr] = Hole()
            return redex, program

    return program, Hole()

def sinterp(program):
    i = 0
    while 1:
        print("ORIGIN")
        program.pp()
        # Short circuit condexpr
        if isinstance(program,e.Cond) and isinstance(program.pred, v.Bool):
            if program.pred.value:
                program = program.true
            else:
                program = program.false
            print("OPTIMIZE_IF")
            program.pp()
        
        redex, context = find_redex(program)
        print("DIVISION:")
        redex.pp()
        context.pp()
        if isinstance(context, Hole):
            print("EVALUATION:")
            program.pp()
            program = program.binterp()
            program.pp()
            return program
        
        redex_reloaded = sinterp(redex)
        plug(redex_reloaded, context)
        print("UNITY")
        context.pp()

        if i > 100:
            break

        i = i + 1

