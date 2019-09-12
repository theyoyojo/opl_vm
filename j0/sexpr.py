import j0.expression as e
import j0.value as v

def is_number(string):
    try:
        float(string)
        return True
    except ValueError:
        return False

class Sexpr:
    def length(self):
        if isinstance(self, Nil):
            return 0
        elif isinstance(self, Cons):
            return 1 + self.right.length()
        else:
            raise SaltySyntax("expected a Cons or Nil sexpr, not this garbage")

    def desu(self):
        return desugar(self)

    def first(self):
        if isinstance(self, Nil):
            return Nil()
        elif isinstance(self, Cons):
            return self.left
        else:
            raise SaltySyntax("expected a Cons or Nil sexpr, not this garbage")

    def rest(self):
        if isinstance(self, Nil):
            return Nil()
        elif isinstance(self, Cons):
            return self.right
        else:
            raise SaltySyntax("expected a Cons or Nil sexpr, not this garbage")
        

    def second(self):
        if isinstance(self, Nil):
            return Nil()
        elif isinstance(self, Cons):
            return self.right.left
        else:
            raise SaltySyntax("expected a Cons or Nil sexpr, not this garbage")


class SaltySyntax(Exception):
    pass

class Nil(Sexpr):
    def __init__(self):
        pass

    def repr(self):
        return "()"

    def pp(self):
        print("\n" + self.repr() + "\n")

class Atom(Sexpr):
    def __init__(self, value):
        self.value = value

    def repr(self):
        return self.value

    def pp(self):
        print("\n" + self.repr() + "\n")

class Cons(Sexpr):
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def repr(self):
        return "(" + self.left.repr() + " " + self.right.repr() + ")"

    def pp(self):
        print("\n" + self.repr() + "\n")

def desugar_minus(sexpr):
    # Unary case
    if sexpr.length() == 1:
        return e.Mult(v.Number(-1), sexpr.first().desu())
    # Binary case
    elif sexpr.length() == 2:
        return e.Add(sexpr.first().desu(),
            e.Mult(v.Number(-1), sexpr.second().desu()))
    else:
        raise SaltySyntax("- function expects 1 or 2 arguments, got " + 
                sexpr.length() + " instead")

def desugar_plus(sexpr):
    # Base case
    if sexpr.length() == 2:
        return e.Add(sexpr.first().desu(), sexpr.second().desu())
    # Recursive case
    elif sexpr.length() > 2:
        return e.Add(sexpr.first().desu(), Cons(Atom("+"), sexpr.rest()).desu())
    else:
        raise SaltySyntax("unknown error. Negative length?")
    
    

def desugar_mult(sexpr):
    # Base case
    if sexpr.length() == 2:
        return e.Mult(sexpr.first().desu(), sexpr.second().desu())
    # Recursive case
    elif sexpr.length() > 2:
        return e.Mult(sexpr.first().desu(), Cons(Atom("*"), sexpr.rest()).desu())
    # Error case (basement case)
    else:
        raise SaltySyntax("* function expects more than 0 arguments")

antirecipies = { \
        "-": desugar_minus,
        "+": desugar_plus,
        "*": desugar_mult
        }

def desugar_cons(sexpr):
    action = sexpr.first().repr()
    if action in antirecipies:
        return antirecipies[action](sexpr.rest())
    elif sexpr.length() == 1:
        return sexpr.first().desu()
    else:
        raise SaltySyntax("Unknown function \"" + action + "\"")

def desugar_atom(sexpr):
    if is_number(sexpr.repr()):
        return v.Number(float(sexpr.repr()))
    else:
        raise SaltySyntax("Unsure how to desugar atomic sexpr \"" + sexpr.repr() + "\"")

def desugar(sexpr):
    #print("sexpr length: " + str(sexpr.length())
    # sexpr.pp()
    if isinstance(sexpr, Cons):
        return desugar_cons(sexpr)
    elif isinstance(sexpr, Atom):
        return desugar_atom(sexpr)
    elif isinstance(sexpr, Nil):
        raise SaltySyntax("Cannot desugar a null cons cell")
    else:
        raise SaltySyntax("Unknown object, giving up")
    
