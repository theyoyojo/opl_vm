import j0.expression as e
import j0.value as v

def is_number(string):
    try:
        float(string)
        return True
    except ValueError:
        return False


class SaltySyntax(Exception):
    pass

class Nil:
    tstr  = "Nil"
    def __init__(self):
        pass

    def repr(self):
        return "()"

    def pp(self):
        print("\n" + self.repr() + "\n")

    def desu(self):
        return

class Atom:
    tstr = "Atom"
    def __init__(self, value):
        self.value = value

    def repr(self):
        return self.value

    def pp(self):
        print("\n" + self.repr() + "\n")

    def desu(self):
        if is_number(self.value):
            return v.Number(float(self.value))
        else:
            raise SaltySyntax("Not sure how to desugar this atom")

class Cons:
    tstr = "Cons"
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def repr(self):
        return "(" + self.left.repr() + " " + self.right.repr() + ")"

    def pp(self):
        print("\n" + self.repr() + "\n")

    def desu(self):
        # What do we have on the right?
        # if it's another cons, recursively simplify that thing to an atom
        # Actually, assume it can never be non-atom, because that would make no sense

        if self.left.repr() == "()":
            return
         
        if is_number(self.left.value):
            if self.right.repr() == "()":
                return v.Number(float(self.left.value))

        if self.left.value == "+":
            return e.Add(self.right.left.desu(),self.right.right.desu())
        if self.left.value == "*":
            return e.Mult(self.right.left.desu(),self.right.right.desu())
        else:
            raise SaltySyntax("Something went wrong :)")

