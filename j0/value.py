class Number():
    def __init__(self, number):
        self.number = number

    def repr(self):
        return str(self.number)

    def pp(self):
        print(self.repr())
