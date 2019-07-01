/**
 * @file parser.h
 * @brief parser.h for IFJ Project 2017 AT vut.fit.vutbr.cz
 * @date 11. 11. 2017, last rev. 11.11.2017
 * @author Tomas Danis - xdanis05
 *
 * Module description: Parsing module resposible for the syntactic analysis of
 *                   a given source program written in language IFJ17.
 */

/*    Includes section   */
#include "error.h"
#include "expr.h"
#include "generator.h"
#include "ifj17_api.h"
#include "parser.h"
#include "safe_malloc.h"
#include "tokenstack.h"

/* Prototypes  */
bool program();
bool decl_list();
bool var_decl();
bool func_decl();
bool func_def();
bool param_list();
bool param();
bool param_list_cont();
bool opt_scope_modifier();
bool opt_initialiser();
bool type();
bool stat_list();
bool statement();
bool assignment();
bool read_statement();
bool print_statement();
bool expr_list();
bool scope_statement();
bool selection_statement();
bool alternative_statement();
bool iteration_statement();
bool opt_type_decl();
bool opt_step();
bool opt_id();
bool do_cycle();
bool opt_cond();
bool cond();
bool do_mode();
bool return_statement();
bool iteration_control_statement();
bool control_statement();
bool cycle_type_list();
bool cycle_type();
bool empty_statement();

void pop_block_vars(symtab_t *table, int depth);
bool all_functions_defined(symtab_t *table);

/* Global variables/Global variables struct? */
/** @brief Current token  */
token_t token;
symtab_t global_table;
symtab_t local_table;
tstack semStack;
tstack instr_stack;
char *current_function_name;
int block_depth = 0;
int context;
int cycle_depth = 0;
int code_structure;

bool parse()
{
   return program();
}

/*    Functions definitions section  */
/**
 * Possible rules:
 * <prog> -> <decl_list> Scope EOL <stat_list> End Scope EOF
 */
bool program()
{
   bool success = true;

   /* Predict( <prog> -> <decl_list> Scope EOL <stat_list> End Scope EOF )
   = { Declare, Function, Scope, Dim, Static, EOL } */
   if(token.id == DECLARE_ID || token.id == FUNCTION_ID || 
      token.id == SCOPE_ID || token.id == DIM_ID || token.id == STATIC_ID ||
      token.id == EOL_ID)
   {
      /* <prog> -> <decl_list> <scope-statement> EOF simulation */

      if(!decl_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* At the end of the declaration list, all declared functions should
         have also been defined */
      if(!all_functions_defined(&global_table))
      {
         HANDLE_ERROR(SEM_ERR, success);
      }

      /* Generate main label and main body local frame */
      generate(&instr_stack, SCOPE_ID);
      context = CONTEXT_MAIN;
      code_structure = CODE_STRUCTURE_LINEAR;

      if(token.id != SCOPE_ID)
      {
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after 'scope',"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      block_depth++;

      GET_TOKEN(token);

      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* At the end of a scope block, variables declared in it
         should become invalid */
      pop_block_vars(&local_table, block_depth);
      
      block_depth--;

      if(token.id != END_ID)
      {
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != SCOPE_ID)
      {
         error_msg(line_counter, "Expected 'scope' after 'end', received "
                   "%s\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);

      while(token.id == EOL_ID) GET_TOKEN(token);
      if(token.id != EOF)
      {
         error_msg(line_counter, "No other structures are allowed after"
                   " the end of the main program!\n");
         HANDLE_ERROR(SYNT_ERR, success);
      }
   }
   else
   {
      error_msg(line_counter, "Expected either: function declaration, "
                " function definition, variable declaration or scope."
                " received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <decl_list> -> <var_decl> <decl_list>
 * <decl_list> -> <func_decl> <decl_list>
 * <decl_list> -> <func_def> <decl_list>
 * <decl-list> -> <empty_statement> <decl-list>
 * <decl_list> -> ε
 */
bool decl_list()
{
   bool success = true;
   context = CONTEXT_DECL_LIST;

   /* Predict( <decl_list> -> <var_decl> <decl_list> ) = { Dim, Static } */
   if(token.id == DIM_ID || token.id == STATIC_ID)
   {
      /* <decl_list> -> <var_decl> <decl_list> simulation */

      if(!var_decl()) HANDLE_ERROR(SYNT_ERR, success);

      if(!decl_list()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <decl_list> -> <func_decl> <decl_list> ) = { Declare } */
   else if(token.id == DECLARE_ID)
   {
      /* <decl_list> -> <func_decl> <decl_list> simulation */

      if(!func_decl()) HANDLE_ERROR(SYNT_ERR, success);

      if(!decl_list()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <decl_list> -> <func_def> <decl_list> ) = { Function } */
   else if(token.id == FUNCTION_ID)
   {
      /* <decl_list> -> <func_def> <decl_list> simulation */

      if(!func_def()) HANDLE_ERROR(SYNT_ERR, success);

      if(!decl_list()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <decl-list> -> <empty_statement> <decl-list> ) = { EOL } */
   else if(token.id == EOL_ID)
   {
      /* <decl-list> -> <empty_statement> <decl-list> simulation */

      GET_TOKEN(token);
      while(token.id == EOL_ID) GET_TOKEN(token);
      //if(!empty_statement()) HANDLE_ERROR(SYNT_ERR, success);
      //Causes sigsegs with too many EOLS */

      if(!decl_list()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <decl_list> -> ε ) = { Scope } */
   else if(token.id == SCOPE_ID)
   {
      /* <decl_list> -> ε simulation */

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected either: function declaration, "
                " function definition, variable declaration or scope."
                " received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <func_decl> -> Declare Function id ( <param_list> ) As <type> EOL
 */
bool func_decl()
{
   bool success = true;

   symbol_t function_entry;
   function_entry.type = SYMB_FUNC;
   function_entry.s.func.state = FUNC_STATE_DECLARED;
   token_t param_num;
   token_t *params;

   /* Predict( <func_decl> -> Declare Function id ( <param_list> ) As <type>
      EOL ) = { Declare } */
   if(token.id == DECLARE_ID)
   {
      /* <func_decl> -> Declare Function id ( <param_list> ) As <type> EOL
      simulation */

      GET_TOKEN(token);
      if(token.id != FUNCTION_ID)
      {
         error_msg(line_counter, "Expected 'function' after 'declare',"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != ID_ID)
      {
         error_msg(line_counter, "Expected identifier after 'function',"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      /* Saving the name of the function for later */
      function_entry.id = token.a.str;

      GET_TOKEN(token);
      if(token.id != LEFT_BRACKET_ID)
      {
         error_msg(line_counter, "Expected '(' after '%s',"
                   " received '%s'\n", function_entry.id, 
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!param_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <param_list> output:
         Number of parameters followed by a reversed list of parameters */
      param_num = tstack_toppop(&semStack);
      params = smalloc(param_num.a.val_int * sizeof(token_t));
      if(params == NULL)
      {
         internal_error_msg("Failed to allocate memory!\n");
         HANDLE_ERROR(INTERNAL_ERR, success);
      }

      /* Save the parameter types */
      for(int i = param_num.a.val_int-1; i >= 0; i--)
      {
         params[i] = tstack_toppop(&semStack);
      }

      if(token.id != RIGHT_BRACKET_ID)
      {
         error_msg(line_counter, "Expected ')' after list of parameters,"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != AS_ID)
      {
         error_msg(line_counter, "Expected 'as' after ')',"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!type()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <type> output:
         Read type token */
      token_t return_type = tstack_toppop(&semStack);
      function_entry.s.func.returnType = return_type.id;

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line after '%s',"
                   " received '%s'\n", token_to_string(return_type.id), 
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);

      /* Semantic checks begin */
      function_entry.s.func.param_count = param_num.a.val_int;

      /* Check for redeclaration attempt */
      if(stab_get_data(&global_table, function_entry.id) != NULL)
      {
         error_msg(line_counter-1, "Function '%s' redeclaration attempt!\n",
                   function_entry.id);
         HANDLE_ERROR(SEM_ERR, success);
      }

      function_entry.s.func.state = FUNC_STATE_DECLARED;
      function_entry.s.func.params = smalloc(param_num.a.val_int *
                                            sizeof(param_t));
      if(function_entry.s.func.params == NULL)
      {
         internal_error_msg("Failed to allocate memory!\n");
         HANDLE_ERROR(INTERNAL_ERR, success);
      }

      /* Save function parameters and generate their ifjcode17 names */
      token_t param_name_token;
      for(int i = 0; i < param_num.a.val_int; i++)
      {
         function_entry.s.func.params[i].type = params[i].type;
         param_name_token.a.str = params[i].a.str;
         SAFE_PUSH(instr_stack, param_name_token);
         function_entry.s.func.params[i].code_name = 
         generate(&instr_stack, PARAM_NAME_GEN_ID);
      }

      token_t func_name_token;
      func_name_token.a.str = function_entry.id;
      SAFE_PUSH(instr_stack, func_name_token);
      function_entry.s.func.code_label = generate(&instr_stack, 
                                                  FUNC_NAME_ID);

      /* Save the information about the function into the symbol table */
      if(stab_insert(&global_table, &function_entry))
      {
         internal_error_msg("Failed to allocate memory in the symbol table!\n");
         HANDLE_ERROR(INTERNAL_ERR, success);
      }
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <func_def> -> Function id ( <param_list> ) As <type> EOL <stat_list> 
 * End Function EOL
 */
bool func_def()
{
   bool success = true;
   context = CONTEXT_FUNCTION;

   symbol_t function_entry;
   function_entry.type = SYMB_FUNC;
   function_entry.s.func.state = FUNC_STATE_DEFINED;
   token_t param_num;
   token_t *params;

   /* Predict( <func_def> -> Function id ( <param_list> ) As <type> EOL 
      <stat_list> End Function EOL ) = { Function } */
   if(token.id == FUNCTION_ID)
   {
      /* <func_def> -> Function id ( <param_list> ) As <type> EOL <stat_list> 
         End Function EOL simulation */

      GET_TOKEN(token);
      if(token.id != ID_ID)
      {
         error_msg(line_counter, "Expected identifier after 'function',"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }
      current_function_name = function_entry.id = token.a.str;

      GET_TOKEN(token);
      if(token.id != LEFT_BRACKET_ID)
      {
         error_msg(line_counter, "Expected '(' after '%s',"
                   " received '%s'\n", current_function_name,
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!param_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* Clearing contents of any previous local table, no longer needed */
      stab_clear(&local_table);

      /* Semantic stack <param_list> output:
         Number of parameters followed by a reversed list of parameters */
      param_num = tstack_toppop(&semStack);
      params = smalloc(param_num.a.val_int * sizeof(token_t));
      if(params == NULL)
      {
         internal_error_msg("Failed to allocate memory!\n");
         HANDLE_ERROR(INTERNAL_ERR, success);
      }
      for(int i = param_num.a.val_int-1; i >= 0; i--)
      {
         params[i] = tstack_toppop(&semStack);
      }

      if(token.id != RIGHT_BRACKET_ID)
      {
         error_msg(line_counter, "Expected ')' after list of parameters,"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != AS_ID)
      {
         error_msg(line_counter, "Expected 'as' after ')',"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!type()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <type> output:
         Read type token */
      token_t return_type = tstack_toppop(&semStack);
      function_entry.s.func.returnType = return_type.id;

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line after '%s',"
                   " received '%s'\n", token_to_string(return_type.id), 
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      /* Semantic checks */
      symbol_t *found_entry;
      int semantic_action;
      found_entry = stab_get_data(&global_table, function_entry.id);
      if(found_entry == NULL)
      {
         semantic_action = FUNC_DEF_ACTION_NEW_ENTRY;  
      }
      else
      {
         /* Check if a variable with this name already exists */
         if(found_entry->type == SYMB_VAR)
         {
            error_msg(line_counter-1,"A variable with the function name '%s' "
                       "you are trying to define already exists!\n", 
                       found_entry->id);
            HANDLE_ERROR(SEM_ERR, success);
         }
         else
         {
            /* Check function redefinition attempt */
            if(found_entry->s.func.state == FUNC_STATE_DEFINED)
            {
               error_msg(line_counter-1, "Function '%s' redefinition "
                         "attempt!\n", found_entry->id);
               HANDLE_ERROR(SEM_ERR, success);
            }

            /* Check return type compatibility */
            if(found_entry->s.func.returnType !=
               function_entry.s.func.returnType)
            {
               error_msg(line_counter-1, "Function '%s' declaration and "
                         "definition return type do not match!\n", 
                         found_entry->id);
               HANDLE_ERROR(SEM_ERR, success);
            }

            /* Check if number of parameters match */
            if(found_entry->s.func.param_count !=
               param_num.a.val_int)
            {
               error_msg(line_counter-1, "Function '%s' declaration and "
                         "definition number of parameters do not match!\n", 
                         found_entry->id);
               HANDLE_ERROR(SEM_ERR, success);
            }

            for(int i = 0; i < found_entry->s.func.param_count; i++)
            {
               if(found_entry->s.func.params[i].type !=
                  params[i].type)
               {
                  error_msg(line_counter-1, "Function '%s' declaration and "
                         "definition parameter number %i types "
                         "do not match!\n", found_entry->id, i+1);
                  HANDLE_ERROR(SEM_ERR, success);
               }
            }

            semantic_action = FUNC_DEF_ACTION_UPDATE;
         }
      }

      if(semantic_action == FUNC_DEF_ACTION_NEW_ENTRY)
      {
         function_entry.s.func.param_count = param_num.a.val_int;
         function_entry.s.func.params = 
         smalloc(sizeof(param_t) * function_entry.s.func.param_count);
      }
      else
      {
         found_entry->s.func.state = FUNC_STATE_DEFINED;
      }

      token_t param_name_token;
      for(int i = 0; i < param_num.a.val_int; i++)
      {
         if(semantic_action == FUNC_DEF_ACTION_NEW_ENTRY)
         {
            function_entry.s.func.params[i].name = params[i].a.str;
            function_entry.s.func.params[i].type = params[i].type;
            param_name_token.a.str = params[i].a.str;
            SAFE_PUSH(instr_stack, param_name_token);
            function_entry.s.func.params[i].code_name = 
            generate(&instr_stack, PARAM_NAME_GEN_ID);
         }
         else
         {
            found_entry->s.func.params[i].name = params[i].a.str;
         }

         /* Check param name uniqueness */
         if(stab_get_data(&local_table, params[i].a.str) != NULL)
         {
            error_msg(line_counter, "Parameter with the name '%s' already"
                   " exists!\n", params[i].a.str);
            HANDLE_ERROR(SEM_ERR, success);
         }

         symbol_t* global_entry;
         global_entry = stab_get_data(&global_table, params[i].a.str);
         /* Param names cant collide with function names */
         if(global_entry != NULL)
         {
            if(global_entry->type == SYMB_FUNC)
            {
               error_msg(line_counter, "Function with the name '%s' already"
                   " exists!\n", params[i].a.str);
               HANDLE_ERROR(SEM_ERR, success);
            }
         }

         /* Insert parameter into local symtable for the function */
         /* Symbol initialisation */
         symbol_t param_entry;
         struct var_info param_var;
         param_entry.id = params[i].a.str;
         param_entry.type = SYMB_VAR;
         vdstack_init(&(param_entry.s.var_stack));

         /* Variable entry initialisation */
         param_var.type = params[i].type;
         param_var.depth = 1;
         param_var.scope_modifier = SCOPE_LOCAL;
         param_var.used = false;
         param_var.is_const = false;
         if(semantic_action == FUNC_DEF_ACTION_NEW_ENTRY)
         {
            param_var.code_name = function_entry.s.func.params[i].code_name;
         }
         else
         {
            param_var.code_name = found_entry->s.func.params[i].code_name;
         }

         /* Param insertion */
         vdstack_push(&(param_entry.s.var_stack), &param_var);
         if(stab_insert(&local_table, &param_entry))
         {
            internal_error_msg("Failed to allocate memory!\n");
            HANDLE_ERROR(INTERNAL_ERR, success);
         }
      }

      token_t function_token;
      function_token.a.str = function_entry.id;
      if(semantic_action == FUNC_DEF_ACTION_NEW_ENTRY)
      {
         SAFE_PUSH(instr_stack, function_token);
         function_entry.s.func.code_label = 
         generate(&instr_stack, FUNC_NAME_ID);
         function_token.a.str = function_entry.s.func.code_label;
         SAFE_PUSH(instr_stack, function_token);
         generate(&instr_stack, FUNCTION_ID);
         if(stab_insert(&global_table, &function_entry))
         {
            internal_error_msg("Failed to allocate memory!\n");
            HANDLE_ERROR(INTERNAL_ERR, success);
         }
      }
      else
      {
         function_token.a.str = found_entry->s.func.code_label;
         SAFE_PUSH(instr_stack, function_token);
         generate(&instr_stack, FUNCTION_ID);
      }

      GET_TOKEN(token);

      /* Function code starts linearly */
      code_structure = CODE_STRUCTURE_LINEAR;
      block_depth++;

      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);

      block_depth--;

      if(token.id != END_ID)
      {
         error_msg(line_counter, "Expected 'end' after list of statements,"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != FUNCTION_ID)
      {
         error_msg(line_counter, "Expected 'function' after 'end',"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after 'function',"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      function_token.id = return_type.id;
      SAFE_PUSH(instr_stack, function_token);

      /* Generate default return in a function */
      generate(&instr_stack, FUNC_END_ID);

      GET_TOKEN(token);

      /* Clear the symtable for future code */
      stab_clear(&local_table);

   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <var_decl> -> Dim <opt-scope-modifier> id As <type> <opt-initialiser> EOL
 * <var-decl> -> Static id as <type> <opt-initialiser> EOL
 */
bool var_decl()
{
   bool success = true;

   symbol_t var_entry;
   var_entry.type = SYMB_VAR;
   token_t modifier, name_token, type_token, init_expr;

   /* Predict( <var_decl> -> Dim <opt-scope-modifier> id As <type> 
      <opt-initialiser> EOL ) = { Dim } */
   if(token.id == DIM_ID)
   {
      /* <var_decl> -> Dim <opt-scope-modifier> id As <type> 
         <opt-initialiser> EOL simulation */

      GET_TOKEN(token);
      if(!opt_scope_modifier()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <opt_scope_modifier> output:
         Read scope modifier or SCOPE_LOCAL */
      modifier = tstack_toppop(&semStack);

      if(token.id != ID_ID)
      {
         error_msg(line_counter, "Expected identifier after 'dim',"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      name_token = token;

      GET_TOKEN(token);
      if(token.id != AS_ID)
      {
         error_msg(line_counter, "Expected 'as' after '%s',"
                   " received '%s'\n", name_token.a.str, 
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!type()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <type> output:
         Read type token */
      type_token = tstack_toppop(&semStack);

      if(!opt_initialiser()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <opt_initialiser> output:
         Initialising expression or NO_INIT */
      init_expr = tstack_toppop(&semStack);

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' at the end of "
                   "variable declaration, received '%s'\n", 
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
   }
   /* Predict( <var-decl> -> Static id as <type> <opt-initialiser> EOL ) = 
      { Static } */
   else if(token.id == STATIC_ID)
   {
      /* <var-decl> -> Static id as <type> <opt-initialiser> EOL simulation */

      /* Check context, static variables can only appear in functions */
      if(context != CONTEXT_FUNCTION && context != CONTEXT_MAIN)
      {
         error_msg(line_counter, "Static variable declarations can only"
                   " appear in functions!\n");
         HANDLE_ERROR(SEM_ERR, success);
      }
      modifier.id = SCOPE_STATIC;

      GET_TOKEN(token);
      if(token.id != ID_ID)
      {
         error_msg(line_counter, "Expected identifier after 'static',"
                   " received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      name_token = token;

      GET_TOKEN(token);
      if(token.id != AS_ID)
      {
         error_msg(line_counter, "Expected 'as' after '%s',"
                   " received '%s'\n", name_token.a.str,
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!type()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <type> output:
         Read type token */
      type_token = tstack_toppop(&semStack);

      if(!opt_initialiser()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <opt_initialiser> output:
         Initialising expression or NO_INIT */
      init_expr = tstack_toppop(&semStack);

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' at the end of "
                   "variable declaration, received '%s'\n", 
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

      /* Semantic check section begin */
   symbol_t *g_found_entry = stab_get_data(&global_table, name_token.a.str);
   symbol_t *l_found_entry = stab_get_data(&local_table, name_token.a.str);
   struct var_info var;
   var.type = type_token.id;
   var.depth = block_depth;
   var.scope_modifier = modifier.id;
   var.used = false;

   if(init_expr.id == NO_INIT)
   {
      init_expr.is_const = true;
      init_expr.type = var.type;
   }

   int semantic_action; //Tells if entry is updated or a new one created 
   symtab_t *table_to_use; //Which table will the entry be created in
   symbol_t *symbol_to_use; //Which existing entry will be updated

   /* Checks for cases where variables are declared in this decl list*/
   if(context == CONTEXT_DECL_LIST)
   {
      /* On global level, no duplication is allowed */
      if(g_found_entry != NULL)
      {
         error_msg(line_counter-1, "Identifier '%s' is already in use!\n",
                   name_token.a.str);
         HANDLE_ERROR(SEM_ERR, success);
      }

      /* Non-shared variables cannot appear in a declaration list */
      if(var.scope_modifier != SCOPE_SHARED)
      {
         error_msg(line_counter-1, "Non-shared variables cannot appear "
                   " outside of main program or function body!\n");
         HANDLE_ERROR(SYNT_ERR, success);
      }

      /* Global variables cannot have non-constant initialisers */
      if(init_expr.id != NO_INIT && !init_expr.is_const)
      {
         error_msg(line_counter-1, "Global variables cannot have "
                   " non-constant initialisers!\n");
         HANDLE_ERROR(OTHER_ERR, success);
      }

      semantic_action = VAR_DECL_ACTION_NEW_ENTRY;
      table_to_use = &global_table;
   }
   else
   {
      /* Shared variables can only be declared in declaration list */
      if(var.scope_modifier == SCOPE_SHARED)
      {
         error_msg(line_counter-1, "Global variables can only declared "
                   " outside of function bodies and main program!\n");
         HANDLE_ERROR(SYNT_ERR, success);
      }

      if(l_found_entry != NULL)
      {
         struct var_info *top_entry;
         top_entry = vdstack_top(&(l_found_entry->s.var_stack));

         /* Variable on the same depth level redeclaration check */
         if(top_entry->depth == block_depth)
         {
            error_msg(line_counter-1, "Variable '%s' duplicate "
                      "declaration!\n", name_token.a.str);
            HANDLE_ERROR(SEM_ERR, success);
         }

         semantic_action = VAR_DECL_ACTION_PUSH;
         symbol_to_use = l_found_entry;
      }
      else if(g_found_entry != NULL)
      {
         /* Check if this id isn't used for a function already */
         if(g_found_entry->type == SYMB_FUNC)
         {
            error_msg(line_counter-1, "Function with the name '%s' already"
               " exists!\n", name_token.a.str);
            HANDLE_ERROR(SEM_ERR, success);
         }

         semantic_action = VAR_DECL_ACTION_NEW_ENTRY;
         table_to_use = &local_table;
      }
      else
      {
         semantic_action = VAR_DECL_ACTION_NEW_ENTRY;
         table_to_use = &local_table;
      }
   }

   /* Check if initialiser type matches variable type */
   token_t conv;
   if(!find_conversion(var.type, init_expr.type, &conv.id))
   {
      error_msg(line_counter-1, "No suitable conversion from '%s' to '%s' "
                "exists!\n", token_to_string(init_expr.type), 
                token_to_string(type_token.id));
      HANDLE_ERROR(TYPE_COMP_ERR, success);
   }

   /* Convert initialiser type ot prevent bad var typing in ifjcode17 */
   if(init_expr.is_const)
   {
      convert_literal(&init_expr, &conv);
   }

   if(init_expr.is_const)
   {
      switch(var.type)
      {
         case INTEGER_ID:
            if(init_expr.id == NO_INIT)
               init_expr.a.val_int = 0;
            var.value.int_value = init_expr.a.val_int;
            break;
         case DOUBLE_ID:
            if(init_expr.id == NO_INIT)
               init_expr.a.val_real = 0.0;
            var.value.double_value = init_expr.a.val_real;
            break;
         case STRING_ID:
            if(init_expr.id == NO_INIT)
               init_expr.a.str = "";
            var.value.string_value = init_expr.a.str;
            break;
      }
      var.is_const = true;
   }
   else
   {
      var.value.tmp_value = init_expr.a.str;
      var.is_const = false;
   }

   if(modifier.id == SCOPE_STATIC || modifier.id == SCOPE_SHARED)
      var.is_const = false;

   name_token.id = modifier.id;
   name_token.is_const = false;
   SAFE_PUSH(instr_stack, name_token);
   init_expr.id = type_token.id;
   SAFE_PUSH(instr_stack, init_expr);
   SAFE_PUSH(instr_stack, conv);
   var.code_name = generate(&instr_stack, INIT_ID);

   /* Symbol table update */
   if(semantic_action == VAR_DECL_ACTION_NEW_ENTRY)
   {
      vdstack_init(&(var_entry.s.var_stack));
      var_entry.id = name_token.a.str;
      vdstack_push(&(var_entry.s.var_stack), &var);
      if(stab_insert(table_to_use, &var_entry))
      {
         internal_error_msg("Failed to allocate memory!\n");
         HANDLE_ERROR(INTERNAL_ERR, success);
      }
   }
   else if(semantic_action == VAR_DECL_ACTION_PUSH)
   {
      vdstack_push(&(symbol_to_use->s.var_stack), &var);
   }

   return success;
}

/**
 * Possible rules:
 * <param_list> -> <param> <param_list_cont>
 * <param_list> -> ε
 */
bool param_list()
{
   bool success = true;

   /* Predict( <param_list> -> <param> <param_list_cont> ) = { id } */
   if(token.id == ID_ID)
   {
      /* <param_list> -> <param> <param_list_cont> simulation */

      if(!param()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <param> output:
         Type and name of this parameter */
      //We do nothing as we want to collect params on the stack 

      /* Semantic stack <param_list_cont> input: 
         Number of already read parameters */
      token_t param_num;
      param_num.a.val_int = 1;
      SAFE_PUSH(semStack, param_num);

      if(!param_list_cont()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <param_list_cont> output:
         Number of parameters, followed by a reversed list of parameters */
      /* We do nothing as all the output information for <param_list> is 
         already on the stack */

   }
   /* Predict( <param_list> -> ε ) = { ) } */
   else if(token.id == RIGHT_BRACKET_ID)
   {
      /* <param_list> -> ε simulation */

      /* Semantic stack <param_list> output:
         Number of parameters, followed by a reversed list of parameters*/
      token_t param_num;
      param_num.a.val_int = 0;
      SAFE_PUSH(semStack, param_num);

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected parameter declaration or ')',"
          " received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <param> -> id As <type>
 */
bool param()
{
   bool success = true;

   /* Predict( <param> -> id As <type> ) = { id } */
   if(token.id == ID_ID)
   {
      /* <param> -> id As <type> simulation */

      token_t param_token;
      param_token = token;

      GET_TOKEN(token);
      if(token.id != AS_ID)
      {
         error_msg(line_counter, "Expected 'as' after '%s',"
                   " received '%s'\n", param_token.a.str,
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!type()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <type> output:
         Read type token */
      token_t type_token = tstack_toppop(&semStack);

      /* Semantick stack <param> output:
         Type and name of the parameter */
      param_token.type = type_token.id;
      SAFE_PUSH(semStack, param_token);
   }
   else
   {
      error_msg(line_counter, "Expected parameter declaration,"
          " received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <param_list_cont> -> , <param> <param_list_cont>
 * <param_list_cont> -> ε
 */
bool param_list_cont()
{
   bool success = true;

   /* Predict( <param_list_cont> -> , <param> <param_list_cont> ) = { , } */
   if(token.id == COMMA_ID)
   {
      /* <param_list_cont> -> , <param> <param_list_cont> simulation */

      /* Semantic stack <param_list_cont> input:
         Number of already read parameters */
      token_t param_num = tstack_toppop(&semStack);

      GET_TOKEN(token);
      if(!param()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <param> output:
         Type of the parameter */
      //We do nothing as we want to collect params on the stack

      /* Semantic stack <param_list_cont> input:
         Number of already read parameters */
      param_num.a.val_int++;
      SAFE_PUSH(semStack, param_num);

      if(!param_list_cont()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <param_list_cont> -> ε ) = { ) } */
   else if(token.id == RIGHT_BRACKET_ID)
   {
      /* <param_list_cont> -> ε simulation */

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected parameter declaration or ')',"
          " received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <opt_scope_modifier> -> Shared
 * <opt_scope_modifier> -> ε
 */
bool opt_scope_modifier()
{
   bool success = true;

   token_t modifier;

   /* Predict( <opt_scope_modifier> -> Shared ) = { Shared } */
   if(token.id == SHARED_ID)
   {
      /* <opt_scope_modifier> -> Shared simulation */

      /* Semantic stack <opt_scope_modifier> output:
         Read scope modifier or SCOPE_LOCAL */
      modifier.id = SCOPE_SHARED;
      SAFE_PUSH(semStack, modifier);

      GET_TOKEN(token);
   }
   /* Predict( <opt_scope_modifier> -> ε ) = { id } */
   else if(token.id == ID_ID)
   {
      /* <opt_scope_modifier> -> ε simulation */

      modifier.id = SCOPE_LOCAL;
      SAFE_PUSH(semStack, modifier);

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected 'shared' or identifier,"
          " received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <opt_initialiser> -> = <expr>
 * <opt_initialiser> -> ε
 */
bool opt_initialiser()
{
   bool success = true;

   /* Predict( <opt_initialiser> -> = <expr> ) = { = } */
   if(token.id == EQUALS_ID)
   {
      /* <opt_initialiser> -> = <expr> simulation */

      GET_TOKEN(token);
      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr> output:
         Final expression token */
      //We do nothing as the expression is already on the stack
   }
   /* Predict( <opt_initialiser> -> ε ) = { EOL } */
   else if(token.id == EOL_ID)
   {
      /* <opt_initialiser> -> ε simulation */

      /* Semantic stack <opt_initialiser> output:
         Final expression token */
      token_t no_init;
      no_init.id = NO_INIT;
      SAFE_PUSH(semStack, no_init);

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected initialiser or 'End-of-Line',"
          " received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <type> -> Integer
 * <type> -> Double
 * <type> -> String
 */
bool type()
{
   bool success = true;

   /* Predict( <type> -> Integer ) = { Integer } */
   if(token.id == INTEGER_ID)
   {
      /* <type> -> Integer simulation */

      /* Semantic stack <type> output:
         Read type token */
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
   }
   /* Predict( <type> -> Double ) = { Double } */
   else if(token.id == DOUBLE_ID)
   {
      /* <type> -> Double simulation */

      /* Semantic stack <type> output:
         Read type token */
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
   }
   /* Predict( <type> -> String ) = { String } */
   else if(token.id == STRING_ID)
   {
      /* <type> -> String simulation */

      /* Semantic stack <type> output:
         Read type token */
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
   }
   else
   {
      error_msg(line_counter, "Expected 'integer', 'double' or 'string',"
          " received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <stat_list> -> <statement> <stat_list>
 * <stat_list> -> ε
 */
bool stat_list()
{
   bool success = true;

   /* Predict( <stat-list> -> <statement> <stat-list> ) = 
      { id, Input, Print, Scope, If, For, Do, Return, Dim, Static, EOL,
        exit, continue } */
   if(token.id == ID_ID || token.id == INPUT_ID || token.id == PRINT_ID || 
      token.id == SCOPE_ID || token.id == IF_ID || token.id == FOR_ID || 
      token.id == DO_ID || token.id == RETURN_ID || token.id == DIM_ID || 
      token.id == STATIC_ID || token.id == CONTINUE_ID || 
      token.id == EXIT_ID)
   {
      /* <stat_list> -> <statement> <stat_list> simulation */

      if(!statement()) HANDLE_ERROR(SYNT_ERR, success);

      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Sigseg fix */
   else if(token.id == EOL_ID)
   {
      GET_TOKEN(token);
      while(token.id == EOL_ID) GET_TOKEN(token);
      //if(!empty_statement()) HANDLE_ERROR(SYNT_ERR, success);
      //Causes sigsegs with too many EOLS */

      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);
   }
   /* Predict( <stat-list> -> ε ) = { End, elseif, else, next, loop} */
   else if(token.id == END_ID || token.id == ELSEIF_ID || 
      token.id == ELSE_ID || token.id == NEXT_ID || token.id == LOOP_ID)
   {
      /* <stat_list> -> ε simulation */

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected a statement,"
          " received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <statement> -> <assignment>
 * <statement> -> <read_statement>
 * <statement> -> <print_statement>
 * <statement> -> <scope_statement>
 * <statement> -> <selection_statement>
 * <statement> -> <iteration_statement>
 * <statement> -> <return_statement>
 * <statement> -> <empty_statement>
 * <statement> -> <var_decl>
 * <statement> -> <iteration_control_statement> 
 */
bool statement()
{
   bool success = true;

   /* Predict( <statement> -> <assignment> ) = { id } */
   if(token.id == ID_ID)
   {
      /* <statement> -> <assignment> simulation */

      if(!assignment()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <statement> -> <read_statement> ) = { Input } */
   else if(token.id == INPUT_ID)
   {
      /* <statement> -> <read_statement> simulation */

      if(!read_statement()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <statement> -> <print_statement> ) = { Print } */
   else if(token.id == PRINT_ID)
   {
      /* <statement> -> <print_statement> simulation */

      if(!print_statement()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <statement> -> <scope-statement> ) = { Scope } */
   else if(token.id == SCOPE_ID)
   {
      /* <statement> -> <scope-statement> simulation */

      if(!scope_statement()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <statement> -> <selection_statement> ) = { If } */
   else if(token.id == IF_ID)
   {
      /* <statement> -> <selection_statement> simulation */

      if(!selection_statement()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <statement> -> <iteration_statement> ) = { Do, For } */
   else if(token.id == DO_ID || token.id == FOR_ID)
   {
      /* <statement> -> <iteration_statement> simulation */

      if(!iteration_statement()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <statement> -> <return_statement> ) = { return } */
   else if(token.id == RETURN_ID)
   {
      /* <statement> -> <return_statement> simulation */

      if(!return_statement()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <statement> -> <empty_statement> ) = { EOL } */
   else if(token.id == EOL_ID)
   {
      /* <statement> -> <empty_statement> simulation */

      if(!empty_statement()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <statement> -> <var-decl> ) = { Dim, Static } */
   else if(token.id == DIM_ID || token.id == STATIC_ID)
   {
      /* <statement> -> <var-decl> simulation */

      if(!var_decl()) HANDLE_ERROR(SYNT_ERR, success);

   }
      /* Predict( <statement> -> <iteration_control_statement> ) = 
         { continue, exit } */
   else if(token.id == CONTINUE_ID || token.id == EXIT_ID)
   {
      /* <statement> -> <iteration_control_statement>  simulation */

      if(!iteration_control_statement()) HANDLE_ERROR(SYNT_ERR, success);

   }
   else
   {
      error_msg(line_counter, "Expected a statement,"
                " received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <assignment> -> id = <expr> EOL
 */
bool assignment()
{
   bool success = true;

   token_t name, right;
   /* Predict( <assignment> -> id = <expr> EOL ) = { id } */
   if(token.id == ID_ID)
   {
      /* <assignment> -> id = <expr> EOL simulation */

      /* Saving the ID for semantic checks */
      name = token;

      GET_TOKEN(token);
      if(token.id != EQUALS_ID)
      {
         error_msg(line_counter, "Expected '=' after '%s',"
                   " received '%s'\n", name.a.str,
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr> output:
         Final expression */
      right = tstack_toppop(&semStack);

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' at the end of the"
                   " assigment, received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);

      /* Semantic checks */
      symbol_t *var_entry;
      /* Check variable declaration */
      if(!check_variable_declaration(name.a.str, &var_entry))
      {
          HANDLE_ERROR(SEM_ERR, success);
      }

      /* Check type compatibility */
      struct var_info *var = vdstack_top(&(var_entry->s.var_stack));
      token_t conv;
      if(!find_conversion(var->type, right.type, &(conv.id)))
      {
         error_msg(line_counter-1, "No suitable conversion from '%s' to '%s' "
                   "exists!\n", token_to_string(right.type), 
                  token_to_string(var->type));
         HANDLE_ERROR(TYPE_COMP_ERR, success);
      }

      /* Can we optimise this assigment internally? */
      if(right.is_const && code_structure == CODE_STRUCTURE_LINEAR &&
         var->scope_modifier != SCOPE_SHARED)
      {
         cp_val_from_token(&right, var);
         var->is_const = true;
      }
      else
      {
         var->is_const = false;
      }

      /* Literal conversion is handled internally */
      if(right.is_const)
      {
         convert_literal(&right, &conv);
      }

      name.a.str = var->code_name;
      name.is_const = false;
      
      /* Push generation information onto the stack */
      SAFE_PUSH(instr_stack, name);
      SAFE_PUSH(instr_stack, right);
      SAFE_PUSH(instr_stack, conv);

      /* Generate the assigment */
      generate(&instr_stack, ASS_ID);
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <read_statement> -> Input id EOL
 */
bool read_statement()
{
   bool success = true;

   token_t name;

   /* Predict( <read_statement> -> Input id EOL ) = { Input } */
   if(token.id == INPUT_ID)
   {
      /* <read_statement> -> Input id EOL simulation */

      GET_TOKEN(token);
      if(token.id != ID_ID)
      {
         error_msg(line_counter, "Expected identifier after 'input',"
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      /* Save the ID for semchecks */
      name = token;

      GET_TOKEN(token);
      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after '%s',"
                   "received '%s'\n", name.a.str,
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);

      /* Semantic checks */

      symbol_t *var_entry;
      /* Check variable declaration */
      if(!check_variable_declaration(name.a.str, &var_entry))
      {
          HANDLE_ERROR(SEM_ERR, success);
      }

      struct var_info *var = vdstack_top(&(var_entry->s.var_stack));

      var->used = true;
      var->is_const = false;

      name.a.str = var->code_name;
      name.type = var->type;

      /* Push read generation data */
      SAFE_PUSH(instr_stack, name);

      /* Generate read */
      generate(&instr_stack, INPUT_ID);

   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <print_statement> -> print <expr> ; <expr_list> EOL
 */
bool print_statement()
{
   bool success = true;

   /* Predict( <print_statement> -> print <expr> ; <expr_list> EOL ) = 
      { print } */
   if(token.id == PRINT_ID)
   {
      /* <print_statement> -> print <expr> ; <expr_list> EOL simulation */

      GET_TOKEN(token);
      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr> output:
         Final expression */
      //We do nothing as we want to collect the expressions on the stack

      if(token.id != SEMICOLON_ID)
      {
         error_msg(line_counter, "Expected ';' after expression,"
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);

      /* Semantic stack <expr_list> input:
         Number of read expressions so far */
      token_t expr_num;
      expr_num.a.val_int = 1;
      SAFE_PUSH(semStack, expr_num);

      if(!expr_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr_list> output:
         Number of read expressions followed by a 
         reversed list of these expressions */
      /* We pass the expressions to print out to the generator */
      expr_num = tstack_toppop(&semStack);
      for(int i = 0; i < expr_num.a.val_int; i++)
      {
         token_t expr_to_write = tstack_toppop(&semStack);
         SAFE_PUSH(instr_stack, expr_to_write);
      }
      SAFE_PUSH(instr_stack, expr_num);
      generate(&instr_stack, PRINT_ID);

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after the end of"
                   "print statement, received '%s'\n", 
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <expr_list> -> <expr> ; <expr_list>
 * <expr_list> -> ε
 */
bool expr_list()
{
   bool success = true;

   /* Predict( <expr_list> -> <expr> ; <expr_list> ) =
      { id, (, int_literal, double_literal, string_literal,
      length, asc, substr, chr } */
   if(token.id == ID_ID || token.id == LEFT_BRACKET_ID || 
      token.id == INTEGER_LITERAL_ID || token.id == DOUBLE_LITERAL_ID || 
      token.id == STRING_LITERAL_ID || token.id == LENGTH_ID || 
      token.id == ASC_ID || token.id == SUBSTR_ID || token.id == CHR_ID)
   {
      /* <expr_list> -> <expr> ; <expr_list> simulation */

      /* Semantic stack <expr_list> input:
         Number of read expressions so far */
      token_t expr_num = tstack_toppop(&semStack);

      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr> output:
         Final expression */
      //We do nothing because we want to collect the expresions on the stack

      if(token.id != SEMICOLON_ID)
      {
         error_msg(line_counter, "Expected ';' after expression,"
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);

      /* Semantic stack <expr_list> input:
         Number of read expressions so far */
      expr_num.a.val_int++;
      SAFE_PUSH(semStack, expr_num);

      if(!expr_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr_list> output:
         Number of read expressions so far followed 
         by a reversed list of expressions */
      //We do nothing because the stack already contains the output information

   }
   /* Predict( <expr_list> -> ε ) = { EOL } */
   else if(token.id == EOL_ID)
   {
      /* <expr_list> -> ε simulation */

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected an expression to print,"
                "received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <scope_statement> -> Scope EOL <stat-list> End Scope EOL
 */
bool scope_statement()
{
   bool success = true;

   /* Predict( <scope_statement> -> Scope EOL <stat-list> End Scope EOL ) = 
      { Scope } */
   if(token.id == SCOPE_ID)
   {
      /* <scope_statement> -> Scope EOL <stat-list> End Scope EOL simulation */

      GET_TOKEN(token);
      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after 'scope',"
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      block_depth++;

      GET_TOKEN(token);

      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* At the end of a scope block, variables declared in it
         should become invalid */
      pop_block_vars(&local_table, block_depth);
      
      block_depth--;

      if(token.id != END_ID)
      {
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != SCOPE_ID)
      {
         error_msg(line_counter, "Expected 'scope' after 'end', received "
                   "%s\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after 'scope', "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <selection_statement> -> If <expr> then EOL 
 * <stat_list> <alternative_statement> End If EOL
 */
bool selection_statement()
{
   bool success = true;

   /* Predict( <selection_statement> -> If <expr> then EOL <stat_list> 
      <alternative_statement> End If EOL ) = { If } */
   if(token.id == IF_ID)
   {
      /* <selection_statement> -> If <expr> then EOL <stat_list> 
         <alternative_statement> End If EOL simulation */
      generate(&instr_stack, IF_ID);

      GET_TOKEN(token);
      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr> output:
         Final expression */
      token_t condition = tstack_toppop(&semStack);

      if(token.id != THEN_ID)
      {
         error_msg(line_counter, "Expected 'then' after condition, "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after 'then', "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      if(condition.type != BOOLEAN_ID)
      {
         error_msg(line_counter, "Conditional statement expression "
                   "must be boolean type!\n");
         HANDLE_ERROR(TYPE_COMP_ERR, success);
      }

      SAFE_PUSH(instr_stack, condition);
      generate(&instr_stack, THEN_ID);

      GET_TOKEN(token);

      int previous_structure = code_structure;
      code_structure = CODE_STRUCTURE_JUMP;
      block_depth++;

      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* After exiting the blocks, variable declared in it
         should become invalid */
      pop_block_vars(&local_table, block_depth);

      block_depth--;
      code_structure = previous_structure;

      if(!alternative_statement()) HANDLE_ERROR(SYNT_ERR, success);

      if(token.id != END_ID)
      {
         error_msg(line_counter, "Expected 'end' et the end of the "
                   "conditional statement, received '%s'\n", 
                   token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != IF_ID)
      {
         error_msg(line_counter, "Expected 'if' after 'end', "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after 'if', "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);

      generate(&instr_stack, END_IF_ID);
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <alternative_statement> -> elseif <expr> then EOL 
 *                            <stat_list> <alternative_statement>
 * <alternative_statement> -> else EOL <stat_list>
 * <alternative_statement> -> ε
 */
bool alternative_statement()
{
   bool success = true;

   /* Predict( <alternative_statement> -> elseif <expr> then EOL 
      <stat_list> <alternative_statement> ) = { elseif } */
   if(token.id == ELSEIF_ID)
   {
      /* <alternative_statement> -> elseif <expr> then EOL <stat_list> 
         <alternative_statement> simulation */

      generate(&instr_stack, ELSEIF_ID);
      
      GET_TOKEN(token);
      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr> output:
         Final expression */
      token_t condition = tstack_toppop(&semStack);

      if(token.id != THEN_ID)
      {
         error_msg(line_counter, "Expected 'then' after condition, "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after 'then', "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      if(condition.type != BOOLEAN_ID)
      {
         error_msg(line_counter, "Conditional statement expression "
                   "must be boolean type!\n");
         HANDLE_ERROR(TYPE_COMP_ERR, success);
      }
      
      SAFE_PUSH(instr_stack, condition);
      generate(&instr_stack, THEN_ID);

      GET_TOKEN(token);

      int previous_structure = code_structure;
      code_structure = CODE_STRUCTURE_JUMP;
      block_depth++;

      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* After exiting the blocks, variable declared in it
         should become invalid */
      pop_block_vars(&local_table, block_depth);

      block_depth--;
      code_structure = previous_structure;

      if(!alternative_statement()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <alternative_statement> -> else EOL <stat_list> ) = { else } */
   else if(token.id == ELSE_ID)
   {
      /* <alternative_statement> -> else EOL <stat_list> simulation */

      GET_TOKEN(token);
      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after 'else', "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);

      generate(&instr_stack, ELSE_ID);

      int previous_structure = code_structure;
      code_structure = CODE_STRUCTURE_JUMP;
      block_depth++;

      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* After exiting the blocks, variable declared in it
         should become invalid */
      pop_block_vars(&local_table, block_depth);

      block_depth--;
      code_structure = previous_structure;

   }
   /* Predict( <alternative_statement> -> ε ) = { End } */
   else if(token.id == END_ID)
   {
      /* <alternative_statement> -> ε simulation */

      return success;
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}
/**
 * Possible rules:
 * <iteration_statement> -> for id <opt_type_decl> = 
 * <expr> to <expr> <opt_step> EOL <stat_list> Next <opt_id> EOL
 * <iteration_statement> -> do <do_cycle> EOL
 */
bool iteration_statement()
{
   bool success = true;

   token_t name;

   /* Predict( <iteration_statement> -> for id <opt_type_decl> = 
      <expr> to <expr> <opt_step> EOL <stat_list> Next <opt_id> EOL ) =
      { for } */
   if(token.id == FOR_ID)
   {
      /* <iteration_statement> -> for id <opt_type_decl> = <expr> to <expr> 
         <opt_step> EOL <stat_list> Next <opt_id> EOL simulation */

      /* Save information about the cycle were entering */

      cycle_depth++;

      block_depth++;

      /* Generate internal labels for cycle structure */
      generate(&instr_stack, FOR_ID);

      /* Read label ID in ifjcode17 to utilise in exit/continue */
      token.type = tstack_top(&instr_stack).type;
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
      if(token.id != ID_ID)
      {
         error_msg(line_counter, "Expected identifier after 'for', "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      name = token;

      GET_TOKEN(token);
      if(!opt_type_decl()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <opt_type_decl> output:
         Read type or NO_DECL */
      token_t type_decl = tstack_toppop(&semStack);

      struct var_info *var;

      if(type_decl.id == NO_DECL)
      {
         symbol_t *symbol_to_use;
         if(!check_variable_declaration(name.a.str, &symbol_to_use))
         {
            HANDLE_ERROR(SEM_ERR, success);
         }

         var = vdstack_top(&(symbol_to_use->s.var_stack));
         var->is_const = false;
      }
      else
      {
         symbol_t *symbol_to_use;
         symbol_t new_entry;
         new_entry.type = SYMB_VAR;
         new_entry.id = name.a.str;
         vdstack_init(&(new_entry.s.var_stack));

         symbol_t *g_found_entry = stab_get_data(&global_table, name.a.str);
         symbol_t *l_found_entry = stab_get_data(&local_table, name.a.str);
         if(g_found_entry != NULL)
         {
            if(g_found_entry->type == SYMB_FUNC)
            {
               error_msg(line_counter, "Function with the name '%s' already",
                   " exists!\n", name.a.str);
               HANDLE_ERROR(OTHER_ERR, success);
            }
         }

         if(l_found_entry != NULL)
            symbol_to_use = l_found_entry;
         else
         {
            if(stab_insert(&local_table, &new_entry))
            {
               internal_error_msg("Failed to allocate memory!\n");
               HANDLE_ERROR(INTERNAL_ERR, success);
            }

            symbol_to_use = stab_get_data(&local_table, name.a.str);
         }

         struct var_info it_var;
         it_var.type = type_decl.id;
         it_var.depth = block_depth;
         it_var.scope_modifier = SCOPE_LOCAL;
         it_var.is_const = false;
         it_var.used = false;
         vdstack_push(&(symbol_to_use->s.var_stack), &it_var);
         var = vdstack_top(&(symbol_to_use->s.var_stack));
      }

      if(token.id != EQUALS_ID)
      {
         error_msg(line_counter, "Expected '=' after variable declaration, "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr> output:
         Final expression */
      token_t start = tstack_toppop(&semStack);

      if(token.id != TO_ID)
      {
         error_msg(line_counter, "Expected 'to' after '=', "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      token_t end = tstack_toppop(&semStack);

      if(!opt_step()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <opt_step> output:
         Step expression */
      token_t step = tstack_toppop(&semStack);

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after for "
                   "declaration, received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      token_t conv_start;
      token_t conv_end;
      token_t conv_step;

      /* Check starting expression type compatibility */
      if(!find_conversion(var->type, start.type, &conv_start.id))
      {
         error_msg(line_counter-1, "No suitable conversion from '%s' to '%s' "
                "exists!\n", token_to_string(var->type), 
                token_to_string(start.type));
         HANDLE_ERROR(TYPE_COMP_ERR, success);
      }

      if(start.is_const)
      {
         convert_literal(&start, &conv_start);
      }


      /* Check end expresison type compatibility */
      if(!find_conversion(var->type, end.type, &conv_end.id))
      {
         error_msg(line_counter-1, "No suitable conversion from '%s' to '%s' "
                "exists!\n", token_to_string(var->type), 
                token_to_string(end.type));
         HANDLE_ERROR(TYPE_COMP_ERR, success);
      }

      if(end.is_const)
      {
         convert_literal(&end, &conv_end);
      }

      /* Check step expression type compatbility */
      if(!find_conversion(var->type, step.type, &conv_step.id))
      {
         error_msg(line_counter-1, "No suitable conversion from '%s' to '%s' "
                "exists!\n", token_to_string(var->type), 
                token_to_string(step.type));
         HANDLE_ERROR(TYPE_COMP_ERR, success);
      }

      if(step.is_const)
      {
         convert_literal(&step, &conv_step);
      }

      name.id = SCOPE_LOCAL;

      /* Initialise the iterator */
      SAFE_PUSH(instr_stack, name);
      SAFE_PUSH(instr_stack, start);
      SAFE_PUSH(instr_stack, conv_start);
      var->code_name = generate(&instr_stack, INIT_ID);

      /* Generate for structure */
      token_t iterator_code;
      iterator_code.a.str = var->code_name;
      iterator_code.is_const = false;

      SAFE_PUSH(instr_stack, iterator_code);
      SAFE_PUSH(instr_stack, end);
      SAFE_PUSH(instr_stack, step);
      SAFE_PUSH(instr_stack, conv_end);
      SAFE_PUSH(instr_stack, conv_step);

      generate(&instr_stack, MAKE_FOR_ID);

      int previous_structure = code_structure;
      code_structure = CODE_STRUCTURE_JUMP;

      GET_TOKEN(token);
      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);

      code_structure = previous_structure;

      if(token.id != NEXT_ID)
      {
         error_msg(line_counter, "Expected 'next' after the end of for cycle, "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!opt_id()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <opt_id> output:
         Read ID or NO_ID */
      token_t opt_id = tstack_toppop(&semStack);

      if(opt_id.id != NO_ID)
      {
         /* ID after next must be the iterator variable */
         if(strcmp(opt_id.a.str, name.a.str))
         {
            error_msg(line_counter, "For/Next variable mismatch!\n");
            HANDLE_ERROR(OTHER_ERR, success);
         }
      }

      generate(&instr_stack, NEXT_ID);

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after 'next' " 
            "statement, received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      /* After exiting the blocks, variable declared in it
         should become invalid */
      pop_block_vars(&local_table, block_depth);


      block_depth--;

      /* Pop cycle information */
      cycle_depth--;
      tstack_pop(&semStack);

      GET_TOKEN(token);
   }
   /* Predict( <iteration_statement> -> do <do_cycle> EOL ) = { do } */
   else if(token.id == DO_ID)
   {
      /* <iteration_statement> -> do <do_cycle> EOL simulation */

      cycle_depth++;

      /* Generate initial do..loop structure */
      generate(&instr_stack, DO_ID);

      /* Read label ID in ifjcode17 to utilise in exit/continue */
      token.type = tstack_top(&instr_stack).type;
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);

      int previous_structure = code_structure;
      code_structure = CODE_STRUCTURE_JUMP;

      if(!do_cycle()) HANDLE_ERROR(SYNT_ERR, success);

      code_structure = previous_structure;

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after 'do', "
                   "received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      cycle_depth--;
      tstack_pop(&semStack);

      GET_TOKEN(token);
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <opt_type_decl> -> as <type>
 * <opt_type_decl> -> ε
 */
bool opt_type_decl()
{
   bool success = true;

   token_t type_decl;

   /* Predict( <opt_type_decl> -> as <type> ) = { as } */
   if(token.id == AS_ID)
   {
      /* <opt_type_decl> -> as <type> simulation */

      GET_TOKEN(token);
      if(!type()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <type> output:
         Read type */
      //We do nothing as the desired output is already on the stack */

   }
   /* Predict( <opt_type_decl> -> ε ) = { = } */
   else if(token.id == EQUALS_ID)
   {
      /* <opt_type_decl> -> ε simulation */

      type_decl.id = NO_DECL;
      SAFE_PUSH(semStack, type_decl);

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected 'as' or '=' after 'for', "
                "received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <opt_step> -> step <expr>
 * <opt_step> -> ε
 */
bool opt_step()
{
   bool success = true;

   /* Predict( <opt_step> -> step <expr> ) = { step } */
   if(token.id == STEP_ID)
   {
      /* <opt_step> -> step <expr> simulation */

      GET_TOKEN(token);
      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr> output:
         Final expression */
      //We do nothing because the desired output is already on the stack

   }
   /* Predict( <opt_step> -> ε ) = { EOL } */
   else if(token.id == EOL_ID)
   {
      /* <opt_step> -> ε simulation */

      /* Semantic stack <opt_step> output:
         Step expression */
      token_t step_expr;
      step_expr.id = EXPR_ID;
      step_expr.type = INTEGER_ID;
      step_expr.is_const = true;
      step_expr.a.val_int = 1;

      SAFE_PUSH(semStack, step_expr);

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected 'End-of-Line' or 'step' after for "
                "declaration ,received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <opt_id> -> id
 * <opt_id> -> ε
 */
bool opt_id()
{
   bool success = true;

   /* Predict( <opt_id> -> id ) = { id } */
   if(token.id == ID_ID)
   {
      /* <opt_id> -> id simulation */

      /* Semantic stack <opt_id> output:
         Read ID or NO_ID */
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
   }
   /* Predict( <opt_id> -> ε ) = { EOL } */
   else if(token.id == EOL_ID)
   {
      /* <opt_id> -> ε simulation */

      token_t id;
      id.id = NO_ID;

      SAFE_PUSH(semStack, id);

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected 'End-of-Line' or 'identifier' "
                "after 'next' received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <do_cycle> -> EOL <stat_list> Loop <opt_cond>
 * <do_cycle> -> <cond> EOL <stat_list> Loop
 */
bool do_cycle()
{
   bool success = true;

   /* Predict( <do_cycle> -> EOL <stat_list> Loop <opt_cond> ) = { EOL } */
   if(token.id == EOL_ID)
   {
      /* <do_cycle> -> EOL <stat_list> Loop <opt_cond> simulation */

      block_depth++;

      GET_TOKEN(token);
      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* After exiting the blocks, variable declared in it
         should become invalid */
      pop_block_vars(&local_table, block_depth);

      block_depth--;

      if(token.id != LOOP_ID)
      {
         error_msg(line_counter, "Expected 'loop' after the end of cycle"
                   ", received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!opt_cond()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <opt_cond> output:
         Condition expression and do mode, or NO_COND */
      token_t condition = tstack_toppop(&semStack);
      token_t do_mode;

      if(condition.id != NO_COND)
      {
         do_mode = tstack_toppop(&semStack);

         /* Conditions must be booleans */
         if(condition.type != BOOLEAN_ID)
         {
            error_msg(line_counter, "Conditional statement expression "
                   "must be boolean type!\n");
            HANDLE_ERROR(TYPE_COMP_ERR, success);
         }

         /* Condition checking generation */
         SAFE_PUSH(instr_stack, do_mode);
         SAFE_PUSH(instr_stack, condition);

         generate(&instr_stack, MAKE_LOOP_ID);

      }

      /* Finish loop generation */
      generate(&instr_stack, LOOP_ID);
   }
   /* Predict( <do_cycle> -> <cond> EOL <stat_list> Loop ) = 
      { until, while } */
   else if(token.id == UNTIL_ID || token.id == WHILE_ID)
   {
      /* <do_cycle> -> <cond> EOL <stat_list> Loop simulation */

      if(!cond()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <cond> output:
         Condition and do mode */
      token_t condition = tstack_toppop(&semStack);
      token_t do_mode = tstack_toppop(&semStack);

      if(condition.type != BOOLEAN_ID)
      {
         error_msg(line_counter, "Conditional statement expression "
                   "must be boolean type!\n");
         HANDLE_ERROR(TYPE_COMP_ERR, success);
      }

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after conditional "
                   "statement, received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      /* Condition checking generation */
      SAFE_PUSH(instr_stack, do_mode);
      SAFE_PUSH(instr_stack, condition);

      generate(&instr_stack, MAKE_LOOP_ID);

      block_depth++;

      GET_TOKEN(token);
      if(!stat_list()) HANDLE_ERROR(SYNT_ERR, success);

      if(token.id != LOOP_ID)
      {
         error_msg(line_counter, "Expected 'loop' after the end of cycle"
                   ", received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);

      /* After exiting the blocks, variable declared in it
         should become invalid */
      pop_block_vars(&local_table, block_depth);

      block_depth--;

      /* Finish do..loop cycle */
      generate(&instr_stack, LOOP_ID);

   }
   else
   {
      error_msg(line_counter, "Expected 'End-of-Line' or loop condition "
                "after 'do', received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <opt_cond> -> <cond>
 * <opt_cond> -> ε
 */
bool opt_cond()
{
   bool success = true;

   /* Predict( <opt_cond> -> <cond> ) = { until, while } */
   if(token.id == UNTIL_ID || token.id == WHILE_ID)
   {
      /* <opt_cond> -> <cond> simulation */

      if(!cond()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <cond> output:
         Condition and do-mode */
      //We do nothing as the desired output is already on the stack */

   }
   /* Predict( <opt_cond> -> ε ) = { EOL } */
   else if(token.id == EOL_ID)
   {
      /* <opt_cond> -> ε simulation */

      token_t cond;
      cond.id = NO_COND;

      SAFE_PUSH(semStack, cond);

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected 'End-of-Line' or loop condition "
                "after 'loop', received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <cond> -> <do_mode> <expr>
 */
bool cond()
{
   bool success = true;

   /* Predict( <cond> -> <do_mode> <expr> ) = { until, while } */
   if(token.id == UNTIL_ID || token.id == WHILE_ID)
   {
      /* <cond> -> <do_mode> <expr> simulation */

      if(!do_mode()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <do_mode> output:
         do mode */
      //We do nothing as we want to have the do mode on the stack in output

      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <expr> output:
         Final expression */
      //We do nothing as the desired output is already on the stack */
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <do_mode> -> until
 * <do_mode> -> while
 */
bool do_mode()
{
   bool success = true;

   /* Predict( <do_mode> -> until ) = { until } */
   if(token.id == UNTIL_ID)
   {
      /* <do_mode> -> until simulation */

      /* Semantic stack <do_mode> output:
         Do mode */
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
   }
   /* Predict( <do_mode> -> while ) = { while } */
   else if(token.id == WHILE_ID)
   {
      /* <do_mode> -> while simulation */

      /* Semantic stack <do_mode> output:
         Do mode */
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <return_statement> -> return <expr> EOL
 */
bool return_statement()
{
   bool success = true;

   /* Predict( <return_statement> -> return <expr> EOL ) = { return } */
   if(token.id == RETURN_ID)
   {
      /* <return_statement> -> return <expr> EOL simulation */

      /* Return can only appear in functions */
      if(context != CONTEXT_FUNCTION)
      {
         error_msg(line_counter, "Return statements can only"
                   " appear in functions!\n");
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      if(!expr()) HANDLE_ERROR(SYNT_ERR, success);
      
      /* Semantic stack <expr> output:
         Final expression */
      token_t ret_val = tstack_toppop(&semStack);

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after return "
                "statement, received '%s'\n", token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);
      token_t conv;
      symbol_t *function_entry;
      function_entry = stab_get_data(&global_table, current_function_name);

      if(!find_conversion(function_entry->s.func.returnType,
                      ret_val.type,
                      &conv.id))
      {
         error_msg(line_counter-1, "No suitable conversion from '%s' to '%s' "
                   "exists!\n", token_to_string(ret_val.type), 
                   token_to_string(function_entry->s.func.returnType));
         HANDLE_ERROR(TYPE_COMP_ERR, success);
      }

      if(ret_val.is_const)
      {
         convert_literal(&ret_val, &conv);
      }

      /*Prepare generation data */
      SAFE_PUSH(instr_stack, ret_val);
      SAFE_PUSH(instr_stack, conv);

      /*Generate return */
      generate(&instr_stack, RETURN_ID);
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <iteration_control_statement> -> <control_statement> 
 * <cycle_type> <cycle_type_list> EOL
 */
bool iteration_control_statement()
{
   bool success = true;

   /* Predict( <iteration_control_statement> -> <control_statement> 
      <cycle_type> <cycle_type_list> EOL ) = { Exit, Continue } */
   if(token.id == EXIT_ID || token.id == CONTINUE_ID)
   {
      /* <iteration_control_statement> -> <control_statement> 
         <cycle_type> <cycle_type_list> EOL simulation */

      if(!control_statement()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <control_statement> output:
         Control statement */
      token_t control = tstack_toppop(&semStack);

      if(!cycle_type()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <cycle_type> output:
         Cycle type */
      token_t cycle = tstack_toppop(&semStack);
      cycle.a.val_int = 1;

      /* Semantic stack <cycle_type_list> input:
         Initial cycle */
      SAFE_PUSH(semStack, cycle);

      if(!cycle_type_list()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <cycle_type_list> output:
         Cycle type with numbers of occurences */
      cycle = tstack_toppop(&semStack);

      if(token.id != EOL_ID)
      {
         error_msg(line_counter, "Expected 'End-of-Line' after %s statement "
                ", received '%s'\n", token_to_string(control.id),
                token_to_string(token.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      GET_TOKEN(token);

      /* Loop flow-control statements are not allowed outside of loops */
      if(cycle_depth == 0)
      {
         error_msg(line_counter, "Loop flow control statements are not"
                  " allowed outside of loops!\n");
         HANDLE_ERROR(SYNT_ERR, success);
      }

      token_t *cycle_array = smalloc(sizeof(token_t) * cycle_depth);
      if(cycle_array == NULL)
      {
         internal_error_msg("Failed to allocate memory!\n");
         HANDLE_ERROR(INTERNAL_ERR, success);
      }


      /* Load cycles from the stack */
      for(int i = 0; i < cycle_depth; i++)
      {
         cycle_array[i] = tstack_toppop(&semStack);
      }

      int counter = 0;
      int i = 0;
      for(; i < cycle_depth && cycle.a.val_int != counter; i++)
      {
         token_t c = cycle_array[i];
         if(c.id == FOR_ID || c.id == DO_ID)
         {
            if(c.id == cycle.id)
               counter++;
         }

      }

      /* There are more cycles listed in the statement than we are in */
      if(counter != cycle.a.val_int)
      {
         error_msg(line_counter, "There is no outer %s cycle to %s cycle!\n",
                   token_to_string(cycle.id), token_to_string(cycle.id));
      }

      /* Generate the control flow statement */
      SAFE_PUSH(instr_stack, cycle_array[i-1]);
      generate(&instr_stack, control.id);

      /* Return the cycles to the stack */
      for(int i = cycle_depth-1; i >= 0; i--)
      {
         SAFE_PUSH(semStack, cycle_array[i]);
      }
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <control_statement> -> Exit
 * <control_statement> -> Continue
 */
bool control_statement()
{
   bool success = true;

   /* Predict( <control_statement> -> Exit ) = { Exit } */
   if(token.id == EXIT_ID)
   {
      /* <control_statement> -> Exit simulation */

      /* Semantic stack <control_statement> output:
         Control statement */
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
   }
   /* Predict( <control_statement> -> Continue ) = { Continue } */
   else if(token.id == CONTINUE_ID)
   {
      /* <control_statement> -> Continue simulation */

      /* Semantic stack <control_statement> output:
         Control statement */
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
   }
   else
   {
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <cycle_type_list> -> , <cycle_type> <cycle_type_list>
 * <cycle_type_list> -> ε
 */
bool cycle_type_list()
{
   bool success = true;

   /* Predict( <cycle_type_list> -> , <cycle_type> <cycle_type_list> ) = { , } */
   if(token.id == COMMA_ID)
   {
      /* <cycle_type_list> -> , <cycle_type> <cycle_type_list> simulation */

      /* Semantic stack <cycle_type_list> input:
         Initial cycle type */
      token_t init_cycle = tstack_toppop(&semStack);

      GET_TOKEN(token);
      if(!cycle_type()) HANDLE_ERROR(SYNT_ERR, success);

      /* Semantic stack <cycle_type> output:
         Cycle type */
      token_t cycle = tstack_toppop(&semStack);

      if(cycle.id != init_cycle.id)
      {
         error_msg(line_counter, "Expected '%s' in control statement "
                   ", received '%s'\n", token_to_string(init_cycle.id),
                   token_to_string(cycle.id));
         HANDLE_ERROR(SYNT_ERR, success);
      }

      init_cycle.a.val_int++;

      SAFE_PUSH(semStack, init_cycle);

      if(!cycle_type_list()) HANDLE_ERROR(SYNT_ERR, success);

   }
   /* Predict( <cycle_type_list> -> ε ) = { EOL } */
   else if(token.id == EOL_ID)
   {
      /* <cycle_type_list> -> ε simulation */

      return success;
   }
   else
   {
      error_msg(line_counter, "Expected 'End-of-Line' or ',' in control "
                "statement, received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <cycle_type> -> for
 * <cycle_type> -> do
 */
bool cycle_type()
{
   bool success = true;

   /* Predict( <cycle_type> -> for ) = { for } */
   if(token.id == FOR_ID)
   {
      /* <cycle_type> -> for simulation */

      /* Semantic stack <cycle_type> output:
         Cycle type */
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
   }
   /* Predict( <cycle_type> -> do ) = { do } */
   else if(token.id == DO_ID)
   {
      /* <cycle_type> -> do simulation */

      /* Semantic stack <cycle_type> output:
         Cycle type */
      SAFE_PUSH(semStack, token);

      GET_TOKEN(token);
   }
   else
   {
      error_msg(line_counter, "Expected 'for' or 'do' in control "
                "statement, received '%s'\n", token_to_string(token.id));
      HANDLE_ERROR(SYNT_ERR, success);
   }

   return success;
}

/**
 * Possible rules:
 * <empty_statement> -> EOL
 */
bool empty_statement()
{
   bool success = true;

   /* Predict( <empty_statement> -> EOL ) = { EOL } */
   if(token.id == EOL_ID)
   {
      /* <empty_statement> -> EOL simulation */

      GET_TOKEN(token);
   }

   return success;
}

void pop_block_vars(symtab_t *table, int depth)
{
   stab_iterator_t it;
   stab_iterate_set(table, &it);
   symbol_t *to_remove = NULL;
   while(it.item != NULL)
   {
      if(it.item->data.type == SYMB_VAR)
      {
         struct var_info *var = vdstack_top(&(it.item->data.s.var_stack));
         if(var->depth == depth)
            vdstack_pop(&(it.item->data.s.var_stack));
         if(vdstack_top(&(it.item->data.s.var_stack)) == NULL)
         {
            to_remove = &(it.item->data);
         }
      }
      stab_iterate_next(table, &it);
      if(to_remove != NULL)
      {
         stab_delete_symbol(table, to_remove->id);
         to_remove = NULL;
      }
   }
}

bool all_functions_defined(symtab_t *table)
{
   bool success = true;

   stab_iterator_t it;
   stab_iterate_set(table, &it);
   while(it.item != NULL)
   {
      if(it.item->data.type == SYMB_FUNC)
      {
         if(it.item->data.s.func.state != FUNC_STATE_DEFINED)
         {
            error_msg(line_counter, "Function '%s' has not been defined!\n",
                      it.item->data.id);
            HANDLE_ERROR(SEM_ERR, success);
         }
      }
      stab_iterate_next(table, &it);
   }

   return success;
}

bool integrate_builtin()
{
   bool success = true;

   int param_len;

   symbol_t function_entry;
   function_entry.type = SYMB_FUNC;
   function_entry.s.func.state = FUNC_STATE_DEFINED;

   /* Length insertion */
   function_entry.id = "length";
   function_entry.s.func.returnType = INTEGER_ID;
   function_entry.s.func.param_count = 1;
   function_entry.s.func.params = 
   smalloc(function_entry.s.func.param_count * sizeof(param_t));
   if(function_entry.s.func.params == NULL)
   {
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   function_entry.s.func.params[0].type = STRING_ID;
   param_len = strlen("LF@str");
   function_entry.s.func.params[0].code_name = smalloc(param_len + 1);
   if(function_entry.s.func.params[0].code_name == NULL)
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   strncpy(function_entry.s.func.params[0].code_name, "LF@str", param_len + 1);
   
   if(stab_insert(&global_table, &function_entry))
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }

   /* Chr insertion */
   function_entry.id = "chr";
   function_entry.s.func.returnType = STRING_ID;
   function_entry.s.func.param_count = 1;
   function_entry.s.func.params = 
   smalloc(function_entry.s.func.param_count * sizeof(param_t));
   if(function_entry.s.func.params == NULL)
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   function_entry.s.func.params[0].type = INTEGER_ID;
   param_len = strlen("LF@char_val");
   function_entry.s.func.params[0].code_name = smalloc(param_len + 1);
   if(function_entry.s.func.params[0].code_name == NULL)
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   strncpy(function_entry.s.func.params[0].code_name, 
           "LF@char_val", param_len + 1);

   if(stab_insert(&global_table, &function_entry))
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }

   /* Asc insertion */
   function_entry.id = "asc";
   function_entry.s.func.returnType = INTEGER_ID;
   function_entry.s.func.param_count = 2;
   function_entry.s.func.params = 
   smalloc(function_entry.s.func.param_count * sizeof(param_t));
   if(function_entry.s.func.params == NULL)
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   function_entry.s.func.params[0].type = STRING_ID;
   param_len = strlen("LF@str");
   function_entry.s.func.params[0].code_name = smalloc(param_len + 1);
   if(function_entry.s.func.params[0].code_name == NULL)
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   strncpy(function_entry.s.func.params[0].code_name, 
           "LF@str", param_len + 1);

   function_entry.s.func.params[1].type = INTEGER_ID;
   param_len = strlen("LF@char_pos");
   function_entry.s.func.params[1].code_name = smalloc(param_len + 1);
   if(function_entry.s.func.params[1].code_name == NULL)
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   strncpy(function_entry.s.func.params[1].code_name, 
           "LF@char_pos", param_len + 1);
   if(stab_insert(&global_table, &function_entry))
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }

   /* Substr insertion */
   function_entry.id = "substr";
   function_entry.s.func.returnType = STRING_ID;
   function_entry.s.func.param_count = 3;
   function_entry.s.func.params = 
   smalloc(function_entry.s.func.param_count * sizeof(param_t));
   if(function_entry.s.func.params == NULL)
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   function_entry.s.func.params[0].type = STRING_ID;
   param_len = strlen("LF@str");
   function_entry.s.func.params[0].code_name = smalloc(param_len + 1);
   if(function_entry.s.func.params[0].code_name == NULL)
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   strncpy(function_entry.s.func.params[0].code_name, 
           "LF@str", param_len + 1);
   
   function_entry.s.func.params[1].type = INTEGER_ID;
   param_len = strlen("LF@start");
   function_entry.s.func.params[1].code_name = smalloc(param_len + 1);
   if(function_entry.s.func.params[1].code_name == NULL)
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   strncpy(function_entry.s.func.params[1].code_name, 
           "LF@start", param_len + 1);
   
   function_entry.s.func.params[2].type = INTEGER_ID;
   param_len = strlen("LF@length");
   function_entry.s.func.params[2].code_name = smalloc(param_len + 1);
   if(function_entry.s.func.params[2].code_name == NULL)
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   strncpy(function_entry.s.func.params[2].code_name, 
           "LF@length", param_len + 1);
   if(stab_insert(&global_table, &function_entry))
   {
      internal_error_msg("Failed to allocate memory!\n");
      HANDLE_ERROR(INTERNAL_ERR, success);
   }
   
   return success;
}

char *token_to_string(int id)
{
   
   switch(id)
   {
      case AND_ID:
         return "and";
      case AS_ID:
         return "as";
      case ASC_ID:
         return "asc";
      case BOOLEAN_ID:
         return "boolean";
      case CHR_ID:
         return "chr";
      case CONTINUE_ID:
         return "continue";
      case DECLARE_ID:
         return "declare";
      case DIM_ID:
         return "dim";
      case DO_ID:
         return "do";
      case DOUBLE_ID:
         return "double";
      case ELSE_ID:
         return "else";
      case ELSEIF_ID:
         return "elseif";
      case END_ID:
         return "end";
      case EXIT_ID:
         return "exit";
      case FALSE_ID:
         return "false";
      case FOR_ID:
         return "for";
      case FUNCTION_ID:
         return "function";
      case IF_ID:
         return "if";
      case INPUT_ID:
         return "input";
      case INTEGER_ID:
         return "integer";
      case LENGTH_ID:
         return "length";
      case LOOP_ID:
         return "loop";
      case NEXT_ID:
         return "next";
      case NOT_ID:
         return "not";
      case OR_ID:
         return "or";
      case PRINT_ID:
         return "print";
      case RETURN_ID:
         return "return";
      case SCOPE_ID:
         return "scope";
      case SHARED_ID:
         return "shared";
      case STATIC_ID:
         return "static";
      case STRING_ID:
         return "string";
      case SUBSTR_ID:
         return "substr";
      case THEN_ID:
         return "then";
      case TRUE_ID:
         return "true";
      case WHILE_ID:
         return "while";
      case LEFT_BRACKET_ID:
         return "(";
      case RIGHT_BRACKET_ID:
         return ")";
      case ASTERISK_ID:
         return "*";
      case SLASH_ID:
         return "/";
      case PLUS_ID:
         return "+";
      case MINUS_ID:
         return "-";
      case BACKSLASH_ID:
         return "\\";
      case EQUALS_ID:
         return "=";
      case GREATER_EQUAL_ID:
         return ">=";
      case LESS_EQUAL_ID:
         return "<=";
      case LESS_ID:
         return "<";
      case GREATER_ID:
         return ">";
      case NOT_EQUAL_ID:
         return "<>";
      case STRING_LITERAL_ID:
         return "string_literal";
      case COMMA_ID:
         return ",";
      case SEMICOLON_ID:
         return ";";
      case INTEGER_LITERAL_ID:
         return "integer literal";
      case DOUBLE_LITERAL_ID:
         return "double literal";
      case TO_ID:
         return "to";
      case UNTIL_ID:
         return "until";
      case STEP_ID:
         return "step";
      case ID_ID:
         return "identifier";
      case EOL_ID:
         return "End-of-Line";
      default:
         return "unknown sequence";
   }
}