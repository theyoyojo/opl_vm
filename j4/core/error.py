class BadArguments(Exception):
    def __init__(self, description, expected, given):
        super().__init__(description + ":\n" \
                "\texpected: " + expected + "\n" \
                "\tgiven: " + given + "\n")

class BadArgumentsCount(Exception):
    def __init__(self, expected, given):
        super().__init__("Invalid number of arguments given", expected, given)

class BadArgumentsContent(Exception):
    def __init__(self, expected, given):
        super().__init__("Invalid argument content", expected , given)
