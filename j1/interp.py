from j1.error import *
import j1.value as v
import j1.expression as e
import j1.context as c
from j1.delta import generate_delta
import copy

class QuestionablyInterpretableExpressionException(Exception):
    pass

def big_interp(code):
    ex = copy.deepcopy(code)
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
    code = copy.deepcopy(ex)
     
    work_exists = not isinstance(code, v.Value)
    print("Work? ", work_exists)
    while work_exists:
        context, redex = code.find_redex()
        print(context.repr(), redex.repr())
        if isinstance(redex, e.If):
            if redex.pred().value:
                code = context.plug(redex.true())
            else:
                code = context.plug(redex.false())
        elif isinstance(redex, e.Application):
            code = context.plug(generate_delta(redex.expressions)())
        else:
            raise QuestionablyInterpretableExpressionException("what is: ", code, "?")
        work_exists = not isinstance(code, v.Value)

    return code

class Rstack:
    def __init__(self):
        self.rstack = []

    def bottom(self):
        return self.rstack[-1]

    def push(self, context):
        self.rstack.append(context)

    def pop(self):
        return self.rstack.pop()

    def empty(self):
        return len(self.rstack) == 0 or isinstance(self.rstack[-1], c.Hole)

    def repr(self):
        if self.empty():
            return "E[holes all the way down]"
        else:
            return "{" + str(len(self.rstack)) + "}" + self.rstack[-1].repr()


# find_redex() -> context, redex
# e* or v* mean zero or more of e or v respectively

def CC0(ex):
    code = copy.deepcopy(ex)
    print("CC0 machine execution begins")
    RK = Rstack()
    i = 0
    while 1:
        print("Iteration: ", i) 
        print("< ", code.repr(), ", ", RK.repr(), ">")
        if issubclass(type(code), v.Value):
            if RK.empty():
                # <v, Hole> => done :)
                break
            elif isinstance(RK.bottom(), c.AppContext):
                if len(RK.bottom().expressions) == 0:
                    # <v_n, E[v0, ... hole]> |-> <delta(Application[v0,...,v_n],E>
                    code = generate_delta(RK.bottom().plug(code).expressions)()
                    # next, pop the lcon off the stack and we are done with it
                    RK.pop()
                elif len(RK.bottom().expressions) > 0:
                    # <v_i, E[v*, hole, e_i+1, e*]> |-> <v_i+i, E[v*, v_i, hole, e*]>
                    RK.bottom().values.append(code)
                    code = RK.bottom().expressions.pop(0)
            elif isinstance(RK.bottom(), c.IfContext0):
                if code.value:
                    # <true, E[if hole e_true e_false]> |-> <e_true, E>
                    code = RK.bottom().true()
                else:
                    # <false, E[if hole e_true e_false]> |-> <e_false , E>
                    code = RK.bottom().false()
                # Both cases require that we pop the bottom
                RK.pop()
        elif isinstance(code,  e.If):
            # < if pred true false, E> |-> <pred, E[IfC hole true false]>
            code_ = copy.deepcopy(code)
            context = code.make_context(0)
            code = code_.pred()
            RK.push(context)
        elif isinstance(code, e.Application):
            # < App[e0, e1, e*], E> |-> < e0, E[hole, e1, e*]
            code_ = copy.deepcopy(code)
            context = code.make_context(0)
            code = code_.expressions[0]
            RK.push(context)

    print("CC0 RESULT: ", code.repr())
    return code
