import j1.value as v

class Expression:
    def pp(self):
        print(self.repr())

class Mult(Expression):
    def __init__(self, left, right):
        
        self.left = left
        self.right = right

    def repr(self):
        return "(* " + self.left.repr() + " " + self.right.repr() + ")"

    def ibig(self):
        return v.Number(self.left.ibig().number * self.right.ibig().number)

class Add(Expression):
    def __init__(self, left, right):
        self.left = left
        self.right = right

    def repr(self):
        return "(+ " + self.left.repr() + " " + self.right.repr() + ")"

    def ibig(self):
        return v.Number(self.left.ibig().number + self.right.ibig().number)

class Cond(Expression):
    def __init__(self, pred, true, false):
        self.pred = pred
        self.true = true
        self.false = false

    def repr(self):
        return "(if " + self.pred.repr() + " then " + self.true.repr() + \
                " else " + self.false.repr() + ")"

    def ibig(self):
        if v.Bool(pred.ibig()).value:
            return true.ibig()
        else:
            return false.ibig()
