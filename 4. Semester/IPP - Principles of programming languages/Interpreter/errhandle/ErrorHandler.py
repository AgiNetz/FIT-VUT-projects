import sys

# Class name to error code mapping dictionary
err_codes = {'ArgError': 10,
             'FileInputError': 11,
             'FileOutputError:': 12,
             'BadXMLError': 31,
             'LexError': 32,
             'SemanticError': 52,
             'OperandTypeError': 53,
             'BadVariableError': 54,
             'FrameError': 55,
             'MissingValueError': 56,
             'ZeroDivideError': 57,
             'StringError': 58,
             'InternalError': 99}
unknown_error = 99


def handle(e):
    """Prints the error message contained in the exception and exits with an appropriate return code
    based on the exception class type"""
    if isinstance(e, BaseException):
        print(e, file=sys.stderr)
        exit(err_codes.get(e.__class__.__name__, unknown_error))
