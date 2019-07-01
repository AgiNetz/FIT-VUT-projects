

class MissingValueError(BaseException):

    def __init__(self, location):
        self.location = location

    def __str__(self):
        return "%s is empty/uninitialized!" % self.location
