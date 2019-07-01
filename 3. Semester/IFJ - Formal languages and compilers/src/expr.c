/**
 *  @file expr.c
 *  @brief Expression source file for IFJ Project 2017 AT vut.fit.vutbr.cz.
 *  @date 15.11.2017
 *  @author Tomas Danis - xdanis05@stud.fit.vutbr.cz
 *
 *  Module description: Parsing module responsible for syntactic analysis of
 *                      expressions written in language IFJ17.
 */
#include "error.h"
#include "expr.h"
#include "generator.h"
#include "parser.h"
#include "safe_malloc.h"
#include "scanner.h"
#include "tokenstack.h"

#include <string.h>

#define R_START 0
#define R_OP_EXPRESSION_EXPR 1
#define R_BRACKET_EXPRESSION_RIGHT_BRACKET 2
#define R_BRACKET_EXPRESSION_EXPRESSION 3
#define R_OP_EXPRESSION_OP 4
#define R_FINISH 5

bool consult_table(int topTerm, int input, int *action);
bool reduce(tstack *stack);
bool shift(tstack *stack, token_t *t);
bool rule_start_shift(tstack *stack, token_t *t);

bool undeclared_analysis(char *var_name, char **part1, char **part2);
bool check_type_compabitility(int *op1_type,
                  int *op2_type,
                  int operator,
                  int *result_type);
bool evaluate_expr(token_t *op1, token_t *op2, int operator,  token_t *result);
void convert_literal(token_t *lit, token_t *conv);
void backslash_literal_evaluate(token_t *lit);
int evaluate_length(char *str);
char *evaluate_substr(char *str, int start, int length);
int evaluate_asc(char *str, int position);
char *evaluate_chr(int char_val);

bool expr()
{
   bool success = true;
   int input;
   int top;
   tstack exprStack;
   if(tstack_init(&exprStack)) HANDLE_ERROR(INTERNAL_ERR, success);
   input = token.id;
   top = tstack_get_topterm(&exprStack).id;

   /* Follow(<expr>) = { EOL, ;, then, to, step, , } - 
   indicates this token is no longer part of expression */
   if(input == EOL_ID || input == SEMICOLON_ID ||
      input == THEN_ID || input == TO_ID || input == STEP_ID ||
      input == COMMA_ID)
   {
      input = EXPR_END_ID;
   }

   do
   {
      /* Function calls will be evaluated using top-down analysis, 
         therefore we check for these cases */
      /* Predict( <expr> -> <built-in-func-call> ) = 
         { length, asc, chr, substr } */
      if(input == LENGTH_ID || input == ASC_ID || input == SUBSTR_ID || 
         input == CHR_ID)
      {
         /* <expr> -> <built-in-func-call> simulation */

         if(!built_in_func_call()) HANDLE_ERROR(SYNT_ERR, success);

         /*Semantic stack <built_in_func_call> output:
         Result expression */
         token_t result = tstack_toppop(&semStack);

         SAFE_PUSH(exprStack, result);
      }
      /* Special case with 2 possible rules. However, one is for 
         LL analysis while the other one for PSA analysis.
       Therefore, we peek the next token to find out which rule to use */
      else if(input == ID_ID)
      {
         token_t tmp = token; 
         GET_TOKEN(token);
         if(token.id == LEFT_BRACKET_ID)
         {
            if(!func_call()) HANDLE_ERROR(SYNT_ERR, success);

            generate(&instr_stack, FUNC_CALL_INIT_ID);

            symbol_t *function_entry;
            token_t param_num = tstack_toppop(&semStack);
            function_entry = stab_get_data(&global_table, tmp.a.str);
            if(function_entry == NULL)
            {
               error_msg(line_counter, "Function with the name '%s' does"
                         " not exist!\n", tmp.a.str);
               HANDLE_ERROR(SEM_ERR, success);
            }

            if(function_entry->s.func.param_count != param_num.a.val_int)
            {
               error_msg(line_counter, "Number of passed arguments to "
                         "function '%s' does not match its number of "
                         "parameters!\n", tmp.a.str);
               HANDLE_ERROR(TYPE_COMP_ERR, success);
            }

            for(int i = param_num.a.val_int-1; i >= 0; i--)
            {
               token_t param_token = tstack_toppop(&semStack);
               token_t conv;
               if(!find_conversion(function_entry->s.func.params[i].type,
                                   param_token.type, &conv.id))
               {
                  error_msg(line_counter, "No suitable conversion "
                            "from '%s' to '%s' "
                            "exists!\n", token_to_string(param_token.type), 
                            token_to_string(
                            function_entry->s.func.params[i].type));
                  HANDLE_ERROR(TYPE_COMP_ERR, success);
               }

               if(param_token.is_const)
               {
                  convert_literal(&param_token, &conv);
               }

               token_t param_name;
               param_name.a.str = function_entry->s.func.params[i].code_name;
               param_name.is_const = false;

               SAFE_PUSH(instr_stack, param_name);
               SAFE_PUSH(instr_stack, param_token);
               SAFE_PUSH(instr_stack, conv);

               generate(&instr_stack, PARAM_ID);
            }
            token_t func_name;
            func_name.a.str = function_entry->s.func.code_label;
            SAFE_PUSH(instr_stack, func_name);

            token_t funcExpr;
            funcExpr.id = EXPR_ID;
            funcExpr.a.str = generate(&instr_stack, FUNC_CALL_FIN_ID);
            funcExpr.type = function_entry->s.func.returnType;
            funcExpr.is_const = false;

            SAFE_PUSH(exprStack, funcExpr);
         }
         else
         {
            int action;
            if(!consult_table(top, input, &action))
            {
               HANDLE_ERROR(SYNT_ERR, success);
            }
            else
            {
               if(action == SHIFT_ACTION)
               {
                  if(!shift(&exprStack, &tmp)) return false;
               }
               else if(action == RULE_START_SHIFT_ACTION)
               {
                  if(!rule_start_shift(&exprStack, &tmp)) return false;
               }
               else if(action == REDUCE_ACTION)
               {
                  if(!reduce(&exprStack))
                  {
                     HANDLE_ERROR(SYNT_ERR, success);
                  }
               }
               //End action branch not possible as tmp is ID_ID

               /* We do not call the next token as it was already called */
               /* When input is ID, there is no way we would reduce. 
               Therefore, there is no risk of losing the ID token 
               prematurely in the next iteration */
            }
         }
      }
      else
      {
         int action;
         if(!consult_table(top, input, &action))
         {
            HANDLE_ERROR(SYNT_ERR, success);
         }
         else
         {
            if(action == SHIFT_ACTION)
            {
               if(!shift(&exprStack, &token)) return false;
            }
            else if(action == RULE_START_SHIFT_ACTION)
            {
               if(!rule_start_shift(&exprStack, &token)) return false;
            }
            else if(action == REDUCE_ACTION)
            {
               if(!reduce(&exprStack))
               {
                  HANDLE_ERROR(SYNT_ERR, success);
               }
            }
            else if(action == END_ACTION)
            {
               break;
            }
         }

         if(input != EXPR_END_ID && action != REDUCE_ACTION)
         {
            GET_TOKEN(token);
         }
      }

      input = token.id;
      top = tstack_get_topterm(&exprStack).id;

      /* Follow(<expr>) = { EOL, ;, then, to, step, , } - indicates this 
      token is no longer part of expression */
      if(input == EOL_ID || input == SEMICOLON_ID ||
      input == THEN_ID || input == TO_ID || input == STEP_ID ||
      input == COMMA_ID)
      {
         input = EXPR_END_ID;
      }
   }
   while(input != EXPR_END_ID || top != EXPR_END_ID);

   token_t final_expr = tstack_toppop(&exprStack);
   SAFE_PUSH(semStack, final_expr);
   tstack_free(&exprStack);

   return success;
}


/**
 * Possible rules:
 * <func_call> -> ( <arg_list> )
 */
bool func_call()
{
   bool success = true;

   /* Predict( <func_call> -> ( <arg_list> ) ) = { ( } */
   if(token.id == LEFT_BRACKET_ID)
   {
      /* <func_call> -> ( <arg_list> ) simulation */

      GET_TOKEN(token);
      if(!arg_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <arg_list> output:
      Number of parameters followed by a reversed list of parameters */
      /*We do nothing as the desired output for <func_call> is
      already on the stack */

      if(token.id != RIGHT_BRACKET_ID)
      {
         error_msg(line_counter, "Expected ')' at the end of a function "
                   " call statement!\n");
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
   }
   else
   {
      error_msg(line_counter, "Expected '(' in a function call!\n");
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <arg-list> -> <arg> <more-args-list>
 * <arg-list> -> ε
 */
bool arg_list()
{
   bool success = true;

   token_t arg_num;

   /* Predict( <arg-list> -> <arg> <more-args-list> ) = { (, id, int_literal,
      double_literal, string_literal, length, asc, chr, substr } */
   if(token.id == LEFT_BRACKET_ID || token.id == ID_ID ||
      token.id == INTEGER_LITERAL_ID || token.id == DOUBLE_LITERAL_ID ||
      token.id == STRING_LITERAL_ID || token.id == LENGTH_ID ||
      token.id == ASC_ID || token.id == CHR_ID || token.id == SUBSTR_ID)
   {
      /* <arg-list> -> <arg> <more-args-list> simulation */

      if(!arg()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <more_args_list> input:
      Number of already read parameters */
      arg_num.a.val_int = 1;
      SAFE_PUSH(semStack, arg_num);

      if(!more_args_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <more_args_list> output:
         Number of parameters followed by a reversed list of parameters*/
      //We do nothing as the desired output is already on the stack

   }
   /* Predict( <arg-list> -> ε ) = { ) } */
   else if(token.id == RIGHT_BRACKET_ID)
   {
      /* <arg-list> -> ε simulation */

      /* Semantic stack <arg-list> output:
         Number of parameters followed by a reversed list of parameters */
      arg_num.a.val_int = 0;
      SAFE_PUSH(semStack, arg_num);

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected an expression as function call"
                " argument!\n");
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <more-args-list> -> , <arg> <more-args-list>
 * <more-args-list> -> ε
 */
bool more_args_list()
{
   bool success = true;

   /* Predict( <more-args-list> -> , <arg> <more-args-list> ) = { , } */
   if(token.id == COMMA_ID)
   {
      /* <more-args-list> -> , <arg> <more-args-list> simulation */

      /* Semantic stack <more-args-list> input:
         Number of already read parameters */
      token_t param_num = tstack_toppop(&semStack);

      GET_TOKEN(token);
      if(!arg()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <arg> output:
         Read parameters */
      param_num.a.val_int++;
      SAFE_PUSH(semStack, param_num);

      if(!more_args_list()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <more-args-list> -> ε ) = { ) } */
   else if(token.id == RIGHT_BRACKET_ID)
   {
      /* <more-args-list> -> ε simulation */

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected ',' or ')' in a function call"
                " statement!\n");
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <arg> -> <expr>
 */
bool arg()
{
   bool success = true;

   /* Predict( <arg> -> <expr> ) = { (, id, int_literal, double_literal, string_literal, length, asc, chr, substr } */
   if(token.id == LEFT_BRACKET_ID || token.id == ID_ID ||
      token.id == INTEGER_LITERAL_ID || token.id == DOUBLE_LITERAL_ID ||
      token.id == STRING_LITERAL_ID || token.id == LENGTH_ID ||
      token.id == ASC_ID || token.id == CHR_ID || token.id == SUBSTR_ID)
   {
      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr> output:
         Final expression */
      //We do nothing as the desired output is already on the stack 

   }
   else
   {
      error_msg(line_counter, "Expected an expression as function call"
                " argument!\n");
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <built-in-func-call> -> length <func-call>
 * <built-in-func-call> -> substr <func-call>
 * <built-in-func-call> -> asc <func-call>
 * <built-in-func-call> -> chr <func-call>
 */
bool built_in_func_call()
{
   bool success = true;

   token_t name;
   bool all_params_const = true;
   /* Predict( <built-in-func-call> -> length <func-call> ) = { length } */
   if(token.id == LENGTH_ID)
   {
      /* <built-in-func-call> -> length <func-call> simulation */
      name.a.str = "length";

      GET_TOKEN(token);
      if(!func_call()) HANDLE_ERROR(SYNT_ERR, success);
   }
   /* Predict( <built-in-func-call> -> substr <func-call> ) = { substr } */
   else if(token.id == SUBSTR_ID)
   {
      /* <built-in-func-call> -> substr <func-call> simulation */
      name.a.str = "substr";

      all_params_const = false;

      GET_TOKEN(token);
      if(!func_call()) HANDLE_ERROR(SYNT_ERR, success);
   }
   /* Predict( <built-in-func-call> -> asc <func-call> ) = { asc } */
   else if(token.id == ASC_ID)
   {
      /* <built-in-func-call> -> asc <func-call> simulation */
      name.a.str = "asc";

      GET_TOKEN(token);
      if(!func_call()) HANDLE_ERROR(SYNT_ERR, success);
   }
   /* Predict( <built-in-func-call> -> chr <func-call> ) = { chr } */
   else if(token.id == CHR_ID)
   {
      /* <built-in-func-call> -> chr <func-call> simulation */
      name.a.str = "chr";

      GET_TOKEN(token);
      if(!func_call()) HANDLE_ERROR(SYNT_ERR, success);
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   /* Semantic checks */
   symbol_t *function_entry;
   token_t param_num = tstack_toppop(&semStack);
   function_entry = stab_get_data(&global_table, name.a.str);

   if(function_entry->s.func.param_count != param_num.a.val_int)
   {
      error_msg(line_counter, "Number of passed arguments to "
                "function '%s' does not match its number of "
                "parameters!\n", name.a.str);
      HANDLE_ERROR(TYPE_COMP_ERR, success);
   }

   for(int i = param_num.a.val_int-1; i >= 0; i--)
   {
      token_t param_token = tstack_toppop(&semStack);
      token_t conv;
      if(!find_conversion(function_entry->s.func.params[i].type,
                          param_token.type, &conv.id))
      {
         error_msg(line_counter, "No suitable conversion "
                   "from '%s' to '%s' "
                   "exists!\n", token_to_string(param_token.type), 
                   token_to_string(
                   function_entry->s.func.params[i].type));
         HANDLE_ERROR(TYPE_COMP_ERR, success);
      }

      if(param_token.is_const)
      {
         convert_literal(&param_token, &conv);
      }
      else
      {
         all_params_const = false;
      }

      SAFE_PUSH(instr_stack, param_token);
      SAFE_PUSH(instr_stack, conv);
   }

   token_t funcExpr;
   funcExpr.id = EXPR_ID;
   funcExpr.type = function_entry->s.func.returnType;
   
   if(all_params_const)
   {
      if(strcmp(name.a.str, "length") == 0)
      {
         tstack_pop(&instr_stack);
         token_t str_token = tstack_toppop(&instr_stack);
         funcExpr.a.val_int = evaluate_length(str_token.a.str);
      }
      else if(strcmp(name.a.str, "chr") == 0)
      {
         tstack_pop(&instr_stack);
         token_t char_val = tstack_toppop(&instr_stack);
         funcExpr.a.str = evaluate_chr(char_val.a.val_int);
         if(funcExpr.a.str == NULL)
         {
            internal_error_msg("Failed to allocate memory!\n");
            HANDLE_ERROR(INTERNAL_ERR, success);
         }
      }
      else if(strcmp(name.a.str, "asc") == 0)
      {
         tstack_pop(&instr_stack);
         token_t str_token = tstack_toppop(&instr_stack);
         
         tstack_pop(&instr_stack);
         token_t char_pos = tstack_toppop(&instr_stack);
         funcExpr.a.val_int = evaluate_asc(str_token.a.str,
                                           char_pos.a.val_int);
      }
      else if(strcmp(name.a.str, "substr") == 0)
      {
         tstack_pop(&instr_stack);
         token_t str_token = tstack_toppop(&instr_stack);
         
         tstack_pop(&instr_stack);
         token_t start_token = tstack_toppop(&instr_stack);

         tstack_pop(&instr_stack);
         token_t length_token = tstack_toppop(&instr_stack);
         funcExpr.a.str = evaluate_substr(str_token.a.str, 
                                              start_token.a.val_int, 
                                              length_token.a.val_int);
         if(funcExpr.a.str == NULL)
         {
            internal_error_msg("Failed to allocate memory!\n");
            HANDLE_ERROR(INTERNAL_ERR, success);
         }
      }
      funcExpr.is_const = true;
   }
   else
   {
      funcExpr.is_const = false;

      if(strcmp(name.a.str, "length") == 0)
      {
         funcExpr.a.str = generate(&instr_stack, INL_LEN_ID);
      }
      else if(strcmp(name.a.str, "chr") == 0)
      {
         funcExpr.a.str = generate(&instr_stack, INL_CHR_ID);
      }
      else if(strcmp(name.a.str, "asc") == 0)
      {
         funcExpr.a.str = generate(&instr_stack, INL_ASC_ID);
      }
      else if(strcmp(name.a.str, "substr") == 0)
      {
         funcExpr.a.str = generate(&instr_stack, INL_SUBSTR_ID);
      }
   }

   SAFE_PUSH(semStack, funcExpr);


   return success;
}

bool reduce(tstack *stack)
{
   bool success = true;
   //Finite state automata implementation for rule detection
   int state = R_START;
   int operator;
   token_t in;
   token_t exprToken;
   exprToken.id = EXPR_ID;
   while((in = tstack_top(stack)).id != RULE_START_ID)
   {
      if(in.id == EXPR_END_ID)
      {
          error_msg(line_counter, "Missing operand in an expression!\n");
          HANDLE_ERROR(SYNT_ERR, success);
          break;
      }
      tstack_pop(stack);

      switch(state)
      {
       case R_START:

         if(in.id == ID_ID)
         {
            /* <expr> -> id simulation */

            state = R_FINISH;

            /* Semantic checks */
            symbol_t *entry_to_use;
            if(!check_variable_declaration(in.a.str, &entry_to_use))
            {
               HANDLE_ERROR(SEM_ERR, success);
            }

            struct var_info *var;
            var = vdstack_top(&(entry_to_use->s.var_stack));
            exprToken.type = var->type;

            /* If the variable is constant and the code structure
               allows it, optimise by not using the variable */
            if(var->is_const && code_structure == 
               CODE_STRUCTURE_LINEAR)
            {
               cp_val_to_token(&exprToken, var);
               exprToken.is_const = true;
            }
            else
            {
               exprToken.a.str = var->code_name;
               var->used = true;
               exprToken.is_const = false;
            }
         }
         else if(in.id == INTEGER_LITERAL_ID)
         {
            /* <expr> -> int_literal simulation */

            state = R_FINISH;
            exprToken.type = INTEGER_ID;
            exprToken.a.val_int = in.a.val_int;
            exprToken.is_const = true;

         }
         else if(in.id == DOUBLE_LITERAL_ID)
         {
            /* <expr> -> double_literal simulation */

            state = R_FINISH;
            exprToken.type = DOUBLE_ID;
            exprToken.a.val_real = in.a.val_real;
            exprToken.is_const = true;
         }
         else if(in.id == STRING_LITERAL_ID)
         {
            /* <expr> -> string_literal simulation */

            state = R_FINISH;
            exprToken.type = STRING_ID;
            exprToken.a.str = in.a.str;
            exprToken.is_const = true;
         }
         else if(in.id == RIGHT_BRACKET_ID)
         {
            /* <expr> -> ( <expr> ) beginning */

            state = R_BRACKET_EXPRESSION_RIGHT_BRACKET;

         }
         else if(in.id == EXPR_ID)
         {
            /* One of <expr> <op> <expr> rules beginning */

            state = R_OP_EXPRESSION_EXPR;

            /* First operand push for generation */
            SAFE_PUSH(semStack, in);
         }
         else
         {
            error_msg(line_counter, "Missing operand in an expression!\n");
            HANDLE_ERROR(SYNT_ERR, success);
         }

         break;

       case R_BRACKET_EXPRESSION_RIGHT_BRACKET:

         if(in.id == EXPR_ID)
         {
            /* <expr> -> ( <expr> ) continuation */

            state = R_BRACKET_EXPRESSION_EXPRESSION;
            exprToken.type = in.type;
            exprToken.a = in.a;
            exprToken.is_const = in.is_const;
         }
         else
         {
            error_msg(line_counter, "Empty bracket expression!\n");
            HANDLE_ERROR(SYNT_ERR, success);
         }

         break;

       case R_BRACKET_EXPRESSION_EXPRESSION:

         if(in.id == LEFT_BRACKET_ID)
         {
            /* <expr> -> ( <expr> ) simulation */

            state = R_FINISH;
         }
         else
         {
            error_msg(line_counter, "Unmatched right bracket!\n");
            HANDLE_ERROR(SYNT_ERR, success);
         }

         break;

       case R_OP_EXPRESSION_EXPR:

         if(in.id == PLUS_ID)
         {
            /* <expr> -> <expr> + <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else if(in.id == MINUS_ID)
         {
            /* <expr> -> <expr> - <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else if(in.id == ASTERISK_ID)
         {
            /* <expr> -> <expr> * <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else if(in.id == SLASH_ID)
         {
            /* <expr> -> <expr> / <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else if(in.id == BACKSLASH_ID)
         {
            /* <expr> -> <expr> \ <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else if(in.id == EQUALS_ID)
         {
            /* <expr> -> <expr> = <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else if(in.id == NOT_EQUAL_ID)
         {
            /* <expr> -> <expr> <> <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else if(in.id == LESS_ID)
         {
            /* <expr> -> <expr> < <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else if(in.id == LESS_EQUAL_ID)
         {
            /* <expr> -> <expr> <= <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else if(in.id == GREATER_ID)
         {
            /* <expr> -> <expr> > <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else if(in.id == GREATER_EQUAL_ID)
         {
            /* <expr> -> <expr> >= <expr> simulation */

            state = R_OP_EXPRESSION_OP;
         }
         else
         {
            error_msg(line_counter, "Missing operator in an expression!\n");
            HANDLE_ERROR(SYNT_ERR, success);
         }
         operator = in.id;

         break;

       case R_OP_EXPRESSION_OP:

         if(in.id == EXPR_ID)
         {
            /* Some <expr> -> <expr> <op> <expr> rule simulation */

            state = R_FINISH;

            /* Semantic checks */
            token_t conv1;
            token_t conv2;
            token_t op2 = tstack_toppop(&semStack);
            conv1.id = in.type;
            conv2.id = op2.type;
            if(!check_type_compabitility(&conv1.id,
                           &conv2.id,
                           operator,
                           &exprToken.type))
            {
               error_msg(line_counter, "Invalid operands to operator '%s'."
                     "('%s' and '%s')\n", token_to_string(operator),
                     token_to_string(in.type), token_to_string(op2.type));
               HANDLE_ERROR(TYPE_COMP_ERR, success);
            }

            /* To-convert and operand type match, no conversion */
            if(in.type == conv1.id)
               conv1.id = NO_CONVERSION;
            if(op2.type == conv2.id)
               conv2.id = NO_CONVERSION;

            /* Convert literals manually */
            if(operator == BACKSLASH_ID)
            {
               if(in.is_const)
                  backslash_literal_evaluate(&in);
               if(op2.is_const)
                  backslash_literal_evaluate(&op2);
            }
            else
            {
               if(in.is_const)
                  convert_literal(&in, &conv1);
               if(op2.is_const)
                  convert_literal(&op2, &conv2);
            }
            
            /* If operands are constant, evaluate them in code */
            if(in.is_const && op2.is_const)
            {
               if(!evaluate_expr(&in, &op2, operator, &exprToken))
                  HANDLE_ERROR(OTHER_ERR, success);
               exprToken.is_const = true;
            }
            else
            {
               /* Push operands */
               SAFE_PUSH(instr_stack, op2);
               SAFE_PUSH(instr_stack, in);

               /* Push convert tokens */
               SAFE_PUSH(instr_stack, conv2);
               SAFE_PUSH(instr_stack, conv1);

               exprToken.a.str = generate(&instr_stack, operator);
               exprToken.is_const = false;
            }
         }
         else
         {
            error_msg(line_counter, "Missing operand in an expression!\n");
            HANDLE_ERROR(SYNT_ERR, success);
         }
         break;

       case R_FINISH:
         /*If we came to this state, it means a rule was finished, but the rule opening token was not found.
            Therefore, no matter the input at this stage, the rule is invalid. */
         HANDLE_ERROR(SYNT_ERR, success);
         break;
      }
   }

   tstack_pop(stack);

   if(success)
   {
      if(state == R_FINISH)
      {
         if(tstack_push(stack, &exprToken)) 
         {
            internal_error_msg("Failed to allocate memory!");
            HANDLE_ERROR(INTERNAL_ERR, success);
         }
      }
      else
      {
         error_msg(line_counter, "Missing operand in an expression!\n");
         HANDLE_ERROR(SYNT_ERR, success);
      }
   }

   return success;
}

bool shift(tstack *stack, token_t *t)
{
   bool success = true;
   if(tstack_push(stack, t)) HANDLE_ERROR(INTERNAL_ERR, success);
   return success;
}

bool rule_start_shift(tstack *stack, token_t *t)
{
   bool success = true;
   token_t tmp;
   tmp.id = RULE_START_ID;
   if(tstack_push_after_topterm(stack, &tmp)) 
      HANDLE_ERROR(INTERNAL_ERR, success);
   if(tstack_push(stack, t)) HANDLE_ERROR(INTERNAL_ERR, success);
   return success;
}

bool consult_table(int topTerm, int input, int *action)
{
   *action = -1;
   if(input == PLUS_ID || input == MINUS_ID)
   {
      if(topTerm == PLUS_ID || topTerm == MINUS_ID || topTerm == ID_ID ||
       topTerm == INTEGER_LITERAL_ID || topTerm == DOUBLE_LITERAL_ID ||
       topTerm == STRING_LITERAL_ID || topTerm == RIGHT_BRACKET_ID ||
       topTerm == ASTERISK_ID || topTerm == SLASH_ID ||
       topTerm == BACKSLASH_ID)
      {
         *action = REDUCE_ACTION;
      }
      else
      {
         *action = RULE_START_SHIFT_ACTION;
      }
   }
   else if(input == ASTERISK_ID || input == SLASH_ID)
   {
      if(topTerm == ASTERISK_ID || topTerm == SLASH_ID || topTerm == ID_ID ||
       topTerm == INTEGER_LITERAL_ID || topTerm == DOUBLE_LITERAL_ID ||
       topTerm == STRING_LITERAL_ID || topTerm == RIGHT_BRACKET_ID)
      {
         *action = REDUCE_ACTION;
      }
      else
      {
         *action = RULE_START_SHIFT_ACTION;
      }
   }
   else if(input == BACKSLASH_ID)
   {
      if(topTerm == ASTERISK_ID || topTerm == SLASH_ID || 
         topTerm == BACKSLASH_ID || topTerm == ID_ID || 
         topTerm == INTEGER_LITERAL_ID || topTerm == DOUBLE_LITERAL_ID || 
         topTerm == STRING_LITERAL_ID || topTerm == RIGHT_BRACKET_ID)
      {
         *action = REDUCE_ACTION;
      }
      else
      {
         *action = RULE_START_SHIFT_ACTION;
      }
   }
   else if(input == EQUALS_ID || input == NOT_EQUAL_ID || 
           input == GREATER_ID || input == GREATER_EQUAL_ID || 
           input == LESS_ID || input == LESS_EQUAL_ID)
   {
      if(topTerm == LEFT_BRACKET_ID || topTerm == EXPR_END_ID)
      {
         *action = RULE_START_SHIFT_ACTION;
      }
      else if(topTerm == EQUALS_ID || topTerm == NOT_EQUAL_ID ||
         topTerm == GREATER_ID || topTerm == GREATER_EQUAL_ID ||
         topTerm == LESS_ID || topTerm == LESS_EQUAL_ID)
      {
         error_msg(line_counter, "Relational operator chaining is not"
         " allowed!\n");
         return false;
      }
      else
      {
         *action = REDUCE_ACTION;
      }
   }
   else if(input == ID_ID || input == INTEGER_LITERAL_ID || 
           input == DOUBLE_LITERAL_ID || input == STRING_LITERAL_ID || 
           input == LEFT_BRACKET_ID)
   {
      if(topTerm == ID_ID || topTerm == INTEGER_LITERAL_ID ||
       topTerm == DOUBLE_LITERAL_ID || topTerm == STRING_LITERAL_ID)
      {
         error_msg(line_counter, "Missing operator between '%s' and '%s'"
            "!\n", token_to_string(topTerm), token_to_string(input));
         return false;
      }
      else
      {
         *action = RULE_START_SHIFT_ACTION;
      }
   }
   else if(input == RIGHT_BRACKET_ID)
   {
      if(topTerm == LEFT_BRACKET_ID)
      {
         *action = SHIFT_ACTION;
      }
      else if(topTerm == EXPR_END_ID)
      {
         /*Special case for arguments in function calls, 
         in this case, ) is no longer part of the expression */
         *action = END_ACTION; 
      }
      else
      {
         *action = REDUCE_ACTION;
      }
   }
   else if(input == EXPR_END_ID)
   {
      if(topTerm == LEFT_BRACKET_ID)
      {
         error_msg(line_counter, "Unmatched left bracket detected!\n");
         return false;
      }
      else
      {
         *action = REDUCE_ACTION;
      }
   }
   else
   {
      error_msg(line_counter, "Expected continuation of expression, "
                "received '%s'!\n", token_to_string(input));
      return false;
   }

   return true;
}

/**
 * @brief Checks type compability of given operands for the given operation.
 * 
 * @param[in,out] *op1_type Type of the first operand. On return, it contains
 *                the type to which the first operand is to be converted.
 * @param[in,out] *op2_type Type of the second operand. On return, it contains
 *                the type to which the second operand is to be converted.
 * @param[in] operator The ID of the operator
 * @param[out] result_type Type of the result of this operation.
 * @return Returns true if the the types are compatible for this operation,
       false otherwise.
 */
bool check_type_compabitility(int *op1_type,
                  int *op2_type,
                  int operator,
                  int *result_type)
{

   if(*op1_type == BOOLEAN_ID || *op2_type == BOOLEAN_ID)
      return false;
   if(*op1_type == STRING_ID || *op2_type == STRING_ID)
   {
      if(operator == MINUS_ID || operator == ASTERISK_ID || 
         operator == SLASH_ID || operator == BACKSLASH_ID)
         return false;
   }

   switch(operator)
   {
      case PLUS_ID:
      {
       if(*op1_type == STRING_ID || *op2_type == STRING_ID)
       {
         if(*op1_type == STRING_ID && *op2_type == STRING_ID)
         {
            *result_type = STRING_ID;
            return true;
         }
         return false;
       }
       if(*op1_type == DOUBLE_ID || *op2_type == DOUBLE_ID)
       {
         *result_type = *op1_type = *op2_type = DOUBLE_ID;
         return true;
       }
       else
       {
         *result_type = INTEGER_ID;
         return true;
       }
       break;
      }

      case MINUS_ID:
      case ASTERISK_ID:
      {
       if(*op1_type == DOUBLE_ID || *op2_type == DOUBLE_ID)
       {
         *result_type = *op1_type = *op2_type = DOUBLE_ID;
         return true;
       }
       else
       {
         *result_type = INTEGER_ID;
         return true;
       }
       break;
      }

      case SLASH_ID:
      {
       if(*op1_type == STRING_ID || *op2_type == STRING_ID)
         return false;
       *result_type = *op1_type = *op2_type = DOUBLE_ID;
       break;
      }

      case BACKSLASH_ID:
      {
       if(*op1_type == STRING_ID || *op2_type == STRING_ID)
         return false;
       *result_type = *op1_type = *op2_type = INTEGER_ID;
       break;
      }

      case LESS_ID:
      case LESS_EQUAL_ID:
      case EQUALS_ID:
      case NOT_EQUAL_ID:
      case GREATER_ID:
      case GREATER_EQUAL_ID:
      {
       *result_type = BOOLEAN_ID;
       if(*op1_type == STRING_ID || *op2_type == STRING_ID)
       {
         if(*op1_type != STRING_ID || *op2_type != STRING_ID)
            return false;
       }
       else if(*op1_type == DOUBLE_ID || *op2_type == DOUBLE_ID)
       {
         *op1_type = *op2_type = DOUBLE_ID;
         return true;
       }
       else
       {
         *op1_type = *op2_type = INTEGER_ID;
         return true;
       }
       break;
      }
   }
   return true;
}



bool evaluate_expr(token_t *op1, token_t *op2, int operator, token_t *result)
{
   bool success = true;

   double op1num;
   double op2num;;
   if(op1->type == INTEGER_ID)
      op1num = op1->a.val_int;
   else if(op1->type == DOUBLE_ID)
      op1num = op1->a.val_real;

   if(op2->type == INTEGER_ID)
      op2num = op2->a.val_int;
   else if(op2->type == DOUBLE_ID)
      op2num = op2->a.val_real;

   switch (operator)
   {
      case PLUS_ID:
      {
       if(op1->type == STRING_ID)
       {
         result->a.str = smalloc(strlen(op1->a.str) +
                        strlen(op2->a.str) +
                        1);
         if(result->a.str == NULL)
         {
            internal_error_msg("Failed to allocate memory!\n");
            HANDLE_ERROR(INTERNAL_ERR, success);
         }
         result->a.str[0] = 0;
         result->a.str = strcat(result->a.str, op1->a.str);
         result->a.str = strcat(result->a.str, op2->a.str);
         return success;
       }
       else if(result->type == INTEGER_ID)
       {
         result->a.val_int = (int)(op1num + op2num);
       }
       else if(result->type == DOUBLE_ID)
       {
         result->a.val_real = op1num + op2num;
       }
       break;
      }

      case MINUS_ID:
      {
       if(result->type == INTEGER_ID)
       {
         result->a.val_int = (int)(op1num - op2num);
       }
       else if(result->type == DOUBLE_ID)
       {
         result->a.val_real = op1num - op2num;
       }
       break;

      }
      case ASTERISK_ID:
      {
       if(result->type == INTEGER_ID)
       {
         result->a.val_int = (int)(op1num * op2num);
       }
       else if(result->type == DOUBLE_ID)
       {
         result->a.val_real = op1num * op2num;
       }
       break;
      }

      case SLASH_ID:
      {
       if(op2num == 0)
       {
         error_msg(line_counter, "Zero division detected!\n");
         HANDLE_ERROR(OTHER_ERR, success);
         return false;
       }
       if(result->type == INTEGER_ID)
       {
         result->a.val_int = (int)(op1num / op2num);
       }
       else if(result->type == DOUBLE_ID)
       {
         result->a.val_real = op1num / op2num;
       }
       break;
      }

      /* For this case, values have already been rounded */
      case BACKSLASH_ID:
      {
       if(op2num == 0)
       {
         error_msg(line_counter, "Zero division detected!\n");
         HANDLE_ERROR(OTHER_ERR, success);
         return false;
       }
       if(result->type == INTEGER_ID)
       {
         result->a.val_int = (int)(op1num / op2num);
       }
       else if(result->type == DOUBLE_ID)
       {
         result->a.val_real = op1num / op2num;
       }
       break;
      }

      case LESS_ID:
      {
       if(op1->type == STRING_ID)
       {
         result->a.val_bool = (strcmp(op1->a.str, op2->a.str) < 0);
       }
       else
       {
         result->a.val_bool = op1num < op2num;
       }
       break;
      }

      case LESS_EQUAL_ID:
      {
       if(op1->type == STRING_ID)
       {
         result->a.val_bool = (strcmp(op1->a.str, op2->a.str) <= 0);
       }
       else
       {
         result->a.val_bool = op1num <= op2num;
       }
       break;
      }

      case EQUALS_ID:
      {
       if(op1->type == STRING_ID)
       {
         result->a.val_bool = (strcmp(op1->a.str, op2->a.str) == 0);
       }
       else
       {
         result->a.val_bool = op1num == op2num;
       }
       break;
      }

      case NOT_EQUAL_ID:
      {
       if(op1->type == STRING_ID)
       {
         result->a.val_bool = (strcmp(op1->a.str, op2->a.str) != 0);
       }
       else
       {
         result->a.val_bool = op1num != op2num;
       }
       break;
      }

      case GREATER_ID:
      {
       if(op1->type == STRING_ID)
       {
         result->a.val_bool = (strcmp(op1->a.str, op2->a.str) > 0);
       }
       else
       {
         result->a.val_bool = op1num > op2num;
       }
       break;
      }

      case GREATER_EQUAL_ID:
      {
       if(op1->type == STRING_ID)
       {
         result->a.val_bool = (strcmp(op1->a.str, op2->a.str) >= 0);
       }
       else
       {
         result->a.val_bool = op1num >= op2num;
       }
       break;
      }
   }
   return true;
}

bool check_variable_declaration(char *name, symbol_t **var_entry)
{
   bool success = true;

   symbol_t *l_var_entry = stab_get_data(&local_table, name);
   symbol_t *g_var_entry = stab_get_data(&global_table, name);

   /* Check for undeclared variable */
   if(l_var_entry == NULL && g_var_entry == NULL)
   {
      //Handle error
      char *part1;
      char *part2;
      if(undeclared_analysis(name, &part1, &part2))
      {
       error_msg(line_counter, "Undeclared variable"
         "'%s' . However, '%s' and '%s' is declared."
         "Are you missing an operator?\n",
         name, part1, part2);
       sfree(part1);
       sfree(part2);
      }
      else
      {
       error_msg(line_counter, "Undeclared variable " 
         "'%s'\n", name);
      }
      HANDLE_ERROR(SEM_ERR, success);
      return success;
   }

   if(g_var_entry != NULL)
   {
      /* Function name cannot be used as a variable */
      if(g_var_entry->type == SYMB_FUNC)
      {
         error_msg(line_counter, "'%s' is function but is used as "
            "variable!\n", name);
         HANDLE_ERROR(SEM_ERR, success);
         return success;
      }

      *var_entry = g_var_entry;
   }

   if(l_var_entry != NULL)
      *var_entry = l_var_entry;

   return success;
}

void convert_literal(token_t *lit, token_t *conv)
{
   if(conv->id == DOUBLE_ID)
   {
      lit->a.val_real = lit->a.val_int;
      lit->type = DOUBLE_ID;
   }
   else if(conv->id == INTEGER_ID)
   {
      lit->a.val_int = round_even(lit->a.val_real);
      lit->type = INTEGER_ID;
   }
   conv->id = NO_CONVERSION;
}

void backslash_literal_evaluate(token_t *lit)
{
   if(lit->type == DOUBLE_ID)
      lit->a.val_real = round_even(lit->a.val_real);
   else
   {
      lit->a.val_real = lit->a.val_int;
      lit->type = DOUBLE_ID;
   }
}

/**
 * Find conversion to type1 from type2 and saves it into result_type
 */
bool find_conversion(int type1, int type2, int *result_type)
{
   if(type1 == STRING_ID || type2 == STRING_ID)
   {
      if(type1 == STRING_ID && type2 == STRING_ID)
      {
       *result_type = NO_CONVERSION;
       return true;
      }
      return false;
   }
   else if(type1 == BOOLEAN_ID || type2 == BOOLEAN_ID)
   {
      if(type1 == BOOLEAN_ID && type2 == BOOLEAN_ID)
      {
       *result_type = NO_CONVERSION;
       return true;
      }
      return false;
   }
   else if(type1 == DOUBLE_ID)
   {
      if(type2 == INTEGER_ID)
      {
       *result_type = DOUBLE_ID;
      }
      else
      {
       *result_type = NO_CONVERSION;
      }
      return true;
   }
   else if(type1 == INTEGER_ID)
   {
      if(type2 == DOUBLE_ID)
      {
       *result_type = INTEGER_ID;
      }
      else
      {
       *result_type = NO_CONVERSION;
      }
      return true;
   }
   return true;
}

bool undeclared_analysis(char *var_name, char **part1, char **part2)
{
   int length = strlen(var_name);
   *part1 = smalloc(length + 1);
   *part2 = smalloc(length + 1);
   if(part1 == NULL || part2 == NULL) return false;

   for(int i = 1; i < length; i++)
   {
      memcpy(*part1, var_name, i);
      (*part1)[i+1] = 0;
      memcpy(*part2, var_name+i, length-i);
      (*part2)[i+1] = 0;
      if(stab_get_data(&local_table, *part1))
      {
       if(stab_get_data(&local_table, *part2))
         return true;
       if(stab_get_data(&global_table, *part2))
         return true;
      }
      else if(stab_get_data(&global_table, *part1))
      {
       if(stab_get_data(&local_table, *part2))
         return true;
       if(stab_get_data(&global_table, *part2))
         return true;
      }
   }
   sfree(*part1);
   sfree(*part2);
   return false;
}

int evaluate_length(char *str)
{
   int i = 0;
   int len = 0;
   while(str[i] != 0)
   {
      len++;
      if(str[i] == '\\')
         len -= 3;
      i++;
   }

   return len;
}

char *evaluate_substr(char *str, int start, int length)
{
   int len = evaluate_length(str);
   char *ret_val;
   int mal_size;
   if(start <= 0 || start > len || len == 0)
   {
      mal_size = 1;
   }
   else if(length < 0 || length > len)
   {
      mal_size = len - start + 1;
   }
   else 
   {
      mal_size = length+1;
   }

   ret_val = smalloc(mal_size);
   if(ret_val == NULL) return NULL;

   if(mal_size == 1)
   {
      ret_val[0] = 0;
      return ret_val;
   }

   strncpy(ret_val, str+start, mal_size-2);
   ret_val[mal_size-1] = 0;

   return ret_val;
}

int evaluate_asc(char *str, int position)
{
   int len = evaluate_length(str);
   if(position-1 < 0 || position-1 >= len)
      return 0;

   int i = 0;
   while(i+1 != position)
   {
      if(str[i] == '\\')
      {
         i -= 3;
      }
      i++;
   }

   int ret_val = 0;

   if(str[i] == '\\')
   {
      ret_val = 100*(str[i+1]-'0') + 10*(str[i+2]-'0') + str[i+3] - '0'; 
   }
   else
   {
      ret_val = str[i];
   }

   return ret_val;
}

char *evaluate_chr(int char_val)
{
   char *ret_val = smalloc(5);
   if(ret_val == NULL) return NULL;

   snprintf(ret_val, 5, "\\%.*d", 3,  char_val);
 
   return ret_val;
}