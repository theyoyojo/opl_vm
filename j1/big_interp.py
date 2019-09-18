from j1.error import *
import j1.value as v
import j1.expression as e
from j1.delta import generate_delta

class QuestionablyInterpretableExpressionException(Exception):
    pass

def big_interp(ex):
    #print("big interp called on: ", ex.repr())
    if isinstance(ex, v.Value):
        return ex
    elif isinstance(ex, e.If):
        if big_interp(ex.pred).value:
            return big_interp(ex.true)
        else:
            return big_interp(ex.false)
    elif isinstance(ex, e.Application):
        for index in range(len(ex.expressions)):
            ex.expressions[index] = big_interp(ex.expressions[index])
        return generate_delta(ex.expressions)()
    else:
        raise QuestionablyInterpretableExpressionException("what is: ", ex, "?")
