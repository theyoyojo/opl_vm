class Nil:
    def __init__(self):
        pass

    def repr(self):
        return "()"

    def pp(self):
        print("\n" + self.repr() + "\n")

class Atom:
    def __init__(self, value):
        self.value = value

    def repr(self):
        return self.value

    def pp(self):
        print("\n" + self.repr() + "\n")

class Cons:
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def repr(self):
        return "(" + self.left.repr() + " " + self.right.repr() + ")"

    def pp(self):
        print("\n" + self.repr() + "\n")
