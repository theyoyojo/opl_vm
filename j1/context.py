import j1.expression as e
import j1.value as v
class Hole:
    def repr(self):
        return "[HOLE]"
    def pp(self):
        print(self.repr())

class Context:
   expressions = []

   def __init__(self, *args):
       for expression in args:
           expressions.append(expression)

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

