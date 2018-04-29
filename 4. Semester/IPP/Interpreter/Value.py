from errhandle import *


class Value:
    """Class representing a value in memory during interpretation"""
    def __init__(self, value, t):
        self.type = t
        if t == "int":
            try:
                self.value = int(value)
            except ValueError:
                raise InternalError("Bad Value call!")
        elif t == "bool":
            if isinstance(value, bool):
                self.value = value
            elif value == "true":
                self.value = True
            elif value == "false":
                self.value = False
            else:
                raise InternalError("Bad Value call!")
        elif t == "string":
            self.value = value
        else:
            raise InternalError("Bad Value call!")

