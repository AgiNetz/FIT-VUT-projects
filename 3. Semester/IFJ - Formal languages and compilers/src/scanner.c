/**
 * @file scanner.c
 * @brief Lexical analyzer module for IFJ Project 2017 AT vut.fit.vutbr.cz.
 * @date 21.10.2017, last rev. 26.11.2017
 * @author Danis Tomas        - xdanis05@stud.fit.vutbr.cz
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
 * Source file defines deterministic finite automaton, which recognizes tokens
 * based on the currently processed character based on the state in which the
 * automaton currently is.
 */

#include "ifj17_api.h"
#include "safe_malloc.h"
#include "scanner.h"
#include "str.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

/** @brief Input file used by get_next_token() function.   */
FILE *source;

/** @brief Line counter used for error reporting.  */
int line_counter = 1;

string_t str;

/* 
 * Finds if the string is a FREEBasic language keyword. 
 * Keywords are ordered alphabetically and search works
 * using binary search tree for better performance.
 */
static int is_keyword(string_t *str);

int get_next_token(token_t *token)
{
   static int buffer = BUF_EMPTY;           // Buffer chars until next call
   int c = 0;                               // Loaded character
   int state = def;                         // State of the finite automaton
   char *string_data;                       // Pointer to the dynamic str. data
   bool quit = false;                       // Function will return on true

   /* Prepare dynamically sized-string.   */
   str_clear(&str);
   
   while(c != EOF)
   {

      /* Update loaded string during the reading process.   */
      string_data = str_get_data(&str);
      
      /*
       * When the local buffer is empty, get the next character, otherwise pass
       * the character from the buffer to 'c'.
       */
      if(buffer == BUF_EMPTY)
      {
         c = fgetc(source);
      }
      else
      {
         c = buffer;
         buffer = BUF_EMPTY;
      }

      /*  Lower characters if the FSA is not processing a string. */
      if(state != str_inside && state != str_esc_seq_x && state != str_error &&
         state != str_esc_seq_xy_any && state != str_esc_seq_xy_5max &&
         state != str_esc_seq_xyz_any && state != str_esc_seq_xyz_5max)
      {
         c = tolower(c);
      }

      /*
       * Increment line counter if the loaded char is an EOL and automaton is
       * currently not in the error state due to correct error reporting.
       */
      if(c == EOL && state != error && state != str_error)
         line_counter++;

      /* The finite automaton implementation. */
      switch(state)
      {
      /* 
       * Default state of the FA, we ignore whitespaces, return lexemes which
       * are only 1 character long right away and switch to states depending
       * on the first lexeme.  In most cases without returning, program simply
       * stays in this state until not specified otherwise.
       */

      case def:
         /* Ignore whitespaces, react on EOL.  */
         if(isspace(c))
         {
            /* If the "space" character is a newline. */
            if(c == EOL)
            {
               token->id = EOL_ID;
               quit = true;
            }
         }
         /* Receive alpha-type character or '_' - switch to ID state.  */
         else if(isalpha(c) || c == '_')
         {
            str_add_char(&str, c);
            state = identifier;
         }
         /* Receive number - switch to number processing state.   */
         else if(isdigit(c))
         {
            str_add_char(&str, c);
            state = num_integer;
         }
         /* Asterisk received and returned.   */
         else if(c == '*')
         {
            token->id = ASTERISK_ID;
            quit = true;
         }
         /* Slash received - comment or slash alone -> return.   */
         else if(c == '/')
         {
            state = comment_ml_check_start; 
         }
         /* Back slash received - return it. */
         else if(c == '\\')
         {
            token->id = BACKSLASH_ID;
            quit = true;
         }
         /* Plus symbol received - return it.   */
         else if(c == '+')
         {
            token->id = PLUS_ID;
            quit = true;
         }
         /* Minus symbol received - return it.  */
         else if(c == '-')
         {
            token->id = MINUS_ID;
            quit = true;
         }
         /* Less symbol received.   */
         else if(c == '<')
         {
            state = logic_less;
         }
         /* Greater symbol received.   */
         else if(c == '>')
         {
            state = logic_greater;
         }
         /* Equal symbol received.  */
         else if(c == '=')
         {
            token->id = EQUALS_ID;
            quit = true;
         }
         /* Left bracket symbol received. */
         else if(c == '(')
         {
            token->id = LEFT_BRACKET_ID;
            quit = true;
         }
         /* Right bracket symbol received.   */
         else if(c == ')')
         {
            token->id = RIGHT_BRACKET_ID;
            quit = true;
         }
         /* Apostrophe symbol received.  */
         else if(c == '\'')
         {
            state = comment_sl;
         }
         /* Exclamation mark received. */
         else if(c == '!')
         {
            state = str_check_start;
         }
         /* Ampersand received.  */
         else if(c == '&')
         {
            state = amp;
         }
         /* Semicolon received.  */
         else if(c == ';')
         {
            token->id = SEMICOLON_ID;
            quit = true;
         }
         /* Comma received.   */
         else if(c == ',')
         {
            token->id = COMMA_ID;
            quit = true;
         }
         else if(c == EOF)
         {
          token->id = EOF;
          quit = true;
         }
         else
            state = error;

         break;

      /* ------------------------------------------------------------------- */

      /*
       * Global error state. Adds characters to the string until next
       * whitespace occurrence.  On whitespace, discard it, fill in the return
       * token and return lexical error.
       */
      case error:
         if(!(isspace(c)))
         {
            if(c == EOF)
               str_append_cstring(&str, "\'EOF\'");
            else
               str_add_char(&str, c);
         }
         else
         {
            token->id = LEX_ERR_TOK_VAL;
            token->a.str = str_copy_to_cstring(&str);
            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * Got a start of a identifier already, now continue assembling the 
       * identifier.   Once the process is done, compare the identifier
       * with pre-defined keywords. Return based on the result.
       */
      case identifier:
         /* If received character is alphanumeric or '_', still in the id. */
         if(isalnum(c) || c == '_')
         {
            str_add_char(&str, c);
         }
         /* Received other character, end of the identifier was reached.   */
         else
         {
            /* Find out if read token is identifier or keyword.   */
            int id = is_keyword(&str);
            buffer = c;

            /* Check if the processed token is keyword or identifier.   */
            if(id == NO_KEYWORD_MATCHED)
            {
               /* No keyword was found -> token is an identifier. */
               token->a.str = str_copy_to_cstring(&str);
               token->id = ID_ID;
            }
            else
               /* Token is a keyword.  */
               token->id = id;

            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * In this case, program has already received a number.  This number is
       * either whole number or a floating point number.  Decide where to 
       * continue.
       * In this state a, following conditions may occur:
       * - Number is received       - stay in this state.
       * - Dot ('.') is received    - proceed to the double_dot state.
       * - Symbol 'e' is received   - proceed to the double_e state.
       * - Other symbol is received - end of number -> return.
       */
      case num_integer:
         if(isdigit(c))
         {
            /* Received digit - add to string and stay in this state.   */
            str_add_char(&str, c);
         }
         else if(c == '.')
         {
            state = num_double_dot;
            str_add_char(&str, c);
         }
         else if(c == 'e')
         {
            state = num_double_e;
            str_add_char(&str, c);
         }
         else
         {
            /* Did not receive expected value - end of the number reached. */
            int value = strtol(string_data, NULL, 10);

            token->a.val_int = value;
            token->id = INTEGER_LITERAL_ID;
            buffer = c;

            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * In this case, number and dot was received.  The state expects only
       * another number to confirm validity of floating point number input.
       */
      case num_double_dot:
         if(isdigit(c))
         {
            str_add_char(&str, c);
            state = num_double_dot_digit; 
         }
         /* Did not receive expected decimal number - return error.  */
         else if(c == EOF)
         {
            str_append_cstring(&str, "\'EOF\'");
            state = error;
         }
         else
         {
            str_add_char(&str, c);
            state = error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * In this case, program has received correct double number in the form
       * of "digits.digits". Now finalize the number expecting more digits or
       * e/E symbol.
      */
      case num_double_dot_digit:
         if(isdigit(c))
         {
            str_add_char(&str, c);
         }
         else if(c == 'e')
         {
            str_add_char(&str, c);
            /* When 'e'/'E' is received check the next character */
            state = num_double_e;
         }
         else
         {
            /* End of the number was reached, return its value.   */
            double fp_num;
            sscanf(str_get_data(&str), "%lg", &fp_num);
            token->a.val_real = fp_num;
            token->id = DOUBLE_LITERAL_ID;
            buffer = c;

            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * Program has previously received a number and an "e" symbol.  At this
       * state, check what comes after an "e" symbol to confirm validity of a
       * floating point number.
       */
      case num_double_e:

         if(c == '+' || c == '-')
         {
            str_add_char(&str, c);
            state = num_double_e_sign;
         }
         else if(isdigit(c))
         {
            str_add_char(&str, c);
            state = num_double_e_digit;
         }
         /* Unexpected value after 'e'/'E' -> switch to error state. */
         else if(c == EOF)
         {
            str_append_cstring(&str, "\'EOF\'");
            state = error;
         }
         else
         {
            str_add_char(&str, c);
            state = error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * Received number is already in the scientific notation using 'e' and
       * '+' or '-' sign.  Now, just add at least one digit to ensure validity
       * of the input number.  Otherwise, switch to the error state.
       */
      case num_double_e_sign:
         if(isdigit(c))
         {
            str_add_char(&str, c);
            state = num_double_e_digit;
         }
         /* Did not receive a digit. Switch to the error state.  */
         else if(c == EOF)
         {
            str_append_cstring(&str, "\'EOF\'");
            state = error;
         }
         else
         {
            str_add_char(&str, c);
            state = error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * Program has already received number containing scientific 'e' notation
       * and at least one other number.  In this state, keep adding more
       * digits until something else is found.  This is the end of the number.
       */
      case num_double_e_digit:
         if(isdigit(c))
         {
            /* Received digit - stay in this state and build up the number.  */
            str_add_char(&str, c);
         }
         else
         {
            /* Other character than digit received - finish the number. */
            double fp_num;
            sscanf(str_get_data(&str), "%lg", &fp_num);
            token->a.val_real = fp_num;
            token->id = DOUBLE_LITERAL_ID;
            buffer = c;

            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * EXTENTION: BASE
       * When the ampersand symbol is received, number expressed by either
       * octal, binary or hexadecimal is expected.  In this state, program
       * check whether one of 'b', 'o' or 'h' symbol is read right after the
       * ampersand symbol to ensure in which notation the number is.  If
       * non-expected symbol is received, automaton switches to error state.
       */
      case amp:
         if(c == 'b')
            /* Received 'b' - switch to pre-binary state.   */
            state = num_bin_zero;
         else if(c == 'o')
            /* Received 'o' - switch to pre-octal state.   */
            state = num_oct_zero;
         else if(c == 'h')
            /* Received 'h' - switch to pre-hexa state.  */
            state = num_hex_zero;
         else
         {
            /* 
             * Received something else = not correct form of a number, switch
             * to the error state.
             */
            str_add_char(&str, '&');

            if(c == EOF)
               str_append_cstring(&str, "\'EOF\'");
            else
               str_add_char(&str, c);

            state = error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * Previous input "&b".  Switch to the binary state if received valid
       * binary number or return 0 otherwise.
       */
      case num_bin_zero:
         if(c == '0' || c == '1')
         {
            str_add_char(&str, c);
            state = num_bin;
         }
         /* Not a correct form of binary number - return zero. */
         else
         {
            token->a.val_int = 0;
            token->id = INTEGER_LITERAL_ID;
            buffer = c;

            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * Previous input "&h".  Switch to the hexa state if received valid
       * hexadecimal number or return 0 otherwise.
       */
      case num_hex_zero:
         if(isdigit(c) || (c >= 'a' && c <= 'f'))
         {
            str_add_char(&str, c);
            state = num_hex;
         }
         /* Not a correct form of hexadecimal number - return zero. */
         else
         {
            token->a.val_int = 0;
            token->id = INTEGER_LITERAL_ID;
            buffer = c;

            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * Previous input "&o".  Switch to the octal state if received valid
       * octal number or return 0 otherwise.
       */
      case num_oct_zero:
         if(c >= '0' && c <= '7')
         {
            str_add_char(&str, c);
            state = num_oct;
         }
         /* Not a correct form of octal number - return zero. */
         else
         {
            token->a.val_int = 0;
            token->id = INTEGER_LITERAL_ID;
            buffer = c;

            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * Binary number state.  Add valid binary numbers into the final number.
       * If invalid binary number is received, end of the number was reached.
       */
      case num_bin:
         if(c == '0' || c == '1')
         {
            str_add_char(&str, c);
         }
         else
         {
            /* Non-binary character received - return decimal value. */
            int bin_val = strtol(string_data, NULL, 2);
            token->a.val_int = bin_val;
            token->id = INTEGER_LITERAL_ID;
            buffer = c;

            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * Hexadecimal number state.  Add valid octal numbers into the final
       * number.  If invalid octal number is received, end of the number was
       * reached.
      */
      case num_hex:
         if(isdigit(c) || (c >= 'a' && c <= 'f'))
         {
            str_add_char(&str, c);
         }
         else
         {
            /* Non-hex character received - return hexadecimal value. */
            int hex_val = strtol(string_data, NULL, 16);
            token->a.val_int = hex_val;
            token->id = INTEGER_LITERAL_ID;
            buffer = c;

            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * Octal number state.  Add valid octal numbers into the final number.
       * If invalid octal number is received, end of the number was reached.
      */
      case num_oct:
         if(c >= '0' && c <= '7')
         {
            str_add_char(&str, c);
         }
         else
         {
            /* Non-octal character received - return octal value. */
            int oct_val = strtol(string_data, NULL, 8);
            token->a.val_int = oct_val;
            token->id = INTEGER_LITERAL_ID;
            buffer = c;

            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * Single line comment.  State ignores every character until the end of
       * the line is reached.
       */
      case comment_sl:
         if(c == EOL)
         {
            /* Caller needs to be informed about EOL occurrence.   */
            token->id = EOL_ID;
            quit = true;
         }
         else if(c == EOF)
         {
            /* EOF "within" single line comment is correct. */
            token->id = EOF;
            quit = true;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * Checks if the start of the multi-line comment.  Next character needs
       * to be a '\'' to proceed to the multi-line comment state.
       */
      case comment_ml_check_start:
         if(c == '\'')
         {
            state = comment_ml_inside;
         }
         else
         {
            buffer = c;
            token->id = SLASH_ID;
            quit = true;
         }

        break;

      /* ------------------------------------------------------------------- */

      /* 
       * This state ignores every character until the possible end of the
       * comment (character apostrophe) is read.  At this point, switch to
       * another state to verify the comment end.
       */
      case comment_ml_inside:
         if(c == '\'')
            state = comment_ml_check_end;
         else if(c == EOF)
            str_append_cstring(&str, "\'EOF\'");

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * Character apostrophe was read previously in the multi-line comment.
       * Now, check for backslash occurrence to confirm end of the comment.
       * Also check for another apostrophe to stay in this state.  Otherwise,
       * switch back to the multi_line comment.
       */
      case comment_ml_check_end:
         if(c == '/')
            /* End of the comment was reached, switch to the default state.  */
            state = def;
         else if(c != '\'')
         {
            /* 
             * Everything else than apostrophe - switch back to the multi-line
             * comment state, otherwise stay here. Loaded characters are
             * buffered so we ndo not need to handle EOF checking in both
             * states.
             */
            buffer = c;
            state = comment_ml_inside;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * Program has already got '!' identifying the start of the string.  In
       * this state, ensure the string beginning is correct by checking for '"'
       * as a next symbol.
       */
      case str_check_start:
         if(c == '"')
         {
            state = str_inside;
         }
         else
         {
            /* Did not receive expected '"' symbol - switch to error state.  */
            str_add_char(&str, '!');

            if(c == EOF)
               str_append_cstring(&str, "\'EOF\'");
            else
               str_add_char(&str, c);

            state = error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * Start of the string "!"" was already processed.  Now, keep adding 
       * new characters to a string until the end of represented as '"' is
       * read.  Newline character and EOF in the string can not happen in the
       * IFJ17 language and are considered as an error.
       */
      case str_inside:
         if(c == '"')
         {
            /* End of the string is received.   */
            token->id = STRING_LITERAL_ID;
            token->a.str = str_copy_to_cstring(&str);
            quit = true;
         }
         else if(c == '\\')
         {
            /* Start of the escape sequence is received. */
            state = str_esc_seq_x;
         }
         else if(c == '\n')
         {
            str_append_cstring(&str, "\'NL\'");
            state = str_error;
         }
         else if(c < 31)
         {
            str_append_cstring(&str, "\'UNPRINTAB\'");
            state = str_error;
         }
         else
            str_add_char(&str, c);

         break;

      /* ------------------------------------------------------------------- */

      /*
       * Program has already received a slash during string processing.
       * In this state, find out what escape sequence did the user input.
       * If a digit 0, 1 or 2 follows, check if the escape seq is correct.  If
       * one of the escape seq character follows, add them to the output string
       * and return to the str_inside state.  Otherwise, switch to error state
       * when unexpected input is received.
       */
      case str_esc_seq_x:
         if(c == '0' || c == '1')
         {
            /* Received a digit 0 or 1, check validity of "\xyz". */
            str_add_char(&str, '\\');
            str_add_char(&str, c);
            state = str_esc_seq_xy_any;
         }
         else if(c == '2')
         {
            /* Received a digit 2, check validity of "\xyz". */
            str_add_char(&str, '\\');
            str_add_char(&str, c);
            state = str_esc_seq_xy_5max;
         }
         else if(c == '"')
         {
            /* Received escaped quotes character.  */
            str_add_char(&str, '\"');
            state = str_inside;
         }
         else if(c == 'n')
         {
            /* Received escaped newline character. */
            str_add_char(&str, '\n');
            state = str_inside;
         }
         else if(c == 't')
         {
            /* Received escaped horizontal tabulator character.   */
            str_add_char(&str, '\t');
            state = str_inside;
         }
         else if(c == '\\')
         {
            /* Received escaped slash character.  */
            str_add_bslash(&str);
            state = str_inside;
         }
         else if(c == EOF)
         {
            /* Received EOF - write it + switch to error - will end anyway.  */
            str_append_cstring(&str, "\'EOF\'");
            state = str_error;
         }
         else
         {
            /* Other unexpected character - error state. */
            str_add_char(&str, c);
            state = str_error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * The first character after '\' is a digit of 0 or 1.  Possible escape
       * sequences are thus "\000" - "\199".  To ensure correct input, expect
       * any digit.
       */
      case str_esc_seq_xy_any: 
         if(isdigit(c))
         {
            /* Second digit is correct. */
            str_add_char(&str, c);
            state = str_esc_seq_xyz_any;
         }
         else if(c == EOF)
         {
            /* Received EOF - write it + switch to error - will end anyway.  */
            str_append_cstring(&str, "\'EOF\'");
            state = str_error;
         }
         else
         {
            /* Other unexpected character - error state. */
            str_add_char(&str, c);
            state = str_error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * The first character after '\' is a digit of 2.  Possible escape
       * sequences are thus "\200" - "\255".  To ensure correct input, expect
       * any digit in the range of 0 to 5.
       */
      case str_esc_seq_xy_5max: 
         if(c >= '0' && c <= '4')
         {
            /* Second digit is correct. */
            str_add_char(&str, c);
            state = str_esc_seq_xyz_any;
         }
         else if(c == '5')
         {
            /* Second digit is correct. */
            str_add_char(&str, c);
            state = str_esc_seq_xyz_5max;
         }
         else if(c == EOF)
         {
            /* Received EOF - write it + switch to error - will end anyway.  */
            str_append_cstring(&str, "\'EOF\'");
            state = str_error;
         }
         else
         {
            /* Other unexpected character - error state. */
            str_add_char(&str, c);
            state = str_error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * The first character after '\' was either 0 or 1.  The second character
       * was a digit which is correct.  In this  state, check the third one
       * expecting another digit 0 to 9.  If this is correct, whole escape
       * sequence was inputted correctly.  Add it to the output string and
       * proceed back to state str_in.  Otherwise, return error.
       */
      case str_esc_seq_xyz_any:
         if(isdigit(c))
         {
            /* Fill in the escape sequence array.  */
            str_add_char(&str, c);
            state = str_inside;
         }
         else if(c == EOF)
         {
            /* Received EOF - write it + switch to error - will end anyway.  */
            str_append_cstring(&str, "\'EOF\'");
            state = str_error;
         }
         else
         {
            /* Other unexpected character - error state. */
            str_add_char(&str, c);
            state = str_error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * The first character after '\' was 2.  The second character a digit
       * within range 0-5 which is correct.  In this state, check the third
       * one expecting another digit 0 to 5.  If this is correct, whole escape
       * sequence was inputted correctly.  Add it to the output string and
       * proceed back to state str_in.  Otherwise, return error.
       */
      case str_esc_seq_xyz_5max:
         if(c >= '0' && c<= '5')
         {
            /* Fill in the escape sequence array.  */
            str_add_char(&str, c);
            state = str_inside;
         }
         else if(c == EOF)
         {
            /* Received EOF - write it + switch to error - will end anyway.  */
            str_append_cstring(&str, "\'EOF\'");
            state = str_error;
         }
         else
         {
            /* Other unexpected character - error state. */
            str_add_char(&str, c);
            state = str_error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /* 
       * Error during processing escape sequence in the string has already
       * occurred before.  Now just finish reading the string and return
       * lexical error.
       */
      case str_error:
         if(c == '"')
         {
            /* Insert !" to the start of the output string. */
            char *str_bkup = str_copy_to_cstring(&str); // Backup the original
            str_set_cstring(&str, "!\"");
            str_append_cstring(&str, str_bkup);

            /* Insert quotes to the end, set to error and return. */        
            str_add_char(&str, c);

            token->id = LEX_ERR_TOK_VAL;
            token->a.str = str_copy_to_cstring(&str);

            sfree(str_bkup);
            quit = true;
         }
         else if(c == EOF)
         {
            /* Received EOF - write it + switch to error - will end anyway.  */
            str_append_cstring(&str, "\'EOF\'");
            state = str_error;
         }
         else
         {
            /* Other unexpected character - error state. */
            str_add_char(&str, c);
            state = str_error;
         }

         break;

      /* ------------------------------------------------------------------- */

      /*
       * Program has already received '<' before.  In this state, determine
       * if the operator is composed of more symbols or it is solo by itself.
       */
      case logic_less:
         if(c == '>')
         {
            /* Not equal operator found.  */
            token->id = NOT_EQUAL_ID;
         }
         else if(c == '=')
         {
            /* Less equal operator found. */
            token->id = LESS_EQUAL_ID;
         }
         else
         {
            /* End of the operator. */
            buffer = c;
            token->id = LESS_ID;
         }

         quit = true;
         break;

      /* ------------------------------------------------------------------- */

      /* 
       * Program has already received '>' before.  In this state,  determine
       * if the operator is composed of another symbol or it is solo by itself.
       */
      case logic_greater:
         if(c == '=')
         {
            /* Greater equal operator found. */
            token->id = GREATER_EQUAL_ID;
         }
         else
         {
            /* End of the operator. */
            buffer = c;
            token->id = GREATER_ID;
         }

         quit = true;
         break;

      /* ------------------------------------------------------------------- */

      }  // Switch

      if(quit)
      {
         /* Return based on current contents of token-id.   */
         if(token->id != LEX_ERR_TOK_VAL)
            /* If id does not signalize error, return id value itself.  */            
            return token->id;
         else
            /* Return LEX_ERR if id signalizes that has error occurred.  */
            return LEX_ERR;
      }

   }  // While loop
   
   /* Check if the input file ended properly in the "def" state.  */
   if(state == def)
   {
      token->id = EOF;
   
      return EOF;
   }
   else
   {
      token->id = LEX_ERR_TOK_VAL;
      token->a.str = str_copy_to_cstring(&str);
      
      return LEX_ERR;
   }
}

static int is_keyword(string_t *str)
{
   /* Processing a string is based on the hard-coded pseudo-binary tree.  */
   char *string_data = str_get_data(str);          // String to be processed

   if(string_data[0] < 'i')
   {
      if(string_data[0] < 'd')
      {
         if(string_data[0] < 'b')
         {
            if(!str_cmpsc(str, AS))
              return AS_ID;
            else if(!str_cmpsc(str, ASC))
               return ASC_ID;
            else if(!str_cmpsc(str, AND))
               return AND_ID;
         }
         else
         {
            if(!str_cmpsc(str, BOOLEANN))
               return BOOLEAN_ID;
            else if(!str_cmpsc(str, CONTINUE))
               return CONTINUE_ID;
            else if(!str_cmpsc(str, CHR))
               return CHR_ID;
         }
      }
      else
      {
         if(string_data[0] == 'd')
         {
            if(!str_cmpsc(str, DECLARE))
               return DECLARE_ID;
            else if(!str_cmpsc(str, DIM))
               return DIM_ID;
            else if(!str_cmpsc(str, DO))
               return DO_ID;
            else if(!str_cmpsc(str, DOUBLE))
               return DOUBLE_ID;
         }
         else if(string_data[0] == 'e')
         {
            if(!str_cmpsc(str, EXIT))
               return EXIT_ID;
            else if(!str_cmpsc(str, ELSE))
               return ELSE_ID;
            else if(!str_cmpsc(str, ELSEIF))
               return ELSEIF_ID;
            else if(!str_cmpsc(str, END))
               return END_ID;
         }
         else
         {
            if(!str_cmpsc(str, FOR))
               return FOR_ID;
            else if(!str_cmpsc(str, FALSEE))
               return FALSE_ID;
            else if(!str_cmpsc(str, FUNCTION))
               return FUNCTION_ID;
         }
      }
   }
   else
   {
      if(string_data[0] < 's')
      {
         if(string_data[0] < 'm')
         {
            if(!str_cmpsc(str, IF))
               return IF_ID;
            else if(!str_cmpsc(str, LOOP))
               return LOOP_ID;
            else if(!str_cmpsc(str, INTEGER))
               return INTEGER_ID;
            else if(!str_cmpsc(str, INPUT))
               return INPUT_ID;
            else if(!str_cmpsc(str, LENGTH))
               return LENGTH_ID;
         }
         else
         {
            if(!str_cmpsc(str, OR))
               return OR_ID;
            else if(!str_cmpsc(str, NEXT))
               return NEXT_ID;
            else if(!str_cmpsc(str, NOT))
               return NOT_ID;
            else if(!str_cmpsc(str, PRINT))
               return PRINT_ID;
            else if(!str_cmpsc(str, RETURN))
               return RETURN_ID;
         }
      }
      else
      {
         if(string_data[0] < 't')
         {
            if(!str_cmpsc(str, SUBSTR))
               return SUBSTR_ID;
            else if(!str_cmpsc(str, SHARED))
               return SHARED_ID;
            else if(!str_cmpsc(str, SCOPE))
               return SCOPE_ID;
            else if(!str_cmpsc(str, STATIC))
               return STATIC_ID;
            else if(!str_cmpsc(str, STRING))
               return STRING_ID;
            else if(!str_cmpsc(str, STEP))
               return STEP_ID;
         }
         else
         {
            if(!str_cmpsc(str, TO))
               return TO_ID;
            else if(!str_cmpsc(str, TRUEE))
               return TRUE_ID;
            else if(!str_cmpsc(str, THEN))
               return THEN_ID;
            else if(!str_cmpsc(str, UNTIL))
               return UNTIL_ID;
            else if(!str_cmpsc(str, WHILE))
               return WHILE_ID;
         }
      }
   }

   return NO_KEYWORD_MATCHED;
}

void set_source_file(FILE *f)
{
   source = f;
}
