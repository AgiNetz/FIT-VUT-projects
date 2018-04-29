

class OperandTypeError(BaseException):

    def __init__(self, operand, instruction, type, should):
        self.operand = operand
        self.instruction = instruction
        self.type = type
        self.should = should

    def __str__(self):
        return "Operand %d of instruction \"%s\" should be of type \"%s\", is \"%s\"!" % (self.operand,
                                                                                          self.instruction,
                                                                                          self.should,
                                                                                          self.type)
