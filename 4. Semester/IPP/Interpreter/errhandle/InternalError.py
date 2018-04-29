
class InternalError(BaseException):

    def __init__(self, message):
        self.message = message

    def __str__(self):
        return "An internal error has occured! %s" % self.message