/**
 * @file generator.h
 * @brief SOME file for IFJ Project 2017 AT vut.fit.vutbr.cz
 * @date 17.11.2017, last rev. xx.xx.xxxx
 * @author Peter Hamran - xhamra00
 *
 * Module description: This modul generates IFJcode17
 */

#ifndef GENERATOR_H
#define GENERATOR_H 1

/*		Includes section	*/
#include "ifj17_api.h"
#include "tokenstack.h"
#include "ilist.h"
#include <stdlib.h>
#include <string.h>

/* Instruction defines */

#define ADD_INS_ID 1
#define ADDS_INS_ID 2
#define AND_INS_ID 3
#define ANDS_INS_ID 4
#define BREAK_INS_ID 5
#define CALL_INS_ID 6
#define CLEARS_INS_ID 7
#define CONCAT_INS_ID 8
#define CREATEFRAME_INS_ID 9
#define DEFVAR_INS_ID 10
#define DIV_INS_ID 11
#define DIVS_INS_ID 12
#define DPRINT_INS_ID 13
#define EQ_INS_ID 14
#define EQS_INS_ID 15
#define FLOAT2INT_INS_ID 16
#define FLOAT2INTS_INS_ID 17
#define FLOAT2R2EINT_INS_ID 18
#define FLOAT2R2EINTS_INS_ID 19
#define FLOAT2R2OINT_INS_ID 20
#define FLOAT2R2OINTS_INS_ID 21
#define GETCHAR_INS_ID 22
#define GT_INS_ID 23
#define GTS_INS_ID 24
#define INT2FLOAT_INS_ID 25
#define INT2FLOATS_INS_ID 26
#define INT2CHAR_INS_ID 27
#define INT2CHARS_INS_ID 28
#define JUMP_INS_ID 29
#define JUMPIFEQ_INS_ID 30
#define JUMPIFEQS_INS_ID 31
#define JUMPIFNEQ_INS_ID 32
#define JUMPIFNEQS_INS_ID 33
#define LABEL_INS_ID 34
#define LT_INS_ID 35
#define LTS_INS_ID 36
#define MOVE_INS_ID 37
#define MUL_INS_ID 38
#define MULS_INS_ID 39
#define NOT_INS_ID 40
#define NOTS_INS_ID 41
#define OR_INS_ID 42
#define ORS_INS_ID 43
#define POPFRAME_INS_ID 44
#define POPS_INS_ID 45
#define PUSHFRAME_INS_ID 46
#define PUSHS_INS_ID 47
#define READ_INS_ID 48
#define RETURN_INS_ID 49
#define SETCHAR_INS_ID 50
#define STRI2INT_INS_ID 51
#define STRI2INTS_INS_ID 52
#define STRLEN_INS_ID 53
#define SUB_INS_ID 54
#define SUBS_INS_ID 55
#define TYPE_INS_ID 56
#define WRITE_INS_ID 57
#define INT_DIV_INS_ID 58


/* Internal comms defines */

#define TMP_ID 1000
#define ASS_ID 1001
#define IF_LAB_ID 1002
#define ELSE_IF_LAB_ID 1003
#define ELSE_LAB_ID 1004
#define END_IF_LAB_ID 1005
#define END_IF_ID 1006
#define FOR_BEGIN_ID 1007
#define FOR_NEXT_ID 1008
#define FOR_CODE_ID 1009
#define FOR_END_ID 1010
#define MAKE_FOR_ID 1011
#define BRANCH_LIST_ID 1012
#define TOKEN_TRUE_ID 1013
#define LOOP_BEGIN_ID 1014
#define MAKE_WHILE_ID 1015
#define LOOP_END_ID 1016
#define MAKE_LOOP_ID 1017
#define INIT_ID 1018
#define LABEL_MAIN 1019
#define FUNC_CALL_INIT_ID 1020
#define FUNC_CALL_FIN_ID 1021
#define PARAM_NAME_GEN_ID 1022
#define PARAM_ID 1023
#define INIT_LESS_ID 1024
#define INL_LEN_ID 1025
#define INL_CHR_ID 1026
#define INL_ASC_ID 1027
#define INL_SUBSTR_ID 1028
#define NO_PRINT_ID 1029
#define FUNC_NAME_ID 1030
#define FUNC_END_ID 1031

/*GODLIKE NIL!!!*/
#define NIL NULL

	
/*	Prototypes	*/
/**
 * @brief Generates instructions and inserts them to instruction list 
 * (instructs). 
 * 
 *
 * @param *code_list Pointer to specific instruction list.
 * @param type Integer containing type of instruction to be generated.
 * @param num Integer containing number of variable parameters
 * 
 * @return void.
 */
void generate_instruction(ilist_t *code_list, int type, int num,  ...);

/**
 * @brief Called from parser and handles the generation of 3AC code
 *
 *
 * @param *instruction_stack Pointer to the stack of tokens.
 * @param id Integer containing id of conducted operation.
 * @return Returns name for variable or label names for functions.
 */
char *generate(tstack *instruction_stack, int id);

/**
 * @brief Generates coresponding label names for spacial cases and handles
 *	uniqueness of labels for those cases.
 *
 *
 * @param *instruction_stack Pointer to the stack of tokens.
 * @param id Integer containing id of label type.
 * @return void.
 */
void generate_label(tstack *instruction_stack, int id);

/**
 * @brief Prints contents of the instruction list.
 *
 *
 * @param *instructs Pointer to the list of instructions to be printed.
 * @return void.
 */
void instruction_print(ilist_t *instructs);

/**
 * @brief Oredrs instructions in list 
 *
 *
 * @param *ins_list Pointer to list of instructions to be ordered
 * @return void.
 */
void order_ilist(ilist_t *ins_list);

/**
 * @brief Handles generation of internal conversions of variables and values passed to.
 *
 *
 * @param *code_list Pointer to global list of generated instructions
 * @param conv_id Identifier of conversion type to be generated
 * @param *op_token Token containing information for conversion
 * @param *counter Pointer to counter for temporary variables generation
 * @return void.
 */
void token_conversion(ilist_t *code_list, int conv_id, token_t *op_token, int *counter);


//void string_convert(char **str);
#endif // GENERATOR_H
