from errhandle import *


class InstructionSignature:
    """Class representing an instruction signature - specifies argument count and types."""
    def __init__(self, arg_count, arg_types, implementation):
        self.arg_count = arg_count  # Number of arguments
        if len(arg_types) != arg_count:  # Check sanity
            raise InternalError("Bad instruction signature!")
        self.arg_types = arg_types  # List of argument types
        self.implementation = implementation  # Reference to instruction implementation
