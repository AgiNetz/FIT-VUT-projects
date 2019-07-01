

class StringError(BaseException):

    def __init__(self, message):
        self.message = message

    def __str__(self):
        return "Invalid string operation! %s" % self.message
