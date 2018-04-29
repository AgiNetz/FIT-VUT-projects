from errhandle import *
import re
from Instruction import Instruction
from Frame import Frame


class Program:
    """Class representing a single module of the interpreted application."""

    def __init__(self, code):
        self.labels = {}  # Dictionary mapping label name to the index in the instruction list
        self.frame_stack = []  # Stack(list) with containing local variable frames
        self.tmp_frame = None  # Temporary variable frame
        self.global_frame = Frame()  # Global variable frame
        self.data_stack = []  # Data stack containing values during program interpretation
        self.call_stack = []  # Stack of return addresses for CALL/RETURN instruction pairs
        self.instructions = []  # A list of instructions
        self.next_instruction = 0  # Next instruction to execute

        # XML structure validation
        if code.tag != "program":  # Root element check
            raise BadXMLError("Root element needs to be \"program\"!")

        for at in code.attrib:  # Attribute values check
            if not re.match(r'(language|name|description)', at):
                raise BadXMLError("Invalid element %s" % at)

        if code.attrib.get("language", None) != "IPPcode18":  # Language attribute validation
            raise BadXMLError("Invalid or missing \"language\" attribute!")

        # Convert XML instructions into internal representation
        for instr in code:
            self.instructions.append(Instruction(instr))

        # Sort instructions based on their order - prepare for in-order execution
        self.instructions.sort(key=lambda i: i.order, reverse=False)

        # Check for invalid instruction orders
        if len(self.instructions) > 0 and self.instructions[0].order < 1:
            raise BadXMLError("Instruction order must be a positive!")

    def map_labels(self):
        """Method for mapping label strings to their position in the instruction list"""
        self.labels.clear()
        for i in range(0, len(self.instructions)):
            if self.instructions[i].opcode == "LABEL":
                label_name = self.instructions[i].args[0].value
                if label_name in self.labels.keys():
                    raise SemanticError("Duplicate label \"%s\"!" % label_name)
                self.labels[label_name] = i

    def run(self):
        """Executes all instructions of the program"""
        # Data clean up and preparation
        self.frame_stack = []
        self.tmp_frame = None
        self.global_frame = Frame()
        self.data_stack = []
        self.call_stack = []
        self.next_instruction = 0
        self.map_labels()
        # Run all instructions
        while self.next_instruction != len(self.instructions):
            self.instructions[self.next_instruction].execute(self)
            self.next_instruction += 1
