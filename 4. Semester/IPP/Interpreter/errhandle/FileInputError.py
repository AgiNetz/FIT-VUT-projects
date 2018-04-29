
class FileInputError(BaseException):

    def __init__(self, filename, message):
        self.filename = filename
        self.message = message

    def __str__(self):
        return "Could not open file %s! %s" % (self.filename, self.message)
