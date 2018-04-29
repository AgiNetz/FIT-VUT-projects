## IPP Projects
The projects from IPP are separate modules of an interpreter for an unstructurued language "IPPcode18".  
It consists of the following parts:
  * Parsing module
  * Interpreter
  * Script for automatic testing
  
# Parsing module
Written in PHP5.6. It parses an IPPcode18 program, checks its lexical and syntactic correctness  
and transforms it into an XML representation.

# Interpreter
Written in Python 3. Takes the XML representation of an IPPcode18 program and interprets it.

# Script for automatic testing
Written in PHP5.6. Scans a given directory and runs tests of previous 2 modules. Outputs the result summary in html format.
