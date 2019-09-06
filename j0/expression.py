#def binary_init(obj, left, right)
#def binary_pp

class Mult:
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def repr(self):
        return "(* " + self.left.repr() + " " + self.right.repr() + ")"

    def pp(self):
        print("\n" + self.repr() + "\n")

class Add:
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def repr(self):
        return "(+ " + self.left.repr() + " " + self.right.repr() + ")"

    def pp(self):
        print("\n" + self.repr() + "\n")
     
