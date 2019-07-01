

class SemanticError(BaseException):

    def __init__(self, message):
        self.message = message

    def __str__(self):
        return "Semantic error found! %s" % self.message