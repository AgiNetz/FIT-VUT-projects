from errhandle import *
from Argument import Argument
from InstructionSignature import InstructionSignature
from Frame import Frame
from Value import Value
import sys


class Instruction:

    def __init__(self, instr):
        # Instruction tag parsing
        if instr.tag != "instruction":
            raise BadXMLError("Invalid instruction element!")

        # Instruction attributes parsing
        for at in instr.attrib:
            if at == "opcode":
                self.opcode = instr.attrib[at]  # Instruction opcode
            elif at == "order":
                try:
                    self.order = int(instr.attrib[at])  # Instruction order
                except ValueError:
                    raise BadXMLError("Instruction order must be a number!")
            else:
                raise BadXMLError("Invalid instruction attribute %s" % at)

        # Opcode and order attributes are mandatory
        if not self.opcode:
            raise BadXMLError("Instruction opcode missing!")
        elif not self.order:
            raise BadXMLError("Instruction order missing!")

        # Convert instruction arguments into internal representation
        self.args = []  # Instruction arguments
        for arg in instr:
            self.args.append(Argument(arg))

        # Sort instruction arguments based on their order number
        self.args.sort(key=lambda a: a.order, reverse=False)

        # Check invalid argument numbers
        if len(self.args) > 0 and self.args[0].order < 1:
            raise BadXMLError("Argument order must start from 1!")

        # Check if argument order numbers are contiguous from 1 to N
        for i in range(1, len(self.args) + 1):
            if self.args[i - 1].order < i:
                raise BadXMLError("Duplicate argument %d!" % i)
            elif self.args[i - 1].order > i:
                raise BadXMLError("Missing argument %d!" % i)

        # Retrieve instruction signature based on opcode, also checks opcode validity
        try:
            ins_sig = self.instr_sigs[self.opcode]
        except KeyError:
            raise BadXMLError("Invalid instruction opcode!")

        # Check instruction signature correctness
        if ins_sig.arg_count != len(self.args):
            raise BadXMLError("Instruction %s order %d has incorrect number of arguments!" % (self.opcode, self.order))

        for i in range(0, len(self.args)):
            if not self.args[i].is_of_type(ins_sig.arg_types[i]):
                raise BadXMLError("Instruction %s order %d: Argument %d has incorrect type!"
                                  " Should be %s, is %s" % (self.opcode, self.order, i, ins_sig.arg_types[i],
                                                            self.args[i].type))

    def execute(self, program):
        self.instr_sigs[self.opcode].implementation(self, program)

    def _move(self, program):
        val = self._get_val_from_arg(program, self.args[1])
        res = Value(val.value, val.type)
        self._set_var(program, self.args[0].value, res)

    def _createframe(self, program):
        program.tmp_frame = Frame()

    def _pushframe(self, program):
        if not program.tmp_frame:
            raise FrameError("Temporary frame doesn't exist!")
        program.frame_stack.append(program.tmp_frame)
        program.tmp_frame = None

    def _popframe(self, program):
        try:
            program.tmp_frame = program.frame_stack.pop()
        except IndexError:
            raise FrameError("Local frame stack is empty!")

    def _defvar(self, program):
        self._add_var(program, self.args[0].value)

    def _call(self, program):
        program.call_stack.append(program.next_instruction)
        try:
            program.next_instruction = program.labels[self.args[0].value]-1
        except KeyError:
            raise SemanticError("Label %s doesn't exist!" % self.args[0].value)

    def _return(self, program):
        try:
            program.next_instruction = program.call_stack.pop()
        except BaseException:
            raise MissingValueError("Call stack")

    def _pushs(self, program):
        val = self._get_val_from_arg(program, self.args[0])
        program.data_stack.append(val)

    def _pops(self, program):
        if not program.data_stack:
            raise MissingValueError("Data stack")
        self._set_var(program, self.args[0].value, program.data_stack.pop())

    def _add(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != "int":
            raise OperandTypeError(1, "ADD", val1.type, "int")
        elif val2.type != "int":
            raise OperandTypeError(2, "ADD", val2.type, "int")
        res = Value(val1.value + val2.value, val1.type)
        self._set_var(program, self.args[0].value, res)

    def _sub(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != "int":
            raise OperandTypeError(1, "SUB", val1.type, "int")
        elif val2.type != "int":
            raise OperandTypeError(2, "SUB", val2.type, "int")
        res = Value(val1.value - val2.value, val1.type)
        self._set_var(program, self.args[0].value, res)

    def _mul(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != "int":
            raise OperandTypeError(1, "MUL", val1.type, "int")
        elif val2.type != "int":
            raise OperandTypeError(2, "MUL", val2.type, "int")
        res = Value(val1.value * val2.value, val1.type)
        self._set_var(program, self.args[0].value, res)

    def _idiv(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != "int":
            raise OperandTypeError(1, "MUL", val1.type, "int")
        elif val2.type != "int":
            raise OperandTypeError(2, "MUL", val2.type, "int")
        if val2.value == 0:
            raise ZeroDivideError()
        res = Value(val1.value // val2.value, val1.type)
        self._set_var(program, self.args[0].value, res)

    def _lt(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != val2.type:
            raise OperandTypeError(2, "LT", val2.type, val1.type)
        res = Value(val1.value < val2.value, "bool")
        self._set_var(program, self.args[0].value, res)

    def _gt(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != val2.type:
            raise OperandTypeError(2, "GT", val2.type, val1.type)
        res = Value(val1.value > val2.value, "bool")
        self._set_var(program, self.args[0].value, res)

    def _eq(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != val2.type:
            raise OperandTypeError(2, "EQ", val2.type, val1.type)
        res = Value(val1.value == val2.value, "bool")
        self._set_var(program, self.args[0].value, res)

    def _and(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != "bool":
            raise OperandTypeError(1, "AND", val1.type, "bool")
        elif val2.type != "bool":
            raise OperandTypeError(2, "AND", val2.type, "bool")
        res = Value(val1.value and val2.value, val1.type)
        self._set_var(program, self.args[0].value, res)

    def _or(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != "bool":
            raise OperandTypeError(1, "OR", val1.type, "bool")
        elif val2.type != "bool":
            raise OperandTypeError(2, "OR", val2.type, "bool")
        res = Value(val1.value or val2.value, val1.type)
        self._set_var(program, self.args[0].value, res)

    def _not(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        if val1.type != "bool":
            raise OperandTypeError(1, "OR", val1.type, "bool")
        res = Value(not val1.value, val1.type)
        self._set_var(program, self.args[0].value, res)

    def _int2char(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        if val1.type != "int":
            raise OperandTypeError(1, "INT2CHAR", val1.type, "int")
        try:
            res = Value(chr(val1.value), "string")
        except ValueError:
            raise StringError("%d doesn't represent a Unicode character!" % val1.value)
        self._set_var(program, self.args[0].value, res)

    def _stri2int(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != "string":
            raise OperandTypeError(1, "STRI2INT", val1.type, "string")
        elif val2.type != "int":
            raise OperandTypeError(2, "STRI2INT", val2.type, "int")
        try:
            res = Value(ord(val1.value[val2.value]), "int")
        except IndexError:
            raise StringError("STRI2INT: Index out of range!")
        self._set_var(program, self.args[0].value, res)

    def _read(self, program):
        read_val = input()
        if self.args[1].value == "int":
            try:
                val = int(read_val)
            except ValueError:
                val = 0
        elif self.args[1].value == "bool":
            read_val = read_val.lower()
            if read_val == "true":
                val = True
            else:
                val = False
        else:
            val = read_val
        res = Value(val, self.args[1].value)
        self._set_var(program, self.args[0].value, res)

    def _write(self, program):
        val = self._get_val_from_arg(program, self.args[0])
        if val.type == "bool":
            print("true" if val.value is True else "false")
        else:
            print(val.value)

    def _concat(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != "string":
            raise OperandTypeError(1, "CONCAT", val1.type, "string")
        elif val2.type != "string":
            raise OperandTypeError(2, "CONCAT", val2.type, "string")
        res = Value(val1.value + val2.value, val1.type)
        self._set_var(program, self.args[0].value, res)

    def _strlen(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        if val1.type != "string":
            raise OperandTypeError(1, "STRLEN", val1.type, "string")
        res = Value(len(val1.value), "int")
        self._set_var(program, self.args[0].value, res)

    def _getchar(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != "string":
            raise OperandTypeError(1, "GETCHAR", val1.type, "string")
        elif val2.type != "int":
            raise OperandTypeError(2, "GETCHAR", val2.type, "int")
        try:
            res = Value(val1.value[val2.value], "string")
        except IndexError:
            raise StringError("GETCHAR: Index out of range!")
        self._set_var(program, self.args[0].value, res)

    def _setchar(self, program):
        val0 = self._get_val_from_arg(program, self.args[0])
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val0.type != "string":
            raise OperandTypeError(1, "SETCHAR", val1.type, "string")
        if val1.type != "int":
            raise OperandTypeError(1, "SETCHAR", val1.type, "int")
        elif val2.type != "string":
            raise OperandTypeError(2, "SETCHAR", val2.type, "string")
        try:
            tmp = val2.value[0]
        except IndexError:
            raise StringError("SETCHAR: Value string is empty!")
        if val1.value not in range(-len(val0.value), len(val0.value)):
            raise StringError("SETCHAR: Index out of range!")
        res = Value(val0.value[:val1.value] + tmp + val0.value[val1.value+1:], "string")
        self._set_var(program, self.args[0].value, res)

    def _type(self, program):
        try:
            val1 = self._get_val_from_arg(program, self.args[1])
            resval = val1.type
        except MissingValueError:
            resval = ""
        res = Value(resval, "string")
        self._set_var(program, self.args[0].value, res)

    def _label(self, program):
        pass

    def _jump(self, program):
        try:
            program.next_instruction = program.labels[self.args[0].value] - 1
        except KeyError:
            raise SemanticError("Label %s doesn't exist!" % self.args[0].value)

    def _jumpifeq(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != val2.type:
            raise OperandTypeError(2, "JUMPIFEQ", val2.type, val1.type)
        if val1.value == val2.value:
            try:
                program.next_instruction = program.labels[self.args[0].value] - 1
            except KeyError:
                raise SemanticError("Label %s doesn't exist!" % self.args[0].value)

    def _jumpifneq(self, program):
        val1 = self._get_val_from_arg(program, self.args[1])
        val2 = self._get_val_from_arg(program, self.args[2])
        if val1.type != val2.type:
            raise OperandTypeError(2, "JUMPIFEQ", val2.type, val1.type)
        if val1.value != val2.value:
            try:
                program.next_instruction = program.labels[self.args[0].value] - 1
            except KeyError:
                raise SemanticError("Label %s doesn't exist!" % self.args[0].value)

    def _dprint(self, program):
        val = self._get_val_from_arg(program, self.args[0])
        if val.type == "bool":
            print("true" if val.value is True else "false", file=sys.stderr)
        else:
            print(val.value, file=sys.stderr)

    def _break(self, program):
        pass

    @staticmethod
    def _add_var(program, variable):
        frame = variable[0:2]
        if frame == "TF":
            try:
                program.tmp_frame.variables[variable[3:]] = None
            except BaseException:
                raise FrameError("Temporary frame doesn't exist!")
        elif frame == "LF":
            try:
                program.frame_stack[-1].variables[variable[3:]] = None
            except BaseException:
                raise FrameError("Local frame stack is empty!")
        elif frame == "GF":
            program.global_frame.variables[variable[3:]] = None

    @staticmethod
    def _get_var(program, variable):
        frame = variable[0:2]
        if frame == "TF":
            try:
                return program.tmp_frame.variables[variable[3:]]
            except KeyError:
                raise BadVariableError(variable)
            except BaseException:
                raise FrameError("Temporary frame doesn't exist!")
        elif frame == "LF":
            try:
                return program.frame_stack[-1].variables[variable[3:]]
            except KeyError:
                raise BadVariableError(variable)
            except BaseException:
                raise FrameError("Local frame stack is empty!")
        elif frame == "GF":
            try:
                return program.global_frame.variables[variable[3:]]
            except KeyError:
                raise BadVariableError(variable)

    @staticmethod
    def _set_var(program, variable, value):
        frame = variable[0:2]
        if frame == "TF":
            if not program.tmp_frame:
                raise FrameError("Temporary frame doesn't exist!")
            if not (variable[3:] in program.tmp_frame.variables.keys()):
                raise BadVariableError(variable)
            program.tmp_frame.variables[variable[3:]] = value
        elif frame == "LF":
            if not program.frame_stack:
                raise FrameError("Local frame stack is empty!")
            if not (variable[3:] in program.frame_stack[-1].variables.keys()):
                raise BadVariableError(variable)
            program.frame_stack[-1].variables[variable[3:]] = value
        elif frame == "GF":
            if not (variable[3:] in program.global_frame.variables.keys()):
                raise BadVariableError(variable)
            program.global_frame.variables[variable[3:]] = value

    @staticmethod
    def _get_val_from_arg(program, arg):
        if arg.is_of_type("var"):
            var_val = Instruction._get_var(program, arg.value)
            if not var_val:
                raise MissingValueError("Variable %s" % arg.value)
            return var_val
        else:
            return Value(arg.value, arg.type)

    # Dictionary of instruction signatures and their implementations
    instr_sigs = {"MOVE": InstructionSignature(2, ["var", "symbol"], _move),
                  "CREATEFRAME": InstructionSignature(0, [], _createframe),
                  "PUSHFRAME": InstructionSignature(0, [], _pushframe),
                  "POPFRAME": InstructionSignature(0, [], _popframe),
                  "DEFVAR": InstructionSignature(1, ["var"], _defvar),
                  "CALL": InstructionSignature(1, ["label"], _call),
                  "RETURN": InstructionSignature(0, [], _return),
                  "PUSHS": InstructionSignature(1, ["symbol"], _pushs),
                  "POPS": InstructionSignature(1, ["var"], _pops),
                  "ADD": InstructionSignature(3, ["var", "symbol", "symbol"], _add),
                  "SUB": InstructionSignature(3, ["var", "symbol", "symbol"], _sub),
                  "MUL": InstructionSignature(3, ["var", "symbol", "symbol"], _mul),
                  "IDIV": InstructionSignature(3, ["var", "symbol", "symbol"], _idiv),
                  "LT": InstructionSignature(3, ["var", "symbol", "symbol"], _lt),
                  "GT": InstructionSignature(3, ["var", "symbol", "symbol"], _gt),
                  "EQ": InstructionSignature(3, ["var", "symbol", "symbol"], _eq),
                  "AND": InstructionSignature(3, ["var", "symbol", "symbol"], _and),
                  "OR": InstructionSignature(3, ["var", "symbol", "symbol"], _or),
                  "NOT": InstructionSignature(2, ["var", "symbol"], _not),
                  "INT2CHAR": InstructionSignature(2, ["var", "symbol"], _int2char),
                  "STRI2INT": InstructionSignature(3, ["var", "symbol", "symbol"], _stri2int),
                  "READ": InstructionSignature(2, ["var", "type"], _read),
                  "WRITE": InstructionSignature(1, ["symbol"], _write),
                  "CONCAT": InstructionSignature(3, ["var", "symbol", "symbol"], _concat),
                  "STRLEN": InstructionSignature(2, ["var", "symbol"], _strlen),
                  "GETCHAR": InstructionSignature(3, ["var", "symbol", "symbol"], _getchar),
                  "SETCHAR": InstructionSignature(3, ["var", "symbol", "symbol"], _setchar),
                  "TYPE": InstructionSignature(2, ["var", "symbol"], _type),
                  "LABEL": InstructionSignature(1, ["label"], _label),
                  "JUMP": InstructionSignature(1, ["label"], _jump),
                  "JUMPIFEQ": InstructionSignature(3, ["label", "symbol", "symbol"], _jumpifeq),
                  "JUMPIFNEQ": InstructionSignature(3, ["label", "symbol", "symbol"], _jumpifneq),
                  "DPRINT": InstructionSignature(1, ["symbol"], _dprint),
                  "BREAK": InstructionSignature(0, [], _break)}
