/**
 * @file ifj17_api.h
 * @brief ifj17_api.h for IFJ Project 2017 AT vut.fit.vutbr.cz
 * @date 10. 11. 2017, last rev. 11.11.2017
 * @author Tomas Danis        - xdanis05@stud.fit.vutbr.cz
 * @author Patrik Goldschmidt - xgolds00@stud.fit.vutbr.cz
 *
 * Header file containing common constants, definitions, macros, declarations used
 * in most of the modules or used for communication between these modules.
 */

#ifndef IFJ17_API_H_INCLUDED
#define IFJ17_API_H_INCLUDED

#include <stdbool.h>

/*  Macros  section   */
#define EXIT_SUCCESS 0
#define LEX_ERR 1
#define SYNT_ERR 2
#define SEM_ERR 3
#define TYPE_COMP_ERR 4
#define OTHER_ERR 6
#define INTERNAL_ERR 99

#define EXPR_END_ID 99
#define TERM_DELIM 100
#define EXPR_ID 101
#define RULE_START_ID 102

#define SCOPE_LOCAL 103
#define SCOPE_STATIC 104
#define SCOPE_SHARED 105

#define CONTEXT_MAIN 106
#define CONTEXT_FUNCTION 107
#define CONTEXT_DECL_LIST 108

#define NO_INIT 109
#define NO_CONVERSION 110
#define NO_DECL 111
#define NO_ID 112
#define NO_COND 113

#define CODE_STRUCTURE_LINEAR 1
#define CODE_STRUCTURE_JUMP 2

/** @brief Line counter in scanner.c for error reporting. */
extern int line_counter;

/**  @brief Stores information about function parameters. */
typedef struct param
{             
   int type;                   ///< Type of the parameter.
   char *name;                 ///< IFJ17 name of the parameter - symtable key.
   char *code_name;            ///< IFJcode17 name of the parameter
} param_t;

/** @brief Information about variable used in symbol definition.  */
struct var_info
{
   int type;                   ///< Variable type.
   int depth;                  ///< Depth of the variable in scope blocks.
   int scope_modifier;         ///< Variable scope modifier.
   bool is_const;              ///< Does the variable contain a constant?
   bool used;
   union var_value             ///< Value of the variable, if constant
   {
      char *tmp_value;         ///< Name of the temporary variable where this 
                               ///< variable is stored (if not generated)
      int int_value;           ///< Integer value of the variable (if const)
      double double_value;     ///< Double value of the variable (if const)
      char *string_value;      ///< String value of the variable (if const)
   } value;
   char *code_name;            ///< Ifjcode17 name of the variable.
};

/* Ilist_t forward declaration for future usage.   */
typedef struct instruction_list ilist_t;

/** @brief Information about function used in symbol definition.  */
struct func_info
{
   int state;                  ///< Function existence state information.
   int returnType;             ///< Return type of the function.
   int param_count;            ///< Number of parameters of this function.
   param_t *params;            ///< Array of parameters.
   char *code_label;           ///< Label of this function in ifjcode17.
   ilist_t *start;             ///< Where this function starts in the ilist
};

typedef struct vdstack_item
{
   struct var_info data;       ///< Variable information.
   struct vdstack_item *prev;  ///< Predecessor of current stack element.
} vdstack_item;

/** @brief Variable depth stack implemented as pointer using simple typedef. */
typedef vdstack_item *vdstack;

/** @brief Symbol structure definition. */
typedef struct item_symbol
{
   char *id;                   ///< Symbol identifier.
   int type;                   ///< Identifier type.
   union symbol_info
   {
      vdstack var_stack;       ///< Information about a variable
      struct func_info func;   ///< Information about a function
   } s;
} symbol_t;

/** @brief Token data type.   */
typedef struct token
{
  int id;                      ///< ID of a lexeme.
  union attr                   ///< Token attribute.
  {
    char *str;                 ///< Token itself or temp var for expressions.
    int val_int;               ///< Integer value of a number.
    double val_real;           ///< Floating point value of a number.
    bool val_bool;             ///< Boolean value for constatn expressions
    ilist_t *ilist_ptr;        ///< Pointer to the previous instruction list.
  } a;
  int type;                    ///< Type of expression (if valid).
  bool is_const;               ///< Identifies constant token (expression).
} token_t;

/** @brief Structure represents instructions of 3AC code (IFJcode17). */
 typedef struct three_addr_code
{
  int operation_type;          ///< Internal representation of operation.
  token_t first_operand;       ///< First instruction operand.
  token_t second_operand;      ///< Second instruction operand.
  token_t third_operand;       ///< Third instruction operand.
} code_t;

/** @brief Doubly-linked instruction list element. */
typedef struct instruction_list_element {
   code_t data;                                 ///< Instruction data
   struct instruction_list_element *lneigh;     ///< Left neighbor
   struct instruction_list_element *rneigh;     ///< Right neighbor
} ilist_elem_t;

/** @brief Doubly-linked instruction list used for code generation.  */
struct instruction_list {
   ilist_elem_t *first;        ///< First element pointer
   ilist_elem_t *activ;        ///< Active element pointer
   ilist_elem_t *last;         ///< Last element pointer
   int size;                   ///< Number of elems in the list
};

/** @brief String implementation using dynamic char array length. */
typedef struct dynamic_str
{
    int len;                   ///< Holds current length of the data array.
    int allocated;             ///< Currently pre-allocated string size.
    char *data;                ///< Char array holding the actual string.
} string_t;

/** @brief Dynamic string structure declaration for scanner usage.  */
extern string_t str;

/** @brief Instruction list variable for generator usage.  */
extern ilist_t instructs;
extern ilist_t global_var_ilist;
extern ilist_t *code_list;
#endif // IFJ17_API_H_INCLUDED
