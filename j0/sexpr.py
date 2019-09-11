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
            return self.right
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

    def desu(self):
        return

class Atom(Sexpr):
    def __init__(self, value):
        self.value = value

    def repr(self):
        return self.value

    def pp(self):
        print("\n" + self.repr() + "\n")

    # def desu(self):
    #     if is_number(self.value):
    #         return v.Number(float(self.value))
    #     else:
    #         raise SaltySyntax("Not sure how to desugar this atom")

class Cons(Sexpr):
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def repr(self):
        return "(" + self.left.repr() + " " + self.right.repr() + ")"

    def pp(self):
        print("\n" + self.repr() + "\n")

    # def desu(self):
    #     # What do we have on the right?
    #     # if it's another cons, recursively simplify that thing to an atom
    #     # Actually, assume it can never be non-atom, because that would make no sense

    #     if self.left.repr() == "()":
    #         return
         
    #     if is_number(self.left.value):
    #         if self.right.repr() == "()":
    #             return v.Number(float(self.left.value))

    #     if self.left.value == "+":
    #         return e.Add(self.right.left.desu(),self.right.right.desu())
    #     elif self.left.value == "*":
    #         return e.Mult(self.right.left.desu(),self.right.right.desu())
    #     elif self.left.value == "-":
    #         # UNARY spaghetti
    #         # if self.right.right.repr() == "()" or not is_number(self.right.right.value):
    #         #     return
    #         return e.Add(e.Mult(self.right.right.desu(),v.Number(-1)),self.right.left.desu())
    #     else:
    #         raise SaltySyntax("Something went wrong :)")



def desugar(sexpr):
    print(sexpr.length())


