class LexError(BaseException):

    def __init__(self, message):
        self.message = message

    def __str__(self):
        return "Lexical error found! %s" % self.message