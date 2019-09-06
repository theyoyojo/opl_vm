import j0.value as v

class Mult:
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def repr(self):
        return "(* " + self.left.repr() + " " + self.right.repr() + ")"

    def pp(self):
        print("\n" + self.repr() + "\n")

    def ibig(self):
        return v.Number(self.left.ibig().number * self.right.ibig().number)

class Add:
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def repr(self):
        return "(+ " + self.left.repr() + " " + self.right.repr() + ")"

    def pp(self):
        print("\n" + self.repr() + "\n")

    def ibig(self):
        return v.Number(self.left.ibig().number + self.right.ibig().number)

