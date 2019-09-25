from j1.error import *
import j1.value as v
import j1.expression as e
import j1.context as c
from j1.delta import generate_delta

class QuestionablyInterpretableExpressionException(Exception):
    pass

def big_interp(ex):
    #print("big interp called on: ", ex.repr())
    if isinstance(ex, v.Value):
        return ex
    elif isinstance(ex, e.If):
        if big_interp(ex.pred()).value:
            return big_interp(ex.true())
        else:
            return big_interp(ex.false())
    elif isinstance(ex, e.Application):
        for index in range(len(ex.expressions)):
            ex.expressions[index] = big_interp(ex.expressions[index])
        return generate_delta(ex.expressions)()
    else:
        raise QuestionablyInterpretableExpressionException("what is: ", ex, "?")

def small_interp(ex):
    print("small_interp( ", ex.repr(), " )")
    context, redex = ex.find_redex()
    print("redex: ", redex.repr())
    print("context: ", context.repr())
    if isinstance(redex, c.Hole):
        if isinstance(context, v.Value):
            return context
        # elif isinstance(context, e.Application):
        #     return generate_delta(ex.expressions)()
        else:
            raise QuestionablyInterpretableExpressionException("what is: ", ex, "?")
    else:
        new_redex = small_interp(redex) 
        print("new_redex: ", new_redex.repr())
        if isinstance (context, c.AppContext):
            new_app = context.plug(new_redex)
            return generate_delta(new_app.expressions)()
        elif isinstance(context, c.IfContext0):
            if small_interp(new_redex).value:
                return small_interp(context.expressions[1])
            else:
                return small_interp(context.expressions[2])
        else:
            return context.plug(new_redex)

# Need context parsers
# need to replace lowest context with a value

# left to right 
# get lowest context
def lowcon(context):
    if isinstance(context.hole(), c.Hole):
        # this is it
        return context
    else:
        # we need to go deeper
        return lowcon(context.hole())

# get second lowest context to "pop off the reverse stack"
# def low2con(context):
#     # This may cause problems
#     if isinstance(context.hole(), c.Hole) or isinstance(context.hole().hole(), c.Hole:
#         # this is it, no where else to go or this is a shallow context anyway
#         return context
#     else:
#         # we need to go deeper
#         return lowcon(context.hole())

def lcon_pop(context):
    pass 

def lcon_push(context):
    pass



def CC0(ex):
    print("CC0 machine execution begins")
    code = ex
    context = c.Hole()
    i = 0
    while 1:
        print("Iteration: ", i) 
        print("< ", code.repr(), ", ", context.repr(), ">")
        lcon = lowcon(context)
        f
        if issubclass(type(code), v.Value):
            if isinstance(lcon, c.Hole):
                # <v, Hole> => done
                break
            elif isinstance(lcon, e.AppContext):
                if isinstance(lcon.expressions[-1], c.Hole):
                    # <vn, E[v0, ... hole]> |-> <delta(v0,...,vn),E>
                    code = generate_delta(lcon.expressions)()
                    # next, pop the lcon off the stack
        elif isinstance(code,  e.If):
            # < if pred true false, E> |-> <pred, E[IfC hole true false]>
            context, code = code.find_redex() # break it up


        new_context, code = code.find_redex()
        context = context.plug(new_context)
        print("Iteration: ",i) 
        print("< ", code.repr(), ", ", context.repr(), ">")
        i = i + 1
        if i > 100:
            break

    return code
