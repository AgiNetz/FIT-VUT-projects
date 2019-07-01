/**
 * @file parser.h
 * @brief parser.h for IFJ Project 2017 AT vut.fit.vutbr.cz
 * @date 11. 11. 2017, last rev. 11.11.2017
 * @author Tomas Danis - xdanis05
 *
 * Module description: Parsing module resposible for the syntactic analysis of a given source
 *                     program written in language IFJ17.
 */

#ifndef PARSER_H_INCLUDED
#define PARSER_H_INCLUDED

/*		Includes section	*/
#include "scanner.h"
#include "symtable.h"
#include "tokenstack.h"
#include "vdstack.h"
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

/*	Macros	section		*/
#define GET_TOKEN(t) do { if(!getToken(&t)) RegisterError(LEX_ERR); /* fprintf(stderr,"%d\n", t.id); */ } while(0)
#define HANDLE_ERROR(err,success) do { if(status == 0) {RegisterError(err); /* fprintf(stderr,"We failed\n"); */} success = false; return false;} while(0)
#define SAFE_PUSH(stack, token) do { if(tstack_push(&stack, &token)) { HANDLE_ERROR(INTERNAL_ERR, success); \
                                internal_error_msg("Failed to allocate memory!\n"); } } while(0)

#define SYMB_VAR 1
#define SYMB_FUNC 2

#define FUNC_STATE_DECLARED 1
#define FUNC_STATE_DEFINED 2

#define VAR_DECL_ACTION_NEW_ENTRY 1
#define VAR_DECL_ACTION_PUSH 2

#define FUNC_DEF_ACTION_NEW_ENTRY 1
#define FUNC_DEF_ACTION_UPDATE 2


extern token_t token;
extern symtab_t global_table;             ///< Global symbol table
extern symtab_t local_table;              ///< Local symbol table for the currently processed function
extern tstack semStack;
extern tstack instr_stack;
extern int code_structure;

/*	Prototypes	*/
/**
 * @brief Brief description
 *
 * Description of this function's purpose and results.
 *
 * @param[in] exPar Description of this input parameter
 * @param[out] *exPar2 Description of this output parameter.
 * @return Description of return values of this function.
 */

 bool parse();
 bool integrate_builtin();
 char *token_to_string(int id);

/*	Functions definitions (inline)	*/

static inline bool getToken(token_t *token)
{
    int result = get_next_token(token);
    if(result == LEX_ERR)
    {
        fprintf(stderr, "Unrecognized token %s\n", token->a.str);
        return false;
    }
    return true;
}

static inline int round_even(double a)
{
    int rounded = a+0.5;
    int tmp = a;
    if(a-tmp == 0.5)
        return (tmp%2 == 0) ? tmp : tmp+1; 
    else
        return rounded;
}

static inline void cp_val_from_token(token_t *t, struct var_info *v)
{
    if(t->type == INTEGER_ID)
    {
        if(v->type == INTEGER_ID)
            v->value.int_value = t->a.val_int;
        else if(v->type == DOUBLE_ID)
            v->value.double_value = t->a.val_int;
    }
    else if(t->type == DOUBLE_ID)
    {
        if(v->type == DOUBLE_ID)
            v->value.double_value = t->a.val_real;
        else if(v->type == INTEGER_ID)
            v->value.int_value = round_even(t->a.val_real);
    }
    else if(t->type == STRING_ID)
    {
        v->value.string_value = t->a.str;
    }
}

static inline void cp_val_to_token(token_t *t, struct var_info *v)
{
    if(v->type == INTEGER_ID)
    {
        if(t->type == INTEGER_ID)
            t->a.val_int = v->value.int_value;
        else if(t->type == DOUBLE_ID)
            t->a.val_real = v->value.int_value;
    }
    else if(v->type == DOUBLE_ID)
    {
        if(t->type == DOUBLE_ID)
            t->a.val_real = v->value.double_value;
        else if(t->type == INTEGER_ID)
            t->a.val_int = round_even(v->value.double_value);
    }
    else if(v->type == STRING_ID)
    {
        t->a.str = v->value.string_value;
    }
}

#endif // PARSER_H_INCLUDED
