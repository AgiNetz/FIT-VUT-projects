from errhandle import *
import re
import codecs


class Argument:
    """Class representing an argument of an instruction in IPPcode18"""

    # Regex expressions for checking lexical correctness of individual argument types
    type_forms = {"string": r"(\\[0-9]{3}|[^[:cntrl:]\s#\\])*",
                  "int": r"(\+|\-){0,1}[0-9]+",
                  "bool": r"(true|false)",
                  "var": r"[TLG]F@([a-zA-Z_\-\$&%\*][0-9a-zA-Z_\-\$&%\*]*)",
                  "type": r"(int|bool|string)",
                  "label": r"([a-zA-Z_\-\$&%\*][0-9a-zA-Z_\-\$&%\*]*)"}

    def __init__(self, arg):
        # Argument tag parsing
        if arg.tag[0:3] != "arg":  # Argument tag check
            raise BadXMLError("Instruction elements must be arguments! Found \"%s\"" % arg.tag)
        try:
            self.order = int(arg.tag[3::])  # Parse argument order
        except ValueError:
            raise BadXMLError("Argument elements must be in the form \"arg[1|2|3]\"!")

        # Argument attributes parsing
        for at in arg.attrib:
            if at == "type":
                self.type = arg.attrib[at]  # Argument type
            else:
                raise BadXMLError("Invalid argument attribute \"%s\"!" % at)

        if not self.type:  # Type attribute is mandatory
            raise BadXMLError("Argument type missing!")

        # Argument value check and parse
        if arg.text:
            self.value = arg.text  # Argument value
        else:
            self.value = ""
        try:
            if not self._check_arg():
                raise LexError("Argument value \"%s\" is not of type \"%s\"" % (self.value, self.type))
        except KeyError:
            raise BadXMLError("Unrecognized argument type!")

        # Convert value to correct type for future use
        if self.type == "int":
            self.value = int(arg.text)
        elif self.type == "bool":
            self.value = True if arg.text == "true" else False
        elif self.type == "string":
            pos = 0
            escape = self.value.find("\\", pos)
            prev_esc = -1
            while escape >= 0:
                if prev_esc == escape:
                    pos += 1
                else:
                    esc_seq = chr(int(self.value[escape+1:escape+4]))
                    self.value = self.value[:escape] + esc_seq + self.value[escape+4:]
                    prev_esc = escape
                escape = self.value.find("\\", pos)

    def _check_arg(self):
        """Checks if the value of the argument is lexically correct based on its type"""
        return re.match(self.__class__.type_forms[self.type], str(self.value))

    def is_of_type(self, t):
        """Checks the argument \"type group\""""
        t = t.lower()
        if t == self.type:
            return True
        if t == "symbol":
            return self.type == "int" or self.type == "string" or self.type == "bool" or self.type == "var"
        return False


