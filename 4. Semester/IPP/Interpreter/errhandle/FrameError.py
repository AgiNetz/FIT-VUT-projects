
class FrameError(BaseException):

    def __init__(self, message):
        self.message = message

    def __str__(self):
        return "Invalid frame operation! %s" % self.message
