

class BadVariableError(BaseException):

    def __init__(self, variable):
        self.variable = variable

    def __str__(self):
        return "The variabe \"%s\" doesn't exist!" % self.variable