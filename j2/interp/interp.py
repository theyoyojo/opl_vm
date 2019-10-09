from j2.core.error import *
import j2.core.value as v
import j2.core.expression as e
from j2.core.delta import generate_delta
import j2.core.top as t
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
        if isinstance(ex.expressions[0], v.Value) and ex.expressions[0].value in big_interp.sigma.keys():
            ex = big_interp.sigma[ex.expressions[0].value].subst(ex.expressions)
            # ex.pp()
            return big_interp(ex)
        return generate_delta(ex.expressions)()
    elif isinstance(ex, t.Program):
        # Interpret all top level exprs in program but return the last one only
        for index in range(len(ex.exprs)):
            ex.exprs[index] = big_interp(ex.exprs[index])
        return big_interp(ex.exprs[-1])
    # If we encounter a define, map it's id to it's defintion in our global sigma
    elif isinstance(ex, t.Define):
        big_interp.sigma[ex.id().value] = ex
    else:
        raise QuestionablyInterpretableExpressionException("what is: ", ex.repr(), "?")
big_interp.sigma = {}
