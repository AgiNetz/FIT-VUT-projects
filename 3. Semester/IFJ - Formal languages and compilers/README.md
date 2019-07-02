# Formal languages and compilers project - Compiler of a structured language into a 3-address code

The goal of the project was to build a compiler that compiles the language IFJ17, which is based on the FreeBASIC language (https://www.freebasic.net/) and transforms the source code into a custom 3-address code. The project was implemented in a 4-man team in C programming languages without any 3rd party libraries. Therefore, we had to implement all data structures we wanted to use ourselves. The project consists of several modules:
- Scanner - Performs lexical analysis/validation and tokenizes the source code
- Parser - Performs top-down syntactic and semantic analysis, bottom-up expression analysis and calls on the generator
- Generator - generates the target 3 address code, manages variable names and other target-code specific details
- Data structures - Implementation of a hash table for symbols, stack for expression analysis and other auxiliary data structures
