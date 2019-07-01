#ifndef EXPR_H_INCLUDED
#define EXPR_H_INCLUDED

#include "ifj17_api.h"
#include <stdbool.h>

#define SHIFT_ACTION 0
#define RULE_START_SHIFT_ACTION 1
#define REDUCE_ACTION 2
#define END_ACTION 3

bool expr();
bool func_call();
bool built_in_func_call();
bool arg_list();
bool more_args_list();
bool arg();
bool check_variable_declaration(char *name, symbol_t **var_entry);
bool find_conversion(int type1, int type2,  int *result_type);
void convert_literal(token_t *lit, token_t *conv);
#endif // EXPR_H_INCLUDED
