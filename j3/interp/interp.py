from j3.core.error import *
import j3.core.value as v
import j3.core.expression as e
from j3.core.delta import generate_delta
import j3.core.top as t
import copy

class QuestionablyInterpretableExpressionException(Exception):
    pass

class Env:
    def __init__(self):
        self.map = {}

    def bind(self, binding, vals):
        i = 0
        if len(binding) != len(vals):
            raise ValueError("Lambda expects {} variables, got {}".format(len(binding), len(vals)))
        for ident in binding:
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

class Clo:
    def __init__(self, lam, env):
        self.lam = lam
        self.env = env

    def extend(self, binding, values):
        self.env.bind(binding, values)


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
        if isinstance(ex, v.Lambda):
            return Clo(ex, env)
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

        # <vn, _, kapp(clo(lam(x0, .., xn),env)) v0 ..., _, (), K> =>
        #   <e, env[x0 <- v0]...[xn <- vn], K>
        if isinstance(ex.expressions[0], Clo):
            env = ex.expressions[0].env
            env.bind(ex.expressions[0].lam.binding, ex.expressions[1:])
            ex = ex.expressions[0].lam.expr
            return big_interp(ex, env)

        return generate_delta(ex.expressions)()
    else:
        raise QuestionablyInterpretableExpressionException("what is: ", ex.repr(), "?")
big_interp.sigma = {}
