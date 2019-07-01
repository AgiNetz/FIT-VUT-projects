/**
 * @file main.c
 * @brief Main file for IFJ Project 2017 AT vut.fit.vutbr.cz
 * @date 10.10.2017, last rev. 18.11.2017
 * @author Patrik Goldschmidt - xgolds00
 *
 * Program for IFJ Project 2017 loads code in source language IFJ17 and
 * interpretes it into the IFJcode17. This code is then furtherly interpreted
 * using IFJ17 interpreter.
 */

/*    Includes section   */
#include "error.h"
#include "generator.h"
#include "ifj17_api.h"
#include "ilist.h"
#include "parser.h"
#include "safe_malloc.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>

/**   @brief Defines expected number of argumens on program start.   */
#define ARG_NUM 1

/**   @brief Defines info/error message on incorrect params run.  */
#define USAGE \
"./ifj17 - IFJ17 language interpreter. Program loads ifj17 source language \
           from stdin and produces output in IFJcode17 to stdout. \
Example run: \
./ifj17 < source.ifj > output.ifjc"

ilist_t instructs;            ///< Instruction list
ilist_t global_var_ilist;     ///< Global variables list
ilist_t *code_list;

int main(int argc, char **argv)
{
   (void)argv;
   int out_code = 0;

   /* Check correct program run. */
   if(argc != ARG_NUM)
   {
      fprintf(stderr, USAGE);
      return INTERNAL_ERR;
   }

   /* Memory and dynamic structures initializations.  */
   memman_init();
   out_code = str_init(&str);
   if(out_code)
      return INTERNAL_ERR;
   stab_init(&global_table);
   stab_init(&local_table);
   ilist_init(&instructs);
   ilist_init(&global_var_ilist);
   out_code = tstack_init(&semStack);
   if(out_code)
      return INTERNAL_ERR;
   out_code = tstack_init(&instr_stack);
   if(out_code)
      return INTERNAL_ERR;
   integrate_builtin();

   //FIXME error checky po inicializaciach + korektne uvolennie + internal err

   /* Set source file as sdtin.  */
   set_source_file(stdin);

   /* Set default code_list to instucts */
   code_list = &instructs;

   /* Start the parsing process. */
   token.id = -2;                   //Ensure no invalid free in GET_TOKEN
   GET_TOKEN(token);
   parse();

   if(status == 0)
   {
      order_ilist(&instructs);
      instruction_print(&instructs);
   }

   /* Deallocate data structures and end the program. */
   tstack_free(&semStack);
   tstack_free(&instr_stack);
   stab_clear(&global_table);
   stab_clear(&local_table);
   ilist_free(&instructs);
   ilist_free(&global_var_ilist);
   str_free(&str);
   memman_free_all();

   return status;
}
