
class AngryE(Exception):
    pass

def get_mad(*args):
    raise AngryE("expected a Cons or Nil sexpr, not this garbage: " + str(args))

class Sexpr:
    def length(self):
        if isinstance(self, Nil):
            return 0
        elif isinstance(self, Cons):
             return 1 + self.right.length()
        # Not sure if taking length of atoms is needed/defined
        # elif isinstance(self, Atom):
        #     return 1
        else:
            get_mad()

    def first(self):
        if isinstance(self, Nil):
            return Nil()
        elif isinstance(self, Cons):
            return self.left
        else:
            get_mad()

    def rest(self):
        if isinstance(self, Nil):
            return Nil()
        elif isinstance(self, Cons):
            return self.right
        else:
            get_mad()

    def second(self):
        if isinstance(self, Nil):
            return Nil()
        elif isinstance(self, Cons):
            return self.right.left
        else:
            get_mad()

class Nil(Sexpr):
    def __init__(self):
        pass

    def repr(self):
        return "()"

    def pp(self):
        print("\nSexpr:" + self.repr() + "\n")

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
