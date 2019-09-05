class Number():
    def __init__(self, number):
        self.number = number

    def repr(self):
        return str(self.number)

    def pretty_print(self):
        print(self.repr())
