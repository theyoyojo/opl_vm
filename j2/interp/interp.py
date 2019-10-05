from j2.core.error import *
import j2.core.value as v
import j2.core.expression as e
from j2.core.delta import generate_delta
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
