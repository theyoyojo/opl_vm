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
            context_label = "E"
        else:
            context_label = "C"
        return context_label + "[" + ' '.join(exp.repr() for exp in self.expressions) + "]"

    def pp(self):
        print(self.repr())

    def plug(self,program):
        # I am the hole
        if isinstance(self, Hole):
            return program
        for e,v in self.__dict__.items():
            if isinstance(v, Hole):
                context.__dict__[e] = program 
                return context
        # No hole to fill
        return self

class EContext:
    pass

class Hole(Context, EContext):
    def plug(self, program):
        return program

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

    def pred(self):
        return self.expressions[0]

    def true(self):
        return self.expressions[1]

    def false(self):
        return self.expressions[2]

    def plug(self, program):
         return e.If(self.expressions[0].plug(program), self.expressions[1], self.expressions[2])

    def hole(self):
        return self.pred

        # if isinstance(self.expressions[0], Hole):
        #     return e.If(program, self.expressions[1], self.expressions[2])
        # else:
        #     return self.expressions[0].plug(program)


# class IfContext1(IfContext):
#     def __init__(self, pred, true, false):
#         if not isinstance(true, Context):
#             get_mad()
#         super().__init__(pred, true, false)

#     def plug(self, program):
#         if isinstance(self.expressions[1], Hole):
#             return e.If(self.expressions[0], program, self.expressions[2])
#         else:
#             return self.expressions[1].plug(program)

# class IfContext2(IfContext):
#     def __init__(self, pred, true, false):
#         if not isinstance(false, Context):
#             get_mad()
#         super().__init__(pred, true, false)

#     def plug(self, program):
#         if isinstance(self.expressions[2], Hole):
#             return e.If(self.expressions[0], self.expressions[1], program)
#         else:
#             return self.expressions[2].plug(program)

# class AppContextGeneral(Context):
#     def __init__(self, *args):
#         i = 0
#         have_seen_context = False
#         for item in args:
#             if isinstance(item, Context):
#                 have_seen_context = True
#                 self.context_index = i
#             i = i + 1
#         if not have_seen_context:
#             raise BadErrorException("AppContextGeneral must contain one context")
#         super().__init__(self, *args)

#     def plug(self, program):
#         ecopy = self.expressions[self.context_index].plug(program)
#         return e.Application(*tuple(ecopy))

        # if isinstance(ecopy[self.context_index],Hole):
        #     ecopy[self.context_index] = program
        #     return e.Application(*tuple(ecopy))
        # else:
        #     return ecopy[self.context_index].plug(program)
            
            
# Need vs before hole, es after
class AppContext(Context, EContext):
    def __init__(self, *args):
        self.values = []
        self.expressions = []
        after_hole = False
        print("new appc: ", args)
        for e in args:
            if issubclass(type(e),Context):
                print("we have hole")
                self.hole = e
                after_hole = True
            elif after_hole:
                self.expressions.append(e)
            else:
                self.values.append(e)

    def plug(self, program):
        # print("AA")
        # e = self.hole.plug(program)
        # print(e)
        # print(self.values + [e])
        # print(self.hole.plug(program))
        # print(self.expressions)
        return e.Application(*tuple(self.values + [self.hole.plug(program)] + self.expressions))

# def find_redex(program):
#     # print()
#     # print(type(program),program)
#     for attr, value in program.__dict__.items():
#         # print(attr, value)
#         if issubclass(type(value),e.Expression):
#             redex = value
#             program.__dict__[attr] = Hole()
#             return redex, program

#     return program, Hole()

# def sinterp(program):
#     i = 0
#     while 1:
#         print("ORIGIN")
#         program.pp()
#         # Short circuit condexpr
#         if isinstance(program,e.Cond) and isinstance(program.pred, v.Bool):
#             if program.pred.value:
#                 program = program.true
#             else:
#                 program = program.false
#             print("OPTIMIZE_IF")
#             program.pp()
        
#         redex, context = find_redex(program)
#         print("DIVISION:")
#         redex.pp()
#         context.pp()
#         if isinstance(context, Hole):
#             print("EVALUATION:")
#             program.pp()
#             program = program.binterp()
#             program.pp()
#             return program
        
#         redex_reloaded = sinterp(redex)
#         plug(redex_reloaded, context)
#         print("UNITY")
#         context.pp()

#         if i > 100:
#             break

#         i = i + 1

