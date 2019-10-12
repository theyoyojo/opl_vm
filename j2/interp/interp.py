from j2.core.error import *
import j2.core.value as v
import j2.core.expression as e
from j2.core.delta import generate_delta
import j2.core.top as t
import copy

class QuestionablyInterpretableExpressionException(Exception):
    pass

class Env:
    def __init__(self):
        self.map = {}

    def bind(self, binding, vals):
        i = 0
        if len(binding) != len(vals):
            raise ValueError("Call to {} expects {} variables, got {}".format(binding[0], len(binding), len(vals)))
        for ident in binding:
            if i == 0:
                # skip name
                i = i + 1
                continue
            self.map[ident.value] = vals[i]
            i = i + 1

    def subst(self, ex):
        if ex.value in self.map.keys():
            return self.map[ex.value]
        else:
            raise ValueError("Unbound identifier {} referenced!".format(ex.value)) 

    def maps(self, ID):
        if ID.value in self.map.keys():
            return True

    def repr(self):
        return repr(self.map)

    def pp(self):
        print(self.repr())


def big_interp(code, env):
    # code.pp()
    ex = copy.deepcopy(code)
    # < x, env, k> => <env(x), 0, k>
    if isinstance(ex, v.ID):
        if env.maps(ex):
            return env.subst(ex)   
        else:
            return ex
        # < v, 0, k> => <v, 0 , k>
    elif isinstance(ex, v.Value):
        return ex
    # <if c t f, env, k> => <c, env, kif env t f k>
    elif isinstance(ex, e.If):
        #<T, env, kif env t f k> => <t, env, k>
        if big_interp(ex.pred(), env).value:
            return big_interp(ex.true(), env)
        #<F, env, kif env t f k> => <f, env, k>
        else:
            return big_interp(ex.false(), env)
    elif isinstance(ex, e.Application):
        # <e e' ..., env, k> => <v v' ..., env k> 
        for index in range(len(ex.expressions)):
            ex.expressions[index] = big_interp(ex.expressions[index], env)
        # <(f v v' ...), env, ... idk this formalism breaks down in a non machine environment
        if isinstance(ex.expressions[0], v.ID) and ex.expressions[0].value in big_interp.sigma.keys():
            definition = big_interp.sigma[ex.expressions[0].value]
            # func.pp()
            new_env = Env()
            new_env.bind(definition.binding, ex.expressions)
            # new_env.pp()
            # ex.pp()
            return big_interp(definition.expr, new_env)
        return generate_delta(ex.expressions)()
    elif isinstance(ex, t.Program):
        # Interpret all top level exprs in program but return the last one only
        for index in range(len(ex.exprs)):
            ex.exprs[index] = big_interp(ex.exprs[index], env)
        return big_interp(ex.exprs[-1], env)
    # If we encounter a define, map it's id to it's defintion in our global sigma
    elif isinstance(ex, t.Define):
        big_interp.sigma[ex.id().value] = ex
    else:
        raise QuestionablyInterpretableExpressionException("what is: ", ex.repr(), "?")
big_interp.sigma = {}
