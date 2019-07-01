import argparse
from Program import Program
from errhandle import *
import xml.etree.ElementTree as ElTree


def parse_args():
    """Parse the arguments for the application and return a dictionary containing their values"""

    arg_parser = argparse.ArgumentParser(description="An interpreter for IPPcode18 language. Takes "
                                                     "an XML document containing a representation of an IPPcode18 "
                                                     "program and executes it.")
    arg_parser.add_argument("--source", help="Source file to read an IPP18code program from", metavar="FILE",
                            required=True)
    try:
        args = arg_parser.parse_args()
    except SystemExit as e:
        if e.code == 0:
            exit(0)
        else:
            raise ArgError()
    return args


def parse_xml(file):
    """Parse XML input from a given file and return its representation as the root of its ElementTree"""
    try:
        code = ElTree.parse(file)
    except FileNotFoundError as err:
        raise FileInputError(err.filename, err.strerror)
    except ElTree.ParseError as err:
        raise BadXMLError(err.msg)
    return code.getroot()


# Argument parsing
try:
    int_args = parse_args()

# Parse the XML input and convert it into an internal representation
    input_code = parse_xml(int_args.source)
    program = Program(input_code)

# Interpret the code
    program.run()
except SystemExit as e:
    if e.code == 0:
        exit(0)
    else:
        ErrorHandler.handle(e)
except BaseException as e:
    ErrorHandler.handle(e)


