/**
 * @file scanner.h
 * @brief Lexical analyzer module for IFJ Project 2017 AT vut.fit.vutbr.cz.
 * @date 21.10.2017, last rev. 26.11.2017
 * @author Goldschmidt Patrik - xgolds00@stud.fit.vutbr.cz
 * @author Psenak Kamil       - xpsena02@stud.fit.vutbr.cz
 *
 * Lexical analyzer source file (scanner).  Module communicates with Syntactic
 * analyzer (parser), reading the input file and passing tokens to this parser.
 * Its operation is done on per call basis, when parser calls get_next_token()
 * function and scanner returns new token (or LEX_ERR) every time.  Module
 * recognizes keywords from identifiers using is_keyword() function, which uses
 * pseudo-binary search tree to find out the keyword faster.
 *
 * Header file provides definition of various keywords and deterministic finite
 * automaton states.  Every lexeme type has its own identifier (number), which
 * is used to fill the token information, so parser is able to recognize what
 * token did scanner read.  Header also provides a scanner interface for calls
 * from outside this module.
 */

#ifndef SCANNER_H
#define SCANNER_H 1

#include "ifj17_api.h"
#include <stdio.h>

/* Identifies an empty buffer.   */
#define BUF_EMPTY -10 

/*  Identifies function output no keyword is found.  */
#define NO_KEYWORD_MATCHED 0

/*  Error value when LEX_ERR occurs.  */
#define LEX_ERR_TOK_VAL -2

/*	Token type definitions.		*/
#define AND_ID 2
#define AS_ID 3
#define ASC_ID 4
#define BOOLEAN_ID 5
#define CHR_ID 6
#define CONTINUE_ID 7
#define DECLARE_ID 8
#define DIM_ID 9
#define DO_ID 10
#define DOUBLE_ID 11
#define ELSE_ID 12
#define ELSEIF_ID 13
#define END_ID 14
#define EXIT_ID 15
#define FALSE_ID 16
#define FOR_ID 17
#define FUNCTION_ID 18
#define IF_ID 19
#define INPUT_ID 20
#define INTEGER_ID 21
#define LENGTH_ID 22
#define LOOP_ID 23
#define NEXT_ID 24
#define NOT_ID 25
#define OR_ID 26
#define PRINT_ID 27
#define RETURN_ID 28
#define SCOPE_ID 29
#define SHARED_ID 30
#define STATIC_ID 31
#define STRING_ID 32
#define SUBSTR_ID 33
#define THEN_ID 34
#define TRUE_ID 35
#define WHILE_ID 36
#define AS "as"
#define ASC "asc"
#define DECLARE "declare"
#define DIM "dim"
#define DO "do"
#define DOUBLE "double"
#define ELSE "else"
#define END "end"
#define CHR "chr"
#define FUNCTION "function"
#define IF "if"
#define INPUT "input"
#define INTEGER "integer"
#define LENGTH "length"
#define LOOP "loop"
#define PRINT "print"
#define RETURN "return"
#define SCOPE "scope"
#define STRING "string"
#define SUBSTR "substr"
#define THEN "then"
#define WHILE "while"
#define AND "and"
#define BOOLEANN "boolean"
#define CONTINUE "continue"
#define ELSEIF "elseif"
#define EXIT "exit"
#define FALSEE "false"
#define FOR "for"
#define NEXT "next"
#define NOT "not"
#define OR "or"
#define SHARED "shared"
#define STATIC "static"
#define TRUEE "true"
#define TO "to"
#define STEP "step"
#define UNTIL "until"
#define LEFT_BRACKET_ID 40
#define RIGHT_BRACKET_ID 41
#define ASTERISK_ID 42
#define SLASH_ID 43
#define PLUS_ID 44
#define MINUS_ID 45
#define BACKSLASH_ID 46
#define EQUALS_ID 47
#define GREATER_EQUAL_ID 48
#define LESS_EQUAL_ID 49
#define LESS_ID 50
#define GREATER_ID 51
#define NOT_EQUAL_ID 52
#define STRING_LITERAL_ID 53
#define COMMA_ID 54
#define SEMICOLON_ID 55
#define INTEGER_LITERAL_ID 56
#define DOUBLE_LITERAL_ID 57
#define TO_ID 58
#define UNTIL_ID 59
#define STEP_ID 60
#define ID_ID 61
#define EOL_ID 62
#define EXPR_END_ID 99

#define EOL '\n'

/* States of finite automaton.   */
enum fsm_states {
   /* Fundamental states.  */
   def                = 100, ///< Default state.
   error,                    ///< Reads chars until next WS and returns error.
   identifier,               ///< Determines if str is a keyword or identifier.
   /* Number literals states.    */
   num_integer,              ///< Loads numbers until 'e' or '.'.
   num_double_dot,           ///< Checks if the next character is a digit.
   num_double_dot_digit,     ///< Loads numbers, until !number.
   num_double_e,             ///< Checks sign or digit after 'e' receipt.
   num_double_e_sign,        ///< Expects a digit to validate 'e' type number.
   num_double_e_digit,       ///< Finishes a number with an exponent.
   /* Extension BASE states.     */
   amp,                      ///< Character '&' was received previously.
   num_bin,                  ///< Reads bin. number and converts it to an int.
   num_bin_zero,             ///< Reads binary, if receive anything else ret 0.
   num_oct,                  ///< Reads oct. number and converts in to an int.
   num_oct_zero,             ///< Reads octal, if receive anything else ret. 0.
   num_hex,                  ///< Reads hex. number and converts in to an int.
   num_hex_zero,             ///< Reads hexa, if receive anything else ret. 0.
   /* Comments states.           */
   comment_sl,               ///< Single line comment. Ignores chars until EOL.
   comment_ml_check_start,   ///< Checks for '\'' to confirm comment start.
   comment_ml_inside,        ///< Ignores chars until end of comment is found.
   comment_ml_check_end,     ///< Checks for the end of the comment.
   /* String literals states.    */
   str_check_start,          ///< Checks for start of the string.
   str_inside,               ///< Adds chars to the string.
   str_esc_seq_x,            ///< Checks for escape sequence in the string.
   str_esc_seq_xy_any,       ///< 2nd digit in the escape seq.  Expects 0-9.
   str_esc_seq_xy_5max,      ///< 2nd digit in the escape seq.  Expects 0-5.
   str_esc_seq_xyz_any,      ///< 3rd digit in the escape seq.  Expects 0-9.
   str_esc_seq_xyz_5max,     ///< 3rd digit in the escape seq.  Expects 0-5.
   str_error,                ///< Reads string until the end, returns error.
   /* Logical operators states.  */
   logic_less,               ///< Determines logic operator after "<" receipt.
   logic_greater,            ///< Determines logic operator after ">" receipt.
};

/**
 * @brief Sets a source file for lexical analyzer functions.
 *
 * @param *f Pointer to the already opened source file.
 * @return void.
 */
void set_source_file(FILE *f);

/**
 * @brief Finds the next token in a stream of characters in the source file
 *        previously set by set_source_file() function.  Function is based on
 *        a finite state automaton implementation. 
 *
 * @param *token token_t data type which will be filled during the process.
 * @return Specific ID for the particular lexeme or LEX_ERR if the read lexeme
 *         is invalid.
 */
int get_next_token(token_t *token);

#endif
