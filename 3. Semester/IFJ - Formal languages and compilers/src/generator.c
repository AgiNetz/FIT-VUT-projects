/**
 * @file generator.c
 * @brief SOME file for IFJ Project 2017 AT vut.fit.vutbr.cz
 * @date 17.11.2017, last rev. xx.xx.xxxx
 * @author Peter Hamran - xhamra00
 *
 * Module description: Generates IFJcode17
 */

/*    Includes section     */
#include "generator.h"
#include "symtable.h"
#include "safe_malloc.h"
#include "scanner.h"
#include "parser.h"
//#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>


/*    Functions definitions section    */
char *generate(tstack *instruction_stack, int id)
{
	/* Counter variable for generation of unique temporary variables */
	static int unique_counter = 1;
	/* Auxiliary variables for computing length of unique_counter */
	int counter_length = 0;
	int temp_counter = unique_counter;
	

	/* Reduction of addition of 2 expressions to one [<Exp> -> <Exp> + <Exp>] */
	if(id == PLUS_ID)
	{
		/* Creation of temporary tokens*/
		token_t tmp1, tmp2, type1_token, type2_token;

		/* Managing stack rules, values and converions */
		type2_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		type1_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp2 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Type conversions for operands */
		token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);
		token_conversion(code_list, type1_token.id, &tmp1, &unique_counter);

		/* Generating the unique name for temporary result variable */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL; 

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		/* Setting up result token for stack and instruction generation */
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generates variable for result of evaluation */
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);

		/* Difference between ADDition of numbers and CONCATenation of strings 
			based on variable type*/
		if(tmp1.type == STRING_ID)
		{
			generate_instruction(code_list, CONCAT_INS_ID, 3, tmp1, tmp2,
										result_token);
		}
		else
		{
			generate_instruction(code_list, ADD_INS_ID, 3, tmp1, tmp2, 
										result_token);
		}

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}
	
	/* Reduction of substitution of 2 expressions to one 
		[<Exp> -> <Exp> - <Exp>]*/
	else if(id == MINUS_ID)
	{
		/* Creation of temporary operand tokens */
		token_t tmp1, tmp2, type1_token, type2_token;
		
		/* Managing stack rules, values and converions */
		type2_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		type1_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp2 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Type conversions for operands */
		token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);
		token_conversion(code_list, type1_token.id, &tmp1, &unique_counter);

		/* Generating the unique name for temporary result variable */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		/* Setting up result token for stack and instruction generation */
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generates variable for result of evaluation and assigns result value*/
		generate_instruction(code_list, DEFVAR_INS_ID,1,result_token);
		generate_instruction(code_list, SUB_INS_ID, 3, tmp1, tmp2, result_token);

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}
	
	/*Reduction of multiplication of 2 expressions to one 
	[<Exp> -> <Exp> * <Exp>]*/
	else if(id == ASTERISK_ID)
	{
		/* Creation of temporary operand tokens */
		token_t tmp1, tmp2, type1_token, type2_token;
		
		/* Managing stack rules, values and converions */
		type2_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		type1_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp2 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/*Type conversions for operands*/
		token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);
		token_conversion(code_list, type1_token.id, &tmp1, &unique_counter);

		/* Generating the unique name for temporary result variable */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		/* Setting up result token for stack and instruction generation */
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generates variable for result of evaluation and assigns result value*/
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, MUL_INS_ID, 3, tmp1, tmp2, result_token);

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}
	
	/* Reduction of division of 2 expressions(float) to one 
		[<Exp> -> <Exp> / <Exp>]*/
	else if(id == SLASH_ID)
	{
		/* Creation of temporary operand tokens */
		token_t tmp1, tmp2, type1_token, type2_token;
		
		/* Managing stack rules, values and converions */
		type2_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		type1_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp2 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Type conversions for operands */
		token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);
		token_conversion(code_list, type1_token.id, &tmp1, &unique_counter);

		/* Generating the unique name for temporary result variable */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		/* Setting up result token for stack and instruction generation */
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generates variable for result of evaluation and assigns result value*/
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, DIV_INS_ID, 3,tmp1, tmp2, result_token);

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}
	
	/*Reduction of addition of 2 expressions to one [<Exp> -> <Exp> \ <Exp>]*/
	else if(id == BACKSLASH_ID)
	{
		/* Creation of temporary operand tokens */
		token_t tmp1, tmp2, type1_token, type2_token;

		/* Managing stack rules, values and converions */
		type2_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		type1_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp2 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Type conversions for operands */
		token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);
		token_conversion(code_list, type1_token.id, &tmp1, &unique_counter);

		if(!tmp2.is_const)
      {
         token_conversion(code_list, DOUBLE_ID, &tmp2, &unique_counter);
			//generate_instruction(code_list, INT2FLOAT_INS_ID, 2, tmp2, tmp2);
      }

		if(!tmp1.is_const)
      {
         token_conversion(code_list, DOUBLE_ID, &tmp1, &unique_counter);
			//generate_instruction(code_list, INT2FLOAT_INS_ID, 2, tmp1, tmp1);
      }

		/* Generating the unique name for temporary result variable */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, "%s%d", 
					"LF@%tmp", unique_counter);

		/* Setting up result token for stack and instruction generation */
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generates variable for result of evaluation and assigns result value*/
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, INT_DIV_INS_ID, 3, tmp1, tmp2, 
									result_token);

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}
	
	/*Reduction of comparison of 2 expressions to one [<Exp> -> <Exp> < <Exp>]*/
	else if(id == LESS_ID)
	{
		/* Creation of temporary operand tokens */
		token_t tmp1, tmp2, type1_token, type2_token;
		
		/* Managing stack rules, values and converions */
		type2_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		type1_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp2 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Type conversions for operands */
		token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);
		token_conversion(code_list, type1_token.id, &tmp1, &unique_counter);

		/* Generating the unique name for temporary result variable */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, "%s%d", 
					"LF@%tmp", unique_counter);

		/* Setting up result token for stack and instruction generation */
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generates variable for result of evaluation and assigns result value*/
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, LT_INS_ID, 3, tmp1, tmp2, result_token);

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}
	
	/*Reduction of comparison of 2 expressions to one [<Exp> -> <Exp> = <Exp>]*/
	else if(id == EQUALS_ID)
	{
		/* Creation of temporary operand tokens */
		token_t tmp1, tmp2, type1_token, type2_token;

		/* Managing stack rules, values and converions */
		type2_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		type1_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp2 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/*Type conversions for operands*/
		token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);
		token_conversion(code_list, type1_token.id, &tmp1, &unique_counter);

		/* Generating the unique name for temporary result variable */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		/* Setting up result token for stack and instruction generation */
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generates variable for result of evaluation and assigns result value*/
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, EQ_INS_ID, 3, tmp1, tmp2, result_token);

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}

	/*Reduction of comparison of 2 expressions to one [<Exp> -> <Exp> > <Exp>]*/
	else if(id == GREATER_ID)
	{
		/* Creation of temporary operand tokens */
		token_t tmp1, tmp2, type1_token, type2_token;
		
		/* Managing stack rules, values and converions */
		type2_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		type1_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp2 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/*Type conversions for operands*/
		token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);
		token_conversion(code_list, type1_token.id, &tmp1, &unique_counter);

		/* Generating the unique name for temporary result variable */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		/* Setting up result token for stack and instruction generation */
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generates variable for result of evaluation and assigns result value*/
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, GT_INS_ID, 3, tmp1, tmp2, result_token);

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}

	/*Reduction of comparison of 2 expressions to one [<Exp> -> <Exp> OR <Exp>]*/
	else if(id == OR_ID)
	{
		/* Creation of temporary operand tokens */
		token_t tmp1, tmp2;

		/* Managing stack rules, values and converions */
		tmp2 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Generating the unique name for temporary result variable */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		/* Setting up result token for stack and instruction generation */
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generates variable for result of evaluation and assigns result value*/
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, OR_INS_ID, 3, tmp1, tmp2, result_token);

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}

	/* Reduction of comparison of 2 expressions to one [<Exp> -> <Exp> <= <Exp>]
	 * Result of expression is given by <Exp> < <Exp> OR <Exp> = <Exp>
	 */
	else if(id == LESS_EQUAL_ID)
{
   token_t tmp1, tmp2, type1_token, type2_token;

   type2_token = tstack_top(instruction_stack);
   tstack_pop(instruction_stack);
   type1_token = tstack_top(instruction_stack);
   tstack_pop(instruction_stack);
   tmp2 = tstack_top(instruction_stack);
   tstack_pop(instruction_stack);
   tmp1 = tstack_top(instruction_stack);
   tstack_pop(instruction_stack);

   token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);
   token_conversion(code_list, type1_token.id, &tmp1, &unique_counter);

   while(temp_counter != 0)
   {
      temp_counter /= 10;
      counter_length++;
   }

   char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
   if(result_name == NULL)
      return NULL;

   snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
            "%s%d", "LF@%tmp", unique_counter);

   token_t result_token;
   result_token.id = TMP_ID;
   result_token.a.str = result_name;
   result_token.is_const = false;

   generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
   generate_instruction(code_list, GT_INS_ID, 3, tmp1, tmp2, result_token);
   generate_instruction(code_list, NOT_INS_ID, 2, result_token, result_token);

   unique_counter++;

   return result_name;
}


	/*Reduction of comparison of 2 expressions to one [<Exp> -> <Exp> >= <Exp>]
	**Result of expression is given by <Exp> > <Exp> OR <Exp> = <Exp>
	*/
	else if(id == GREATER_EQUAL_ID)
{
   token_t tmp1, tmp2, type1_token, type2_token;

   type2_token = tstack_top(instruction_stack);
   tstack_pop(instruction_stack);
   type1_token = tstack_top(instruction_stack);
   tstack_pop(instruction_stack);
   tmp2 = tstack_top(instruction_stack);
   tstack_pop(instruction_stack);
   tmp1 = tstack_top(instruction_stack);
   tstack_pop(instruction_stack);

   token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);
   token_conversion(code_list, type1_token.id, &tmp1, &unique_counter);

   while(temp_counter != 0)
   {
      temp_counter /= 10;
      counter_length++;
   }

   char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
   if(result_name == NULL)
      return NULL;

   snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
            "%s%d", "LF@%tmp", unique_counter);

   token_t result_token;
   result_token.id = TMP_ID;
   result_token.a.str = result_name;
   result_token.is_const = false;

   generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
   generate_instruction(code_list, LT_INS_ID, 3, tmp1, tmp2, result_token);
   generate_instruction(code_list, NOT_INS_ID, 2, result_token, result_token);

   unique_counter++;

   return result_name;
}

	/*Reduction of comparison of 2 expressions to one [<Exp> -> !<Exp>]*/
	else if(id == NOT_ID)
	{
		/* Creation of temporary operand token */
		token_t tmp1;

		/* Managing stack rules, values and converions */
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Generating the unique name for temporary result variable */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		/* Setting up result token for stack and instruction generation */
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generates variable for result of evaluation and assigns result value*/
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, NOT_INS_ID, 3, tmp1, result_token);

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}

	/*Reduction of comparison of 2 expressions to one [<Exp> -> <Exp> <> <Exp>]*/
	else if(id == NOT_EQUAL_ID)
	{
		/* Creation of temporary operand token */
		token_t tmp1;
		
		/* Setting up result token for stack and instruction generation */
		tmp1.id = TMP_ID;
		tmp1.a.str = generate(instruction_stack, EQUALS_ID);
		tstack_push(instruction_stack, &tmp1);
		
		return generate(instruction_stack, NOT_ID);
	}

	/* Assignment of value to variable */
	else if(id == INIT_ID)
	{
		/* Creation of temporary operand tokens */
		token_t tmp1, tmp2, type2_token;

		char frame_str[5];
		
		/* Managing stack rules, values and converions */
		type2_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp2 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Type conversions for value operand */
		token_conversion(code_list, type2_token.id, &tmp2, &unique_counter);

		/* Computes length of space needed for including counter to the name of
		 *	variable
		 */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		/* Scope setup*/
		if(tmp1.id == SCOPE_LOCAL)
			strcpy(frame_str, "LF@_");
		else
		{
			strcpy(frame_str, "GF@_");
			/* Redirects code_list pointer to global variable instruction list
			 * based on value in token.id
			 */
			code_list = &global_var_ilist;
		}


		/* Generating result name for variable, '+2' in size smalloced stands 
		 *	for '\0' and '&' symbols
		 */
		char *result_name = smalloc(strlen(frame_str) + strlen(tmp1.a.str) + 
											counter_length+2);
		if(result_name == NULL)
			return NULL;
		snprintf(result_name, strlen(frame_str) + strlen(tmp1.a.str) + 
					counter_length+2, "%s%s&%d", frame_str, tmp1.a.str, 
					unique_counter);

		/* Setting up result token with ID and name string*/
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generating instruction code*/
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, MOVE_INS_ID, 2, tmp2, result_token);

		/* Reverting the redirection of code list in case of declaring 
		 * global variables
		 */
		if(code_list == &global_var_ilist)
			code_list = &instructs;

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}
	else if(id == TMP_ID)
	{
		/* Creation of temporary operand token */
		token_t tmp1;

		/* Managing stack rules, values and converions */
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Computes length of space needed for including counter to the name of
		 *	variable
		 */
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		/* Setting up result token with ID and name string*/
		token_t result_token;
		result_token.id = TMP_ID;
		result_token.a.str = result_name;
		result_token.is_const = false;

		/* Generating instruction code*/
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, MOVE_INS_ID, 2, tmp1, result_token);

		/* Incrementation of unique_counter keeps variable generation unique */
		unique_counter++;

		/* Returns generated result name to parser for later use */
		return result_name;
	}

	/* <selection-statement> */
	else if(id == IF_ID)
	{
		/* Internal Label generation to stack, generate end_if.label */
		generate_label(instruction_stack, IF_ID);
		return NULL;
	}
	else if(id == THEN_ID)
	{
		/* Creating new pointer to instruction list for rebranching of 
		 * <selection-statement> body generation and later merge to main
		 * instruction list
		 */
		ilist_t *new_inst_list = smalloc(sizeof(ilist_t));
		ilist_init(new_inst_list);

		/* Saves pointer to old instruction list to stack for later merge with
		 * new instruction list of <selection-statement> body
		 */
		token_t list_token;
		list_token.id = THEN_ID;
		list_token.a.ilist_ptr = code_list;
		list_token.is_const = false;

		tstack_push(instruction_stack, &list_token);

		/* Change pointer of used code_list to diverge generator */
		code_list = new_inst_list;

		return NULL;
	}

	else if(id == ELSEIF_ID)
	{
		/* Temporary tokens to pop used values from stack to */
		token_t tmp_lab, tmp_link, tmp1, tmp_end_lab, token_true;
		token_t tmp_counter_token;

		/* Temporary pointer to later set pointing to rebranched instruction 
		 * list 
		 */
		ilist_t *tmp_ins_list;

		/* Exctraction of values from stack */
		tmp_link = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Generate unique else if label to stack */
		generate_label(instruction_stack, ELSEIF_ID);

		tmp_lab = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Setup and generation of previous statement and then merging body of
		 * that statement right after
		 */
		token_true.id = TOKEN_TRUE_ID;
		token_true.type = BOOLEAN_ID;
		token_true.a.val_bool = true;
		token_true.is_const = true;

		tmp_ins_list = code_list;
		code_list = tmp_link.a.ilist_ptr;
		generate_instruction(code_list, JUMPIFNEQ_INS_ID, 3, tmp_lab, tmp1, 
									token_true);
		ilist_merge(code_list, tmp_ins_list);

		tmp_counter_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp_end_lab = tstack_top(instruction_stack);
		tstack_push(instruction_stack, &tmp_counter_token);

		/* Jump to end_label generated */
		generate_instruction(code_list, JUMP_INS_ID, 1 ,tmp_end_lab);
		generate_instruction(code_list, LABEL_INS_ID, 1, tmp_lab);

		return NULL;
	}

	else if(id == ELSE_ID)
	{
		/* Temporary tokens to pop used values from stack to */
		token_t tmp_lab, tmp_link, tmp1, tmp_end_lab, token_true;
		token_t tmp_counter_token;

		/* Temporary pointer to later set pointing to rebranched instruction 
		 * list 
		 */
		ilist_t *tmp_ins_list;

		/* Exctraction of values from stack */
		tmp_link = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Generate unique else label to stack */
		generate_label(instruction_stack, ELSE_ID);

		tmp_lab = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		/* Setup and generation of previous statement and then merging body of
		 * that statement right after
		 */
		token_true.id = TOKEN_TRUE_ID;
		token_true.type = BOOLEAN_ID;
		token_true.a.val_bool = true;
		token_true.is_const = true;

		tmp_ins_list = code_list;
		code_list = tmp_link.a.ilist_ptr;
		generate_instruction(code_list, JUMPIFNEQ_INS_ID, 3, tmp_lab, tmp1, 
									token_true);
		ilist_merge(code_list, tmp_ins_list);

		tmp_counter_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		tmp_end_lab = tstack_top(instruction_stack);
		tstack_push(instruction_stack, &tmp_counter_token);

		/* Jump to end_label generated */
		generate_instruction(code_list, JUMP_INS_ID, 1, tmp_end_lab);
		generate_instruction(code_list, LABEL_INS_ID, 1, tmp_lab);

		return NULL;
	}

	else if(id == END_IF_ID)
	{
		/* First temporary token to be popped and checked for id to ensure 
		 * encapsulation of if-elseif-else statements
		 */
		token_t tmp1;

		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		if(tmp1.id == THEN_ID)
		{
			/* Temporary tokens to pop used values from stack to */
			token_t tmp3, tmp_end_lab, token_true;

			/* Temporary pointer to later set pointing to rebranched instruction 
		 	 * list 
		 	 */
			ilist_t *tmp_ins_list;

			tmp3 = tstack_top(instruction_stack);
			tstack_pop(instruction_stack);
			tstack_pop(instruction_stack);
			tmp_end_lab= tstack_top(instruction_stack);
			tstack_pop(instruction_stack);

			token_true.id = TOKEN_TRUE_ID;
			token_true.type = BOOLEAN_ID;
			token_true.a.val_bool = true;
			token_true.is_const = true;

			tmp_ins_list = code_list;
			code_list = tmp1.a.ilist_ptr;
			generate_instruction(code_list, JUMPIFNEQ_INS_ID, 3, tmp_end_lab, 
										tmp3, token_true);
			ilist_merge(code_list, tmp_ins_list);

			generate_instruction(code_list, LABEL_INS_ID, 1, tmp_end_lab);
		}
		else
		{
			tmp1 = tstack_top(instruction_stack);
			tstack_pop(instruction_stack);
			generate_instruction(code_list, LABEL_INS_ID, 1, tmp1);
		}
		return NULL;
	}

	/*<FOR NEXT iteration statement>*/
	else if(id == FOR_ID)
	{
		generate_label(instruction_stack, FOR_ID);
	}
	else if(id == MAKE_FOR_ID)
	{
		token_t step_token;
		token_t expj_token;
		token_t expi_token;
		token_t label_token;
		//token_t result_token;
		token_t true_token;
		token_t conv1_token, conv2_token;
		//token_t no_conv_token;
		token_t temp_result1_token;
		//token_t temp_result2_token;

		true_token.id = TOKEN_TRUE_ID;
		true_token.type = BOOLEAN_ID;
		true_token.a.val_bool = true;
		true_token.is_const = true;
		//no_conv_token.id = NO_CONVERSION;

		conv2_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		conv1_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		step_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		expj_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		expi_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		label_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		token_conversion(code_list, conv2_token.id, &step_token, &unique_counter);
		token_conversion(code_list, conv1_token.id, &expj_token, &unique_counter);

		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name1 = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name1 == NULL)
		{
			return NULL;
		}

		snprintf(result_name1, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		unique_counter++;
		//temp_counter = unique_counter;
		temp_result1_token.id = TMP_ID;
		temp_result1_token.a.str = result_name1;
		temp_result1_token.is_const = false;

		/*while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name2 = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(result_name == NULL)
		{
			return NULL;
		}

		snprintf(result_name2, counter_length + strlen("LF@%tmp") + 1, 
					"%s%d", "LF@%tmp", unique_counter);

		unique_counter++;
		temp_result2_token.id = TMP_ID;
		temp_result2_token.a.str = result_name2;*/

		
		//generate_instruction(code_list, DEFVAR_INS_ID, 1, temp_result2_token);
		/*Generate first label of FOR NEXT cycle "LABEL FOR_BEGIN_xx"*/
		generate_instruction(code_list, LABEL_INS_ID, 1, label_token);
		/*Store result of expression to result_token for later use in instructions*/
		/*tstack_push(instruction_stack, &no_conv_token);
		tstack_push(instruction_stack, &no_conv_token);
		tstack_push(instruction_stack, &expi_token);
		tstack_push(instruction_stack, &expj_token);
		result_token.id = EXPR_ID;*/

		/********************************************************************/

		generate_instruction(code_list, DEFVAR_INS_ID, 1, temp_result1_token);
		generate_instruction(code_list, GT_INS_ID, 3, expj_token, expi_token, temp_result1_token);
		generate_instruction(code_list, NOT_INS_ID, 2, temp_result1_token, temp_result1_token);


		/********************************************************************/
		//result_token.a.str = generate(instruction_stack, LESS_EQUAL_ID);
		/*Top next cycle label*/
		label_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		/*Conditional jump to "FOR_END" in case the condition is not met*/
		generate_instruction(code_list, JUMPIFNEQ_INS_ID, 3, label_token, temp_result1_token, true_token);
		/*Top next cycle label*/
		label_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		/*Unconditional jump to "FOR_CODE" label*/
		generate_instruction(code_list, JUMP_INS_ID, 1, label_token);
		/*Top next cycle label*/
		label_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		/*Generate label "FOR_NEXT" */
		generate_instruction(code_list, LABEL_INS_ID, 1, label_token);
		generate_instruction(code_list, ADD_INS_ID, 3, expi_token, step_token, expi_token);
		/*Top next cycle label*/
		label_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		/*Unconditional jump to "FOR_BEGIN" label*/
		generate_instruction(code_list, JUMP_INS_ID, 1, label_token);
		/*Top next cycle label*/
		label_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		/*Generate label "FOR_CODE" */
		generate_instruction(code_list, LABEL_INS_ID, 1, label_token);
	}
	else if(id == NEXT_ID)
	{
		token_t label_token;

		/*Top next cycle label*/
		label_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		/*Unconditional jump to "FOR_NEXT" label*/
		generate_instruction(code_list, JUMP_INS_ID, 1, label_token);
		/*Top next cycle label*/
		label_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		/*Generate label "FOR_END" */
		generate_instruction(code_list, LABEL_INS_ID, 1, label_token);
	}
	else if(id == DO_ID)
	{
		token_t label_token;

		generate_label(instruction_stack, DO_ID);
		label_token = tstack_top(instruction_stack);
		generate_instruction(code_list, LABEL_INS_ID, 1, label_token);
	}
	else if(id == MAKE_LOOP_ID)
	{
		token_t exp_token;
		token_t loop_type_token;
		token_t true_token;
		token_t label_token;

		true_token.id = TOKEN_TRUE_ID;
		true_token.type = BOOLEAN_ID;
		true_token.a.val_bool = true;
		true_token.is_const = true;

		exp_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack); 
		loop_type_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		generate_label(instruction_stack, LOOP_ID);
		label_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		if(loop_type_token.id == WHILE_ID)
			generate_instruction(code_list, JUMPIFNEQ_INS_ID, 3, label_token, exp_token, true_token);
		else
			generate_instruction(code_list, JUMPIFEQ_INS_ID, 3, label_token, exp_token, true_token);
		return NULL;
	}
	else if(id == LOOP_ID)
	{
		token_t label_begin_token;
		token_t label_end_token;

		generate_label(instruction_stack, LOOP_ID);

		label_end_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		label_begin_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		generate_instruction(code_list, JUMP_INS_ID, 1, label_begin_token);
		generate_instruction(code_list, LABEL_INS_ID, 1, label_end_token);
	}

	else if(id == PRINT_ID)
	{
		token_t print_count;
		token_t tmp1;

		print_count = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		for(int i = 0; i < print_count.a.val_int; i++)
		{
			tmp1 = tstack_top(instruction_stack);
			generate_instruction(code_list, WRITE_INS_ID, 1, tmp1);
			tstack_pop(instruction_stack);
		}
	}

	else if(id == ASS_ID)
	{
		token_t name_token;
		token_t expr_token;
		token_t type_token;

		type_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		expr_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		name_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		token_conversion(code_list, type_token.id, &expr_token, &unique_counter);

		generate_instruction(code_list, MOVE_INS_ID, 2, expr_token, name_token);	
	}

	else if(id == SCOPE_ID)
	{
		generate_instruction(code_list, LABEL_MAIN, 0);
		generate_instruction(code_list, CREATEFRAME_INS_ID, 0);
		generate_instruction(code_list, PUSHFRAME_INS_ID, 0);

 		ilist_merge(&instructs, &global_var_ilist);
		
	}

   else if(id == FUNC_NAME_ID)
   {
      token_t name_token;

      generate_label(instruction_stack, FUNCTION_ID);

      name_token = tstack_top(instruction_stack);
      tstack_pop(instruction_stack);

      return name_token.a.str;
   }

	else if(id == FUNCTION_ID)
	{
		token_t name_token;
		
		name_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		generate_instruction(code_list, LABEL_INS_ID, 1, name_token);
		generate_instruction(code_list, PUSHFRAME_INS_ID, 0);
		
		return name_token.a.str;
	}

	else if(id == FUNC_CALL_INIT_ID)
	{
		generate_instruction(code_list, CREATEFRAME_INS_ID, 0);
	}

	else if(id == PARAM_ID)
	{
		token_t name_token;
		token_t new_name_token;
		token_t val_token;
		token_t type_token;
	
		type_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		val_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		name_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		token_conversion(code_list, type_token.id, &val_token, &unique_counter);

		new_name_token.id = TMP_ID;
		char *new_name = smalloc(strlen(name_token.a.str)+1);
		if(new_name == NULL)
			return NULL;
		strncpy(new_name, name_token.a.str, strlen(name_token.a.str)+1);
		new_name[0] = 'T';
		new_name_token.a.str = new_name;

		generate_instruction(code_list, DEFVAR_INS_ID, 1, new_name_token);
		generate_instruction(code_list, MOVE_INS_ID, 2, val_token, 
									new_name_token);

		return NULL;
	}

	else if(id == FUNC_CALL_FIN_ID)
	{
		token_t func_name_token;
		token_t tmp_token;

		func_name_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		generate_instruction(code_list, CALL_INS_ID, 1, func_name_token);

		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *tmp_name = smalloc(counter_length+8);
		if(tmp_name == NULL)
			return NULL;

		snprintf(tmp_name, counter_length+8, "%s%d", "LF@%tmp", unique_counter);

		tmp_token.id = TMP_ID;
		tmp_token.a.str = tmp_name;
		tmp_token.is_const = false;

		generate_instruction(code_list, DEFVAR_INS_ID, 1, tmp_token);
		generate_instruction(code_list, POPS_INS_ID, 1, tmp_token);

		unique_counter++;

		return tmp_name;
	}

	else if(id == PARAM_NAME_GEN_ID)
	{
		token_t name_token;

		name_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *result_name = smalloc(strlen(name_token.a.str)+counter_length+6);
		if(result_name == NULL)
			return NULL;

		snprintf(result_name, strlen(name_token.a.str)+counter_length+6, 
					"%s%s%s%d", "LF@_", name_token.a.str, "&",unique_counter);

		return result_name;
	}

	else if(id == RETURN_ID)
	{
		token_t result_val_token;
		token_t type_token;

		type_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		result_val_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		generate_instruction(code_list, PUSHS_INS_ID, 1, result_val_token);

		if(type_token.id == INTEGER_ID)
			generate_instruction(code_list, FLOAT2R2EINTS_INS_ID, 0);
		else if(type_token.id == DOUBLE_ID)
			generate_instruction(code_list, INT2FLOATS_INS_ID, 0);
		generate_instruction(code_list, POPFRAME_INS_ID, 0);
		generate_instruction(code_list, RETURN_INS_ID, 0);

		return NULL;
	}

   else if(id == FUNC_END_ID)
   {
      token_t stack_val_token;
      token_t name_token = tstack_top(instruction_stack);
      tstack_pop(instruction_stack);

      if(name_token.id == INTEGER_ID)
      {
         stack_val_token.type = INTEGER_ID;
         stack_val_token.is_const = true;
         stack_val_token.a.val_int = 0;
         generate_instruction(code_list, PUSHS_INS_ID, 1, stack_val_token);
      }
      else if(name_token.id == DOUBLE_ID)
      {
         stack_val_token.type = DOUBLE_ID;
         stack_val_token.is_const = true;
         stack_val_token.a.val_real = 0.0;
         generate_instruction(code_list, PUSHS_INS_ID, 1, stack_val_token);
      }
      else if(name_token.id == STRING_ID)
      {
         stack_val_token.type = STRING_ID;
         stack_val_token.is_const = true;
         stack_val_token.a.str = "";
         generate_instruction(code_list, PUSHS_INS_ID, 1, stack_val_token);
      }

      generate_instruction(code_list, POPFRAME_INS_ID, 0);
      generate_instruction(code_list, RETURN_INS_ID, 0);
   }

	else if(id == INPUT_ID)
	{
		token_t var_token, string_token;

		var_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

      string_token.id = STRING_ID;
      string_token.type = STRING_ID;
      string_token.is_const = true;
      string_token.a.str = "?\\032";

      generate_instruction(code_list, WRITE_INS_ID, 1, string_token);
		generate_instruction(code_list, READ_INS_ID, 1, var_token);

		return NULL;
	}
	else if(id == INIT_LESS_ID)
	{
		/*creation of temporary tokens*/
		token_t tmp1;

		char frame_str[5];

		/*managing stack rules and getting values necessary for operation*/
		tmp1 = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);


		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		if(tmp1.id == SCOPE_LOCAL)
			strcpy(frame_str, "LF@_");
		else
		{
			strcpy(frame_str, "GF@_");
			code_list = &global_var_ilist;
		}

		char *result_name = smalloc(strlen(frame_str)+strlen(tmp1.a.str)+
				counter_length+3);
			if(result_name == NULL)
			{
				return NULL;
			}
			snprintf(result_name, strlen(frame_str)+strlen(tmp1.a.str)+
						counter_length+2, "%s%s&%d", frame_str, tmp1.a.str, 
						unique_counter);

			token_t result_token;
			result_token.id = TMP_ID;
			result_token.a.str = result_name;
			result_token.is_const = false;

			generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);

			if(code_list == &global_var_ilist)
				code_list = &instructs;

			unique_counter++;
			return result_name;
	}
	else if(id == INL_LEN_ID)
	{
		token_t param;
		token_t str_len;
		//token_t conv_token;
		//token_t return_token;

		//conv_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		param = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *tmp_name = smalloc(counter_length+8);
		if(tmp_name == NULL)
		{
			return NULL;
		}

		snprintf(tmp_name, counter_length+8, "%s%d", "LF@%tmp", unique_counter);

		str_len.id = TMP_ID;
		str_len.a.str = tmp_name;
		str_len.is_const = false;

		generate_instruction(code_list, DEFVAR_INS_ID, 1, str_len);
		generate_instruction(code_list, STRLEN_INS_ID, 2, param, str_len);
		/*generate_instruction(code_list, PUSHS_INS_ID, 1, str_len);
		generate_instruction(code_list, DEFVAR_INS_ID, 1, str_len);
		generate_instruction(code_list, POPS_INS_ID, 1, str_len);*/

		unique_counter++;
		return tmp_name;
	}
	else if(id == INL_CHR_ID)
	{
		token_t char_val_token;
		token_t str_len;
		token_t conv_token;
		//token_t return_token;

		conv_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		char_val_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *tmp_name = smalloc(counter_length+8);
		if(tmp_name == NULL)
		{
			return NULL;
		}

		snprintf(tmp_name, counter_length+8, "%s%d", "LF@%tmp", unique_counter);

		str_len.id = TMP_ID;
		str_len.a.str = tmp_name;
		str_len.is_const = false;

		//generate_instruction(code_list, PUSHFRAME_INS_ID, 0);
		generate_instruction(code_list, PUSHS_INS_ID, 1, char_val_token);
		if(conv_token.id == INTEGER_ID)
			generate_instruction(code_list, FLOAT2R2EINTS_INS_ID, 0);
		generate_instruction(code_list, INT2CHARS_INS_ID, 0);
		//generate_instruction(code_list, POPFRAME_INS_ID, 0);
		generate_instruction(code_list, DEFVAR_INS_ID, 1, str_len);
		generate_instruction(code_list, POPS_INS_ID, 1, str_len);

		unique_counter++;
		return tmp_name;
	}
	else if(id == INL_ASC_ID)
	{
		token_t str_token;
		token_t pos_token;
		token_t str_len_token;
		token_t ret_token;
		token_t int0_token, int1_token;
		token_t true_token;
		token_t label_token;
		token_t conv_pos_token;
		//token_t conv_str_token;

		tstack_pop(instruction_stack);
		str_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		conv_pos_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		pos_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *return_name = smalloc(strlen("LF@%tmp") + counter_length + 1);
		if(return_name == NULL)
		{
			return NULL;
		}

		snprintf(return_name, strlen("LF@%tmp") + counter_length + 1, "%s%d", 
					"LF@%tmp", unique_counter);

		ret_token.id = TMP_ID;
		ret_token.a.str = return_name;
		ret_token.is_const = false;

		char *str_len = smalloc(strlen("LF@*str_len") + counter_length + 1);
		if(str_len == NULL)
		{
			return NULL;
		}

		snprintf(str_len, strlen("LF@*str_len") + counter_length + 1, "%s%d", 
					"LF@*str_len", unique_counter);

		str_len_token.id = TMP_ID;
		str_len_token.a.str = str_len;
		int0_token.type = INTEGER_ID;
		int0_token.is_const = true;
		int0_token.a.val_int = 0;
		int1_token.type = INTEGER_ID;
		int1_token.is_const = true;
		int1_token.a.val_int = 1;
		true_token.id = TOKEN_TRUE_ID;
		true_token.type = BOOLEAN_ID;
		true_token.is_const = true;
		true_token.a.val_bool = true;

		generate_label(instruction_stack, INL_ASC_ID);

		//generate_instruction(code_list, PUSHFRAME_INS_ID, 0);

		generate_instruction(code_list, DEFVAR_INS_ID, 1, ret_token);
		generate_instruction(code_list, MOVE_INS_ID, 2, int0_token, ret_token);
		generate_instruction(code_list, DEFVAR_INS_ID, 1, str_len_token);
		generate_instruction(code_list, STRLEN_INS_ID, 2, str_token, 
									str_len_token); 
		generate_instruction(code_list, PUSHS_INS_ID, 1, pos_token);
		if(conv_pos_token.type == INTEGER_ID)
			generate_instruction(code_list, FLOAT2R2EINTS_INS_ID, 0);
		generate_instruction(code_list, PUSHS_INS_ID, 1, str_len_token);
		generate_instruction(code_list, GTS_INS_ID, 0);
		generate_instruction(code_list, PUSHS_INS_ID, 1, true_token);

		label_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		
		generate_instruction(code_list, JUMPIFEQS_INS_ID, 1, label_token);
		generate_instruction(code_list, PUSHS_INS_ID, 1, str_token);
		generate_instruction(code_list, PUSHS_INS_ID, 1, pos_token);
		generate_instruction(code_list, PUSHS_INS_ID, 1, int1_token);
		generate_instruction(code_list, SUBS_INS_ID, 0);
		generate_instruction(code_list, STRI2INTS_INS_ID, 0);
		generate_instruction(code_list, POPS_INS_ID, 1, ret_token);
		generate_instruction(code_list, LABEL_INS_ID,1, label_token);
		/*generate_instruction(code_list, PUSHS_INS_ID, 1, ret_token);
		//generate_instruction(code_list, POPFRAME_INS_ID, 0);
		generate_instruction(code_list, DEFVAR_INS_ID, 1, ret_token);
		generate_instruction(code_list, POPS_INS_ID, 1, ret_token);*/

		unique_counter++;
		return return_name;
	}
	else if(id == INL_SUBSTR_ID)
	{
		token_t str_token, pos_token, siz_token;
		token_t pos_name_token;
		token_t result_token;
		token_t str_char_token, str_len_token, loop_end_token, available_token, 
				  siz_overflow_token, pos_incorrect_token;
		token_t int0_token, int1_token, true_token, string0_token;
		token_t label_token;
		token_t siz_conv_token, pos_conv_token;

		//str_conv_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		str_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		pos_conv_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		pos_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		siz_conv_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		siz_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		
		

		int0_token.type = INTEGER_ID;
		int0_token.is_const = true;
		int0_token.a.val_int = 0;
		int1_token.type = INTEGER_ID;
		int1_token.is_const = true;
		int1_token.a.val_int = 1;
		true_token.id = TOKEN_TRUE_ID;
		true_token.type = BOOLEAN_ID;
		true_token.is_const = true;
		true_token.a.val_bool = true;
		string0_token.type = STRING_ID;
		string0_token.is_const = true;
		string0_token.a.str = "";


		token_conversion(code_list, siz_conv_token.id, &siz_token, &unique_counter);
		token_conversion(code_list, pos_conv_token.id, &pos_token, &unique_counter);


		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		char *return_name = smalloc(strlen("LF@%tmp") + counter_length + 1);
		if(return_name == NULL)
		{
			return NULL;
		}

		snprintf(return_name, strlen("LF@%tmp") + counter_length + 1, "%s%d", 
					"LF@%tmp", unique_counter);

		result_token.id = TMP_ID;
		result_token.a.str = return_name;
		result_token.is_const = false;

		char *pos_name = smalloc(strlen("LF@%%pos") + counter_length + 1);
		if(pos_name == NULL)
		{
			return NULL;
		}

		snprintf(pos_name, strlen("LF@%%pos") + counter_length + 1, "%s%d", 
					"LF@%%pos", unique_counter);

		pos_name_token.id = TMP_ID;
		pos_name_token.a.str = pos_name;
		pos_name_token.is_const = false;

		char *str_len_name = smalloc(strlen("LF@*str_len") + counter_length +1);
		if(str_len_name == NULL)
		{
			return NULL;
		}

		snprintf(str_len_name, strlen("LF@*str_len") + counter_length +1, "%s%d", 
					"LF@*str_len", unique_counter);

		str_len_token.id = TMP_ID;
		str_len_token.a.str = str_len_name;
		str_len_token.is_const = false;

		char *loop_end_name = smalloc(strlen("LF@*loop_end") + counter_length +1);
		if(loop_end_name == NULL)
		{
			return NULL;
		}

		snprintf(loop_end_name, strlen("LF@*loop_end") + counter_length +1, 
					"%s%d", "LF@*loop_end", unique_counter);

		loop_end_token.id = TMP_ID;
		loop_end_token.a.str = loop_end_name;
		loop_end_token.is_const = false;

		char *str_char_name = smalloc(strlen("LF@*str_char") + counter_length +1);
		if(str_char_name == NULL)
		{
			return NULL;
		}

		snprintf(str_char_name, strlen("LF@*str_char") + counter_length +1, 
					"%s%d", "LF@*str_char", unique_counter);

		str_char_token.id = TMP_ID;
		str_char_token.a.str = str_char_name;
		str_char_token.is_const = false;

		char *available_name = smalloc(strlen("LF@*available") + 
				counter_length +1);
		if(available_name == NULL)
		{
			return NULL;
		}

		snprintf(available_name, strlen("LF@*available") + counter_length +1, 
					"%s%d", "LF@*available", unique_counter);

		available_token.id = TMP_ID;
		available_token.a.str = available_name;
		available_token.is_const = false;

		char *siz_overflow_name = smalloc(strlen("LF@*siz_overflow") + 
				counter_length +1);
		if(siz_overflow_name == NULL)
		{
			return NULL;
		}

		snprintf(siz_overflow_name, strlen("LF@*siz_overflow") + 
					counter_length +1, "%s%d", "LF@*siz_overflow", unique_counter);

		siz_overflow_token.id = TMP_ID;
		siz_overflow_token.a.str = siz_overflow_name;
		siz_overflow_token.is_const = false;

		char *pos_incorrect_name = smalloc(strlen("LF@*pos_incorrect") + 
				counter_length +1);
		if(pos_incorrect_name == NULL)
		{
			return NULL;
		}

		snprintf(pos_incorrect_name, strlen("LF@*pos_incorrect") + 
					counter_length +1, "%s%d", "LF@*pos_incorrect", unique_counter);

		pos_incorrect_token.id = TMP_ID;
		pos_incorrect_token.a.str = pos_incorrect_name;
		pos_incorrect_token.is_const = false;

		generate_label(instruction_stack, INL_SUBSTR_ID);

		//generate_instruction(code_list, PUSHFRAME_INS_ID, 0);

		if(pos_token.is_const)
		{
			generate_instruction(code_list, DEFVAR_INS_ID, 1, pos_name_token);
			generate_instruction(code_list, MOVE_INS_ID, 2, pos_token, pos_name_token);
			pos_token = pos_name_token;
		}
		generate_instruction(code_list, DEFVAR_INS_ID, 1, result_token);
		generate_instruction(code_list, MOVE_INS_ID, 2, string0_token, result_token);
		generate_instruction(code_list, DEFVAR_INS_ID, 1, str_len_token);
		generate_instruction(code_list, STRLEN_INS_ID, 2, str_token, str_len_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, JUMPIFEQ_INS_ID, 3, label_token, int0_token, str_len_token);
		generate_instruction(code_list, DEFVAR_INS_ID, 1, pos_incorrect_token);
		generate_instruction(code_list, LT_INS_ID, 3, int1_token, pos_token, pos_incorrect_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, JUMPIFEQ_INS_ID, 3, label_token, true_token, pos_incorrect_token);
		generate_instruction(code_list, EQ_INS_ID, 3, int0_token, siz_token, pos_incorrect_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, JUMPIFEQ_INS_ID, 3, label_token, true_token, pos_incorrect_token);
		generate_instruction(code_list, SUB_INS_ID, 3, int1_token, pos_token, pos_token);
		generate_instruction(code_list, GT_INS_ID, 3, str_len_token, pos_token, pos_incorrect_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, JUMPIFEQ_INS_ID, 3, label_token, true_token, pos_incorrect_token);
		generate_instruction(code_list, DEFVAR_INS_ID, 1, loop_end_token);
		generate_instruction(code_list, DEFVAR_INS_ID, 1, str_char_token);
		generate_instruction(code_list, DEFVAR_INS_ID, 1, available_token);
		generate_instruction(code_list, SUB_INS_ID, 3, pos_token, str_len_token, available_token);
		generate_instruction(code_list, DEFVAR_INS_ID, 1, siz_overflow_token);
		generate_instruction(code_list, GT_INS_ID, 3, available_token, siz_token, siz_overflow_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, JUMPIFEQ_INS_ID, 3, label_token, true_token, siz_overflow_token);
		generate_instruction(code_list, LT_INS_ID, 3, int0_token, siz_token, siz_overflow_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, JUMPIFEQ_INS_ID, 3, label_token, true_token, siz_overflow_token);
		generate_instruction(code_list, ADD_INS_ID, 3, siz_token, pos_token, available_token);
		generate_instruction(code_list, SUB_INS_ID, 3, int1_token, available_token, available_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, JUMP_INS_ID, 1, label_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, LABEL_INS_ID, 1, label_token);
		generate_instruction(code_list, MOVE_INS_ID, 2, str_len_token, available_token);
		generate_instruction(code_list, SUB_INS_ID, 3, int1_token, available_token, available_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, LABEL_INS_ID, 1, label_token);
		generate_instruction(code_list, GETCHAR_INS_ID, 3, pos_token, str_token, str_char_token);
		generate_instruction(code_list, CONCAT_INS_ID, 3, str_char_token, result_token, result_token);
		generate_instruction(code_list, ADD_INS_ID, 3, int1_token, pos_token, pos_token);
		generate_instruction(code_list, GT_INS_ID, 3, available_token, pos_token, loop_end_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, JUMPIFNEQ_INS_ID, 3, label_token, true_token, loop_end_token);
		label_token = tstack_top(instruction_stack);
				tstack_pop(instruction_stack);
		generate_instruction(code_list, LABEL_INS_ID, 1, label_token);
		//generate_instruction(code_list, PUSHS_INS_ID, 1, result_token);


		unique_counter++;
		return return_name;
	}

	else if(id == EXIT_ID)
	{
		token_t type_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		token_t result_token;

		if(type_token.id == FOR_ID)
		{
			int cnt = type_token.type;
			int cnt_length = 0;

			while(cnt != 0)
			{
				cnt /= 10;
				cnt_length++;
			}

			char *result_name = smalloc(cnt_length + strlen("$FOR_END_") + 1);
			if(result_name == NULL)
				return NULL;
			snprintf(result_name, cnt_length + strlen("$FOR_END_") + 1, "%s%d",
						"$FOR_END_", type_token.type);

			result_token.id = TMP_ID;
			result_token.is_const = false;
			result_token.a.str = result_name;

			generate_instruction(code_list, JUMP_INS_ID, 1, result_token);
		}
		else
		{
			int cnt = type_token.type;
			int cnt_length = 0;

			while(cnt != 0)
			{
				cnt /= 10;
				cnt_length++;
			}

			char *result_name = smalloc(cnt_length + strlen("$LOOP_END_") + 1);
			if(result_name == NULL)
				return NULL;
			snprintf(result_name, cnt_length + strlen("$LOOP_END_") + 1, "%s%d",
						"$LOOP_END_", type_token.type);

			result_token.id = TMP_ID;
			result_token.is_const = false;
			result_token.a.str = result_name;

			generate_instruction(code_list, JUMP_INS_ID, 1, result_token);
		}
	}
	else if(id == CONTINUE_ID)
	{
		token_t type_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);
		token_t result_token;

		if(type_token.id == FOR_ID)
		{
			int cnt = type_token.type;
			int cnt_length = 0;

			while(cnt != 0)
			{
				cnt /= 10;
				cnt_length++;
			}

			char *result_name = smalloc(cnt_length + strlen("$FOR_BEGIN_") + 1);
			if(result_name == NULL)
				return NULL;
			snprintf(result_name, cnt_length + strlen("$FOR_BEGIN_") + 1, "%s%d",
						"FOR_BEGIN_", type_token.type);

			result_token.id = TMP_ID;
			result_token.is_const = false;
			result_token.a.str = result_name;

			generate_instruction(code_list, JUMP_INS_ID, 1, result_token);
		}
		else
		{
			int cnt = type_token.type;
			int cnt_length = 0;

			while(cnt != 0)
			{
				cnt /= 10;
				cnt_length++;
			}

			char *result_name = smalloc(cnt_length + strlen("$LOOP_BEGIN_") + 1);
			if(result_name == NULL)
				return NULL;
			snprintf(result_name, cnt_length + strlen("$LOOP_BEGIN_") + 1, "%s%d",
						"$LOOP_BEGIN_", type_token.type);

			result_token.id = TMP_ID;
			result_token.is_const = false;
			result_token.a.str = result_name;

			generate_instruction(code_list, JUMP_INS_ID, 1, result_token);
		}
	}
	return NIL;
}






	
void generate_instruction(ilist_t *code_list, int type, int num,  ...)
{
	va_list args;
	va_start(args, num);

	token_t tmp_token[3];


	for(int i = 0; i < num; i++)
	{
		tmp_token[i] = va_arg(args, token_t);
	}

	code_t *new_code = smalloc(sizeof(code_t));
	if(new_code == NULL)
	{
		return;
	}

	new_code->operation_type = type;
	new_code->first_operand = tmp_token[0];
	new_code->second_operand = tmp_token[1];
	new_code->third_operand = tmp_token[2];

	if(ilist_insert_last(code_list, new_code))
	{
		return;
	}
}

void generate_label(tstack *instruction_stack, int id)
{
	static unsigned int unq_lab_cnt = 1;
	static unsigned int elsif_unq_lab_cnt = 1;
	int cnt_length_lab = 0;
	int cnt_length_elsif = 0;
	int temp_cnt_lab = unq_lab_cnt;
	int temp_cnt_elsif = elsif_unq_lab_cnt;


	if(id == IF_ID)
	{
		token_t counter_token = {0};
		counter_token.a.val_int = unq_lab_cnt;

		while(temp_cnt_lab != 0)
		{
			temp_cnt_lab /= 10;
			cnt_length_lab++;
		}

		char *result_name = smalloc(cnt_length_lab+strlen("$END_IF_")+1);
		if(result_name == NULL)
		{
			return;
		}

		snprintf(result_name, cnt_length_lab+strlen("$END_IF_")+1, "%s%u", 
					"$END_IF_", unq_lab_cnt);
		/****************************************************************/

		token_t result_token = {0};
		result_token.id = END_IF_LAB_ID;
		result_token.a.str = result_name;

		tstack_push(instruction_stack, &result_token);
		tstack_push(instruction_stack, &counter_token);

		//elsif_unq_lab_cnt = 1;
		unq_lab_cnt++;
	}
	else if(id == ELSEIF_ID)
	{
		token_t counter_token = tstack_top(instruction_stack);
		//tstack_pop(instruction_stack);

		temp_cnt_lab = counter_token.a.val_int;

		while(temp_cnt_lab != 0)
		{
			temp_cnt_lab /= 10;
			cnt_length_lab++;
		}
		while(temp_cnt_elsif != 0)
		{
			temp_cnt_elsif /= 10;
			cnt_length_elsif++;
		}

		char *result_name = smalloc(cnt_length_lab+cnt_length_elsif+11);
		if(result_name == NULL)
		{
			return;
		}

		snprintf(result_name, cnt_length_lab+cnt_length_elsif+11, "%s%u%s%u", 
					"$ELSE_IF_", counter_token.a.val_int, "_", elsif_unq_lab_cnt);
		/****************************************************************/

		token_t result_token = {0};
		result_token.id = ELSE_LAB_ID;
		result_token.a.str = result_name;

		tstack_push(instruction_stack, &result_token);
		//tstack_push(instruction_stack, &counter_token);
		elsif_unq_lab_cnt++;

	}
	else if(id == ELSE_ID)
	{
		token_t counter_token = tstack_top(instruction_stack);
		//tstack_pop(instruction_stack);

		temp_cnt_lab = counter_token.a.val_int;

		while(temp_cnt_lab != 0)
		{
			temp_cnt_lab /= 10;
			cnt_length_lab++;
		}

		char *result_name = smalloc(cnt_length_lab+strlen("$ELSE_")+1);
		if(result_name == NULL)
		{
			return;
		}

		snprintf(result_name, cnt_length_lab+strlen("$ELSE_")+1, "%s%u", 
					"$ELSE_", counter_token.a.val_int);
		/****************************************************************/

		token_t result_token = {0};
		result_token.id = ELSE_LAB_ID;
		result_token.a.str = result_name;

		tstack_push(instruction_stack, &result_token);
		//tstack_push(instruction_stack, &counter_token);
	}
	/*END_IF branch*/
	else if(id == END_IF_ID)
	{
		/****************************************************************/
		while(temp_cnt_lab != 0)
		{
			temp_cnt_lab /= 10;
			cnt_length_lab++;
		}

		char *result_name = smalloc(cnt_length_lab+9);
		if(result_name == NULL)
		{
			return;
		}

		snprintf(result_name, cnt_length_lab+9, "%s%u", "$END_IF_", unq_lab_cnt);
		/****************************************************************/

		token_t result_token = {0};
		result_token.id = END_IF_LAB_ID;
		result_token.a.str = result_name;

		tstack_push(instruction_stack, &result_token);

		elsif_unq_lab_cnt = 1;
		unq_lab_cnt++;
	}

	/*FOR NEXT LOOP*/
	/*Generate all labels accordingly*/
	else if(id == FOR_ID)
	{
		token_t for_end_lab = {0};
		token_t for_next_lab = {0};
		token_t for_code_lab = {0};
		token_t for_begin_lab = {0};

		while(temp_cnt_lab != 0)
		{
			temp_cnt_lab /= 10;
			cnt_length_lab++;
		}
		char *result_name1 = smalloc(cnt_length_lab+strlen("$FOR_BEGIN_")+1);
		if(result_name1 == NULL)
		{
			return;
		}
		char *result_name2 = smalloc(cnt_length_lab+strlen("$FOR_NEXT_")+1);
		if(result_name2 == NULL)
		{
			return;
		}
		char *result_name3 = smalloc(cnt_length_lab+strlen("$FOR_CODE_")+1);
		if(result_name3 == NULL)
		{
			return;
		}
		char *result_name4 = smalloc(cnt_length_lab+strlen("$FOR_END_")+1);
		if(result_name4 == NULL)
		{
			return;
		}

		snprintf(result_name1, cnt_length_lab+strlen("$FOR_BEGIN_")+1, "%s%u", 
					"$FOR_BEGIN_", unq_lab_cnt);
		for_begin_lab.id = LOOP_BEGIN_ID;
		for_begin_lab.a.str = result_name1;
		for_begin_lab.type = unq_lab_cnt;

		snprintf(result_name2, cnt_length_lab+strlen("$FOR_NEXT_")+1, "%s%u", 
					"$FOR_NEXT_", unq_lab_cnt);
		for_next_lab.id = FOR_NEXT_ID;
		for_next_lab.a.str = result_name2;

		snprintf(result_name3, cnt_length_lab+strlen("$FOR_CODE_")+1, "%s%u", 
					"$FOR_CODE_", unq_lab_cnt);
		for_code_lab.id = FOR_CODE_ID;
		for_code_lab.a.str = result_name3;

		snprintf(result_name4, cnt_length_lab+strlen("$FOR_END_")+1, "%s%u", 
					"$FOR_END_", unq_lab_cnt);
		for_end_lab.id = LOOP_END_ID;
		for_end_lab.a.str = result_name4;
		for_end_lab.type = unq_lab_cnt;

		tstack_push(instruction_stack, &for_end_lab);
		tstack_push(instruction_stack, &for_next_lab);
		tstack_push(instruction_stack, &for_code_lab);
		tstack_push(instruction_stack, &for_begin_lab);
		tstack_push(instruction_stack, &for_next_lab);
		tstack_push(instruction_stack, &for_code_lab);
		tstack_push(instruction_stack, &for_end_lab);
		tstack_push(instruction_stack, &for_begin_lab);

		unq_lab_cnt++;
	}
	else if(id == DO_ID)
	{
		/*token_t cnt_token;
		cnt_token.a.val_int = unq_lab_cnt;
		cnt_token.type = INTEGER_ID;
		cnt_token.is_const = true;*/

		while(temp_cnt_lab != 0)
		{
			temp_cnt_lab /= 10;
			cnt_length_lab++;
		}

		char *result_name = smalloc(cnt_length_lab+13);
		if(result_name == NULL)
		{
			return;
		}

		snprintf(result_name, cnt_length_lab+13, "%s%u", 
					"$LOOP_BEGIN_", unq_lab_cnt);

		token_t result_token = {0};
		result_token.id = LOOP_BEGIN_ID;
		result_token.a.str = result_name;
		result_token.type = unq_lab_cnt;

		tstack_push(instruction_stack, &result_token);
		//tstack_push(instruction_stack, &cnt_token);

		unq_lab_cnt++;
	}
	else if(id == LOOP_ID)
	{
		token_t cnt_token = tstack_top(instruction_stack);

		temp_cnt_lab = cnt_token.type;

		while(temp_cnt_lab != 0)
		{
			temp_cnt_lab /= 10;
			cnt_length_lab++;
		}

		char *result_name = smalloc(cnt_length_lab+11);
		if(result_name == NULL)
		{
			return;
		}

		snprintf(result_name, cnt_length_lab+11, "%s%u", 
					"$LOOP_END_", cnt_token.type);

		token_t result_token = {0};
		result_token.id = LOOP_END_ID;
		result_token.a.str = result_name;
		result_token.type = cnt_token.type;

		tstack_push(instruction_stack, &result_token);
	}

	else if(id == FUNCTION_ID)
	{
		token_t name_token = {0};

		name_token = tstack_top(instruction_stack);
		tstack_pop(instruction_stack);

		char *result_name = smalloc(strlen(name_token.a.str) + 2);
		if(result_name == NULL)
		{
			return;
		}

		snprintf(result_name, strlen(name_token.a.str) + 2, "$%s", 
					name_token.a.str);
		name_token.a.str = result_name;

		tstack_push(instruction_stack, &name_token);
	}
	else if(id == INL_ASC_ID)
	{
		token_t name_token = {0};

		while(temp_cnt_lab != 0)
		{
			temp_cnt_lab /= 10;
			cnt_length_lab++;
		}


		char *result_name = smalloc(strlen("$$asc_end") + cnt_length_lab + 1);
		if(result_name == NULL)
		{
			return;
		}

		snprintf(result_name, strlen("$$asc_end") + cnt_length_lab + 1, "%s%d", 
					"$$asc_end", unq_lab_cnt);
		name_token.id = TMP_ID;
		name_token.a.str = result_name;

		tstack_push(instruction_stack, &name_token);

		unq_lab_cnt++;
	}
	else if(id == INL_SUBSTR_ID)
	{
		token_t name_token = {0};

		while(temp_cnt_lab != 0)
		{
			temp_cnt_lab /= 10;
			cnt_length_lab++;
		}

		char *substr_end = smalloc(strlen("$$substr_end") + cnt_length_lab + 1);
		if(substr_end == NULL)
		{
			return;
		}

		snprintf(substr_end, strlen("$$substr_end") + cnt_length_lab + 1, "%s%d", 
					"$$substr_end", unq_lab_cnt);

		char *substr_loop = smalloc(strlen("$$substr_loop") + cnt_length_lab + 1);
		if(substr_loop == NULL)
		{
			return;
		}

		snprintf(substr_loop, strlen("$$substr_loop") + cnt_length_lab + 1, 
					"%s%d", "$$substr_loop", unq_lab_cnt);

		char *substr_siz_overflow = smalloc(strlen("$$substr_siz_overflow") + 
														cnt_length_lab + 1);
		if(substr_siz_overflow == NULL)
		{
			return;
		}

		snprintf(substr_siz_overflow, strlen("$$substr_siz_overflow") + 
					cnt_length_lab + 1, "%s%d", "$$substr_siz_overflow", 
					unq_lab_cnt);

		name_token.id = TMP_ID;
		name_token.a.str = substr_end;
		tstack_push(instruction_stack, &name_token);
		name_token.a.str = substr_loop;
		tstack_push(instruction_stack, &name_token);
		tstack_push(instruction_stack, &name_token);
		name_token.a.str = substr_siz_overflow;
		tstack_push(instruction_stack, &name_token);
		name_token.a.str = substr_loop;
		tstack_push(instruction_stack, &name_token);
		name_token.a.str = substr_siz_overflow;
		tstack_push(instruction_stack, &name_token);
		tstack_push(instruction_stack, &name_token);
		name_token.a.str = substr_end;
		tstack_push(instruction_stack, &name_token);
		tstack_push(instruction_stack, &name_token);
		tstack_push(instruction_stack, &name_token);
		tstack_push(instruction_stack, &name_token);

      unq_lab_cnt++;
	}
}

void instruction_print(ilist_t *instructs)
{
   static int flow_control = 1;
   ilist_elem_t *tmp_list_ptr = instructs->first;


   if(flow_control)
   {
      flow_control = 0;
      printf("%s\n", ".IFJcode17");
      instruction_print(&global_var_ilist);
      if(tmp_list_ptr->data.operation_type != LABEL_MAIN)
         printf("%s\n", "JUMP $$MAIN");
   }
	while(instructs->first != NULL)
	{
		tmp_list_ptr = instructs->first;

		/*ADD <var> <symb1> >symb2> instruction*/
		if(tmp_list_ptr->data.operation_type == ADD_INS_ID )
		{
			printf("%s ", "ADD");

			printf("%s ", tmp_list_ptr->data.third_operand.a.str);

			if(tmp_list_ptr->data.second_operand.is_const)
			{
				if(tmp_list_ptr->data.second_operand.type == INTEGER_ID)
				{
					printf("%s%d ", "int@", 
							tmp_list_ptr->data.second_operand.a.val_int);
				}
				else if(tmp_list_ptr->data.second_operand.type == DOUBLE_ID)
				{
					printf("%s%g ", "float@", 
							tmp_list_ptr->data.second_operand.a.val_real);
				}
			}
			else
			{
					printf("%s ", tmp_list_ptr->data.second_operand.a.str);
			}

			if(tmp_list_ptr->data.first_operand.is_const)
			{
				if(tmp_list_ptr->data.first_operand.type == INTEGER_ID)
				{
					printf("%s%d\n", "int@", 
							tmp_list_ptr->data.first_operand.a.val_int);
				}
				else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID)
				{
					printf("%s%g\n", "float@", 
							tmp_list_ptr->data.first_operand.a.val_real);
				}
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
			}
		}
		//else if(tmp_list_ptr->data.operation_type == ADDS_INS_ID )
		//else if(tmp_list_ptr->data.operation_type == AND_INS_ID )
		//else if(tmp_list_ptr->data.operation_type == ANDS_INS_ID )
		//else if(tmp_list_ptr->data.operation_type == BREAK_INS_ID )
		/*CALL <label> instruction*/
		else if(tmp_list_ptr->data.operation_type == CALL_INS_ID )
		{
			printf("%s %s\n", "CALL", tmp_list_ptr->data.first_operand.a.str);
		}
		/*CLEARS instruction*/
		else if(tmp_list_ptr->data.operation_type == CLEARS_INS_ID )
		{
			printf("%s\n", "CLEARS");
		}
		/*CONCAT instruction*/
		else if(tmp_list_ptr->data.operation_type == CONCAT_INS_ID )
		{
			printf("%s ", "CONCAT");

			printf("%s ", tmp_list_ptr->data.third_operand.a.str);

			if(tmp_list_ptr->data.second_operand.type == 
				STRING_ID && tmp_list_ptr->data.second_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.second_operand.a.str);
				printf("%s%s ", "string@", tmp_list_ptr->data.second_operand.a.str);
			}
			else
			{
				printf("%s ", tmp_list_ptr->data.second_operand.a.str);
			}

			if(tmp_list_ptr->data.first_operand.type == STRING_ID && 
				tmp_list_ptr->data.first_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.first_operand.a.str);
				printf("%s%s\n", "string@", tmp_list_ptr->data.first_operand.a.str);
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
			}
		}
		/*CREATEFRAME instruction*/
		else if(tmp_list_ptr->data.operation_type == CREATEFRAME_INS_ID )
		{
			printf("%s\n", "CREATEFRAME");
		}
		/*DEFVAR <var> instruction*/
		else if(tmp_list_ptr->data.operation_type == DEFVAR_INS_ID )
		{
			printf("%s %s\n", "DEFVAR", tmp_list_ptr->data.first_operand.a.str);
		}
		/*DIV <var> <symb1> >symb2> instruction*/
		else if(tmp_list_ptr->data.operation_type == DIV_INS_ID )
		{
			printf("%s %s ", "DIV", tmp_list_ptr->data.third_operand.a.str);

			if(tmp_list_ptr->data.second_operand.type == 
				INTEGER_ID && tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%d ", "int@", tmp_list_ptr->data.second_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.second_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%g ", "float@", 
							tmp_list_ptr->data.second_operand.a.val_real);
			}
			else
			{
				printf("%s ", tmp_list_ptr->data.second_operand.a.str);
			}

			if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%d\n", "int@", tmp_list_ptr->data.first_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%g\n", "float@", 
						tmp_list_ptr->data.first_operand.a.val_real);
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
			}
		}
		//else if(tmp_list_ptr->data->operation_type == DIVS_INS_ID )
		//else if(tmp_list_ptr->data->operation_type == DPRINT_INS_ID )
		/*EQ <var> <symb1> >symb2> instruction*/
		else if(tmp_list_ptr->data.operation_type == EQ_INS_ID )
		{
			printf("%s %s ", "EQ", tmp_list_ptr->data.third_operand.a.str);

			if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%d ", "int@", tmp_list_ptr->data.first_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%g ", "float@", tmp_list_ptr->data.first_operand.a.val_real);
			}
			else if(tmp_list_ptr->data.first_operand.type == STRING_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.first_operand.a.str);
				printf("%s%s ", "string@", tmp_list_ptr->data.first_operand.a.str);
			}
			else if(tmp_list_ptr->data.first_operand.id == TOKEN_TRUE_ID)
			{
				printf("%s ", "bool@true");
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				if(tmp_list_ptr->data.first_operand.a.val_bool)
				{
					printf("%s ", "bool@true");
				}
				else
				{
					printf("%s ", "bool@false");
				}
			}
			else
			{
				printf("%s ", tmp_list_ptr->data.first_operand.a.str);
			}

			if(tmp_list_ptr->data.second_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%d\n", "int@", tmp_list_ptr->data.second_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.second_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%g\n", "float@", 
							tmp_list_ptr->data.second_operand.a.val_real);
			}
			else if(tmp_list_ptr->data.second_operand.type == 
						STRING_ID && tmp_list_ptr->data.second_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.second_operand.a.str);
				printf("%s%s\n", "string@", tmp_list_ptr->data.second_operand.a.str);
			}
			else if(tmp_list_ptr->data.second_operand.id == TOKEN_TRUE_ID)
			{
				printf("%s\n", "bool@true");
			}
			else if(tmp_list_ptr->data.second_operand.type == 
						DOUBLE_ID && tmp_list_ptr->data.second_operand.is_const)
			{
				if(tmp_list_ptr->data.second_operand.a.val_bool)
				{
					printf("%s\n", "bool@true");
				}
				else
				{
					printf("%s\n", "bool@false");
				}
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.second_operand.a.str);
			}
		}
		//else if(tmp_list_ptr->data->operation_type == EQS_INS_ID )
		//else if(tmp_list_ptr->data->operation_type == FLOAT2INT_INS_ID )
		//else if(tmp_list_ptr->data->operation_type == FLOAT2INTS_INS_ID )
		/*FLOAT2R2EINT <var> <symb> instruction*/
		else if(tmp_list_ptr->data.operation_type == FLOAT2R2EINT_INS_ID )
		{
			printf("%s %s %s\n", "FLOAT2R2EINT", 
					tmp_list_ptr->data.first_operand.a.str, 
					tmp_list_ptr->data.second_operand.a.str);
		}
		/*FLOAT2R2EINTS stack instruction*/
		else if(tmp_list_ptr->data.operation_type == FLOAT2R2EINTS_INS_ID )
		{
			printf("%s\n", "FLOAT2R2EINTS");
		}
		//else if(tmp_list_ptr->data->operation_type == FLOAT2R2OINT_INS_ID )
		//else if(tmp_list_ptr->data->operation_type == FLOAT2R2OINTS_INS_ID )
		/*GETCHAR <var> <symb1> <symb2> instruction*/
		else if(tmp_list_ptr->data.operation_type == GETCHAR_INS_ID )
		{
			printf("%s %s ", "GETCHAR", 
					tmp_list_ptr->data.third_operand.a.str);

         if(tmp_list_ptr->data.second_operand.type == 
            INTEGER_ID && tmp_list_ptr->data.second_operand.is_const)
         {
            printf("%s%d ", "int@", tmp_list_ptr->data.second_operand.a.val_int);
         }
         else if(tmp_list_ptr->data.second_operand.type == 
                  DOUBLE_ID && tmp_list_ptr->data.second_operand.is_const)
         {
            printf("%s%g ", "float@", 
                  tmp_list_ptr->data.second_operand.a.val_real);
         }
         else if(tmp_list_ptr->data.second_operand.type == STRING_ID && 
                  tmp_list_ptr->data.second_operand.is_const)
         {
            //string_convert(&tmp_list_ptr->data.second_operand.a.str);
            printf("%s%s ", "string@", tmp_list_ptr->data.second_operand.a.str);
         }
         else if(tmp_list_ptr->data.second_operand.id == TOKEN_TRUE_ID)
         {
            printf("%s ", "bool@true");
         }
         else if(tmp_list_ptr->data.second_operand.type == BOOLEAN_ID && 
                  tmp_list_ptr->data.second_operand.is_const)
         {
            if(tmp_list_ptr->data.second_operand.a.val_bool)
            {
               printf("%s ", "bool@true");
            }
            else
            {
               printf("%s ", "bool@false");
            }
         }
         else
         {
            printf("%s ", tmp_list_ptr->data.second_operand.a.str);
         }

         if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
            tmp_list_ptr->data.first_operand.is_const)
         {
            printf("%s%d\n", "int@", tmp_list_ptr->data.first_operand.a.val_int);
         }
         else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
                  tmp_list_ptr->data.first_operand.is_const)
         {
            printf("%s%g\n", "float@", tmp_list_ptr->data.first_operand.a.val_real);
         }
         else if(tmp_list_ptr->data.first_operand.type == STRING_ID && 
                  tmp_list_ptr->data.first_operand.is_const)
         {
            //string_convert(&tmp_list_ptr->data.first_operand.a.str);
            printf("%s%s\n", "string@", tmp_list_ptr->data.first_operand.a.str);
         }
         else if(tmp_list_ptr->data.first_operand.id == TOKEN_TRUE_ID)
         {
            printf("%s\n", "bool@true");
         }
         else if(tmp_list_ptr->data.first_operand.type == 
                  BOOLEAN_ID && tmp_list_ptr->data.first_operand.is_const)
         {
            if(tmp_list_ptr->data.first_operand.a.val_bool)
            {
               printf("%s\n", "bool@true");
            }
            else
            {
               printf("%s\n", "bool@false");
            }
         }
         else
         {
            printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
         }
      
		}
		/*GT <var> <symb1> >symb2> instruction*/
		else if(tmp_list_ptr->data.operation_type == GT_INS_ID )
			{
			printf("%s %s ", "GT", tmp_list_ptr->data.third_operand.a.str);

			if(tmp_list_ptr->data.second_operand.type == 
				INTEGER_ID && tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%d ", "int@", tmp_list_ptr->data.second_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.second_operand.type == 
						DOUBLE_ID && tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%g ", "float@", 
						tmp_list_ptr->data.second_operand.a.val_real);
			}
			else if(tmp_list_ptr->data.second_operand.type == STRING_ID && 
						tmp_list_ptr->data.second_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.second_operand.a.str);
				printf("%s%s ", "string@", tmp_list_ptr->data.second_operand.a.str);
			}
			else if(tmp_list_ptr->data.second_operand.id == TOKEN_TRUE_ID)
			{
				printf("%s ", "bool@true");
			}
			else if(tmp_list_ptr->data.second_operand.type == BOOLEAN_ID && 
						tmp_list_ptr->data.second_operand.is_const)
			{
				if(tmp_list_ptr->data.second_operand.a.val_bool)
				{
					printf("%s ", "bool@true");
				}
				else
				{
					printf("%s ", "bool@false");
				}
			}
			else
			{
				printf("%s ", tmp_list_ptr->data.second_operand.a.str);
			}

			if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%d\n", "int@", tmp_list_ptr->data.first_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%g\n", "float@", tmp_list_ptr->data.first_operand.a.val_real);
			}
			else if(tmp_list_ptr->data.first_operand.type == STRING_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.first_operand.a.str);
				printf("%s%s\n", "string@", tmp_list_ptr->data.first_operand.a.str);
			}
			else if(tmp_list_ptr->data.first_operand.id == TOKEN_TRUE_ID)
			{
				printf("%s\n", "bool@true");
			}
			else if(tmp_list_ptr->data.first_operand.type == 
						BOOLEAN_ID && tmp_list_ptr->data.first_operand.is_const)
			{
				if(tmp_list_ptr->data.first_operand.a.val_bool)
				{
					printf("%s\n", "bool@true");
				}
				else
				{
					printf("%s\n", "bool@false");
				}
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
			}
		}
		/*GTS stack instruction*/
		else if(tmp_list_ptr->data.operation_type == GTS_INS_ID )
		{
			printf("%s\n", "GTS");
		}
		/*INT2FLOAT <var> <symb> instruction*/
		else if(tmp_list_ptr->data.operation_type == INT2FLOAT_INS_ID )
		{
			printf("%s %s %s\n", "INT2FLOAT", 
					tmp_list_ptr->data.first_operand.a.str, 
					tmp_list_ptr->data.second_operand.a.str);
		}
		//else if(tmp_list_ptr->data->operation_type == INT2FLOATS_INS_ID )
		//else if(tmp_list_ptr->data->operation_type == INT2CHAR_INS_ID )
		/*INT2CHARS stack instruction*/
		else if(tmp_list_ptr->data.operation_type == INT2CHARS_INS_ID )
		{
			printf("%s\n", "INT2CHARS");
		}

		/*JUMP <label> instruction*/
		else if(tmp_list_ptr->data.operation_type == JUMP_INS_ID )
		{
			printf("%s %s\n", "JUMP", tmp_list_ptr->data.first_operand.a.str);
		}
		/*JUMPIFEQ <label> <symbol1> <symbol2> instruction*/
		else if(tmp_list_ptr->data.operation_type == JUMPIFEQ_INS_ID )
		{
			if(tmp_list_ptr->data.second_operand.type == BOOLEAN_ID && 
				tmp_list_ptr->data.second_operand.is_const && 
				tmp_list_ptr->data.third_operand.type == BOOLEAN_ID &&
				tmp_list_ptr->data.third_operand.is_const)
			{
				if(tmp_list_ptr->data.second_operand.a.val_bool && 
					tmp_list_ptr->data.third_operand.a.val_bool)
				{
					printf("%s %s\n", "JUMP", tmp_list_ptr->data.first_operand.a.str);
				}
			}
			else
			{
				printf("%s %s ", "JUMPIFEQ", tmp_list_ptr->data.first_operand.a.str);

				if(tmp_list_ptr->data.second_operand.type == INTEGER_ID &&
					tmp_list_ptr->data.second_operand.is_const)
				{
					printf("int@%d ", tmp_list_ptr->data.second_operand.a.val_int);
				}
				else if(tmp_list_ptr->data.second_operand.type == DOUBLE_ID &&
					tmp_list_ptr->data.second_operand.is_const)
				{
					printf("float@%g ", tmp_list_ptr->data.second_operand.a.val_real);
				}
				else if(tmp_list_ptr->data.second_operand.type == STRING_ID &&
					tmp_list_ptr->data.second_operand.is_const)
				{
               //string_convert(&tmp_list_ptr->data.second_operand.a.str);
					printf("string@%s ", tmp_list_ptr->data.second_operand.a.str);
				}
				else if(tmp_list_ptr->data.second_operand.type == BOOLEAN_ID &&
					tmp_list_ptr->data.second_operand.is_const)
				{
					if(tmp_list_ptr->data.second_operand.a.val_bool)
					{
						printf("%s ", "bool@true");
					}
					else
					{
						printf("%s ", "bool@false");
					}
				}
				else
				{
					printf("%s ", tmp_list_ptr->data.second_operand.a.str);
				}


				if(tmp_list_ptr->data.third_operand.type == INTEGER_ID &&
					tmp_list_ptr->data.third_operand.is_const)
				{
					printf("int@%d\n", tmp_list_ptr->data.third_operand.a.val_int);
				}
				else if(tmp_list_ptr->data.third_operand.type == DOUBLE_ID &&
					tmp_list_ptr->data.third_operand.is_const)
				{
					printf("float@%g\n", tmp_list_ptr->data.third_operand.a.val_real);
				}
				else if(tmp_list_ptr->data.third_operand.type == STRING_ID &&
					tmp_list_ptr->data.third_operand.is_const)
				{
               //string_convert(&tmp_list_ptr->data.third_operand.a.str);
					printf("string@%s\n", tmp_list_ptr->data.third_operand.a.str);
				}
				else if(tmp_list_ptr->data.third_operand.type == BOOLEAN_ID &&
					tmp_list_ptr->data.third_operand.is_const)
				{
					if(tmp_list_ptr->data.third_operand.a.val_bool)
					{
						printf("%s\n", "bool@true");
					}
					else
					{
						printf("%s\n", "bool@false");
					}
				}
				else
				{
					printf("%s\n", tmp_list_ptr->data.third_operand.a.str);
				}
			}
		}
		/*JUMPIFEQS stack instruction*/
		else if(tmp_list_ptr->data.operation_type == JUMPIFEQS_INS_ID )
		{
			printf("%s %s\n", "JUMPIFEQS", tmp_list_ptr->data.first_operand.a.str);
		}
		/*JUMPIFNEQ <label> <symbol1> <symbol2> instruction*/
		else if(tmp_list_ptr->data.operation_type == JUMPIFNEQ_INS_ID )
		{
			if(tmp_list_ptr->data.second_operand.type == BOOLEAN_ID && 
				tmp_list_ptr->data.second_operand.is_const && 
				tmp_list_ptr->data.third_operand.type == BOOLEAN_ID &&
				tmp_list_ptr->data.third_operand.is_const)
			{
				if(tmp_list_ptr->data.second_operand.a.val_bool ^ 
					tmp_list_ptr->data.third_operand.a.val_bool)
				{
					printf("%s %s\n", "JUMP", tmp_list_ptr->data.first_operand.a.str);
				}
			}
			else
			{
				printf("%s %s ", "JUMPIFNEQ", tmp_list_ptr->data.first_operand.a.str);

				if(tmp_list_ptr->data.second_operand.type == INTEGER_ID &&
					tmp_list_ptr->data.second_operand.is_const)
				{
					printf("int@%d ", tmp_list_ptr->data.second_operand.a.val_int);
				}
				else if(tmp_list_ptr->data.second_operand.type == DOUBLE_ID &&
					tmp_list_ptr->data.second_operand.is_const)
				{
					printf("float@%g ", tmp_list_ptr->data.second_operand.a.val_real);
				}
				else if(tmp_list_ptr->data.second_operand.type == STRING_ID &&
					tmp_list_ptr->data.second_operand.is_const)
				{
               //string_convert(&tmp_list_ptr->data.second_operand.a.str);
					printf("string@%s ", tmp_list_ptr->data.second_operand.a.str);
				}
				else if(tmp_list_ptr->data.second_operand.type == BOOLEAN_ID &&
					tmp_list_ptr->data.second_operand.is_const)
				{
					if(tmp_list_ptr->data.second_operand.a.val_bool)
					{
						printf("%s ", "bool@true");
					}
					else
					{
						printf("%s ", "bool@false");
					}
				}
				else
				{
					printf("%s ", tmp_list_ptr->data.second_operand.a.str);
				}


				if(tmp_list_ptr->data.third_operand.type == INTEGER_ID &&
					tmp_list_ptr->data.third_operand.is_const)
				{
					printf("int@%d\n", tmp_list_ptr->data.third_operand.a.val_int);
				}
				else if(tmp_list_ptr->data.third_operand.type == DOUBLE_ID &&
					tmp_list_ptr->data.third_operand.is_const)
				{
					printf("float@%g\n", tmp_list_ptr->data.third_operand.a.val_real);
				}
				else if(tmp_list_ptr->data.third_operand.type == STRING_ID &&
					tmp_list_ptr->data.third_operand.is_const)
				{
               //string_convert(&tmp_list_ptr->data.third_operand.a.str);
					printf("string@%s\n", tmp_list_ptr->data.third_operand.a.str);
				}
				else if(tmp_list_ptr->data.third_operand.type == BOOLEAN_ID &&
					tmp_list_ptr->data.third_operand.is_const)
				{
					if(tmp_list_ptr->data.third_operand.a.val_bool)
					{
						printf("%s\n", "bool@true");
					}
					else
					{
						printf("%s\n", "bool@false");
					}
				}
				else
				{
					printf("%s\n", tmp_list_ptr->data.third_operand.a.str);
				}
			}
		}
		/*JUMPIFNEQS <lab> stack instruction*/
		else if(tmp_list_ptr->data.operation_type == JUMPIFNEQS_INS_ID )
		{
			printf("%s %s\n", "JUMPIFNEQS", 
					tmp_list_ptr->data.first_operand.a.str);
		}
		/*LABEL <label> instruction*/
		else if(tmp_list_ptr->data.operation_type == LABEL_INS_ID )
		{
			printf("%s %s\n", "LABEL", tmp_list_ptr->data.first_operand.a.str);
		}
		/*LT <var> <symbol1> <symbol2> instruction*/
		else if(tmp_list_ptr->data.operation_type == LT_INS_ID )
		{
			printf("%s %s ", "LT", tmp_list_ptr->data.third_operand.a.str);

			if(tmp_list_ptr->data.second_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.second_operand.is_const) 
			{
				printf("%s%d ", "int@", 
						tmp_list_ptr->data.second_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.second_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%g ", "float@", 
						tmp_list_ptr->data.second_operand.a.val_real);
			}
			else if(tmp_list_ptr->data.second_operand.type == STRING_ID && 
						tmp_list_ptr->data.second_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.second_operand.a.str);
				printf("%s%s ", "string@", tmp_list_ptr->data.second_operand.a.str);
			}
			else if(tmp_list_ptr->data.second_operand.id == TOKEN_TRUE_ID)
			{
				printf("%s ", "bool@true");
			}
			else if(tmp_list_ptr->data.second_operand.type == 
						BOOLEAN_ID && tmp_list_ptr->data.second_operand.is_const)
			{
				if(tmp_list_ptr->data.second_operand.a.val_bool)
				{
					printf("%s ", "bool@true");
				}
				else
				{
					printf("%s ", "bool@false");
				}
			}
			else
			{
				printf("%s ", tmp_list_ptr->data.second_operand.a.str);
			}

			if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%d\n", "int@", 
						tmp_list_ptr->data.first_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%g\n", "float@", 
						tmp_list_ptr->data.first_operand.a.val_real);
			}
			else if(tmp_list_ptr->data.first_operand.type == STRING_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.first_operand.a.str);
				printf("%s%s\n", "string@", tmp_list_ptr->data.first_operand.a.str);
			}
			else if(tmp_list_ptr->data.first_operand.type == TOKEN_TRUE_ID)
			{
				printf("%s\n", "bool@true");
			}
			else if(tmp_list_ptr->data.first_operand.type == BOOLEAN_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				if(tmp_list_ptr->data.first_operand.a.val_bool)
				{
					printf("%s\n", "bool@true");
				}
				else
				{
					printf("%s\n", "bool@false");
				}
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
			}

		}
		/*LTS stack instruction*/
		else if(tmp_list_ptr->data.operation_type == LTS_INS_ID )
		{
			printf("%s\n", "LTS");
		}
		/*MOVE <var> <symbol1> instruction*/
		else if(tmp_list_ptr->data.operation_type == MOVE_INS_ID )
		{
			printf("%s %s ", "MOVE", tmp_list_ptr->data.second_operand.a.str);

			if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
					tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%d\n", "int@", 
						tmp_list_ptr->data.first_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%g\n", "float@", 
						tmp_list_ptr->data.first_operand.a.val_real);
			}
			else if(tmp_list_ptr->data.first_operand.id == TOKEN_TRUE_ID)
			{
				printf("%s\n", "bool@true");
			}
			else if(tmp_list_ptr->data.first_operand.type == STRING_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.first_operand.a.str);
				printf("%s%s\n", "string@", tmp_list_ptr->data.first_operand.a.str);
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
			}
		}
		/*MUL <var> <symbol1> <symbol2> instruction*/
		else if(tmp_list_ptr->data.operation_type == MUL_INS_ID )
		{
			printf("%s %s ", "MUL", tmp_list_ptr->data.third_operand.a.str);

			if(tmp_list_ptr->data.second_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%d ", "int@", 
						tmp_list_ptr->data.second_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.second_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%g ", "float@", 
						tmp_list_ptr->data.second_operand.a.val_real);
			}
			else
			{
				printf("%s ", tmp_list_ptr->data.second_operand.a.str);
			}

			if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%d\n", "int@", 
						tmp_list_ptr->data.first_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%g\n", "float@", 
						tmp_list_ptr->data.first_operand.a.val_real);
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
			}
			
		}
		//else if(tmp_list_ptr->data->operation_type == MULS_INS_ID )
		/*NOT <var> <symbol1> instruction*/
		else if(tmp_list_ptr->data.operation_type == NOT_INS_ID )
		{
			printf("%s %s %s\n", "NOT", tmp_list_ptr->data.second_operand.a.str, 
					tmp_list_ptr->data.first_operand.a.str);
		}
		//else if(tmp_list_ptr->data->operation_type == NOTS_INS_ID )
		/*OR <var> <symb1> <symb2> instruction*/
		else if(tmp_list_ptr->data.operation_type == OR_INS_ID )
		{
			printf("%s %s %s %s\n", "OR", tmp_list_ptr->data.third_operand.a.str, 
													tmp_list_ptr->data.first_operand.a.str, 
													tmp_list_ptr->data.second_operand.a.str);
		}
		//else if(tmp_list_ptr->data->operation_type == ORS_INS_ID )
		/*POPFRAME instruction*/
		else if(tmp_list_ptr->data.operation_type == POPFRAME_INS_ID )
		{
			printf("%s\n", "POPFRAME");
		}
		/*POPS <var> instruction*/
		else if(tmp_list_ptr->data.operation_type == POPS_INS_ID )
		{
			printf("%s %s\n", "POPS", tmp_list_ptr->data.first_operand.a.str);
		}
		/*PUSHFRAME instruction*/
		else if(tmp_list_ptr->data.operation_type == PUSHFRAME_INS_ID )
		{
			printf("%s\n", "PUSHFRAME");
		}
		/*PUSHS <symb> instruction*/
		else if(tmp_list_ptr->data.operation_type == PUSHS_INS_ID )
		{
			printf("%s ", "PUSHS");
			if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%d\n","int@", tmp_list_ptr->data.first_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%g\n","float@", 
						tmp_list_ptr->data.first_operand.a.val_real);
			}
			else if(tmp_list_ptr->data.first_operand.type == STRING_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.first_operand.a.str);
				printf("%s%s\n","string@", tmp_list_ptr->data.first_operand.a.str);
			}
			else if(tmp_list_ptr->data.first_operand.id == TOKEN_TRUE_ID)
			{
				printf("%s\n", "bool@true");
			}
			else
			{
				printf("%s\n",tmp_list_ptr->data.first_operand.a.str);
			}
		}
		/*READ <var> <type> instruction*/
		else if(tmp_list_ptr->data.operation_type == READ_INS_ID )
		{
			printf("%s %s ", "READ", tmp_list_ptr->data.first_operand.a.str);

			if(tmp_list_ptr->data.first_operand.type == INTEGER_ID)
			{
				printf("%s\n", "int");
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID)
			{
				printf("%s\n", "float");
			}
			else if(tmp_list_ptr->data.first_operand.type == STRING_ID)
			{
				printf("%s\n", "string");
			}
			else
			{
				printf("%s\n", "bool");
			}
		}
		/*RETURN instruction*/
		else if(tmp_list_ptr->data.operation_type == RETURN_INS_ID )
		{
			printf("%s\n", "RETURN");
		}
		//else if(tmp_list_ptr->data.operation_type == SETCHAR_INS_ID )
		/*STR2INT <var> <symb1> <symb2> instruction*/
		else if(tmp_list_ptr->data.operation_type == STRI2INT_INS_ID )
		{
			printf("%s %s %s %d\n", "STR2INT", 
										tmp_list_ptr->data.third_operand.a.str, 
										tmp_list_ptr->data.first_operand.a.str, 
										tmp_list_ptr->data.second_operand.a.val_int);
		}
		/*STR2INTS instruction*/
		else if(tmp_list_ptr->data.operation_type == STRI2INTS_INS_ID )
		{
			printf("%s\n", "STRI2INTS");
		}
		/*STRLEN <var> <symb>*/
		else if(tmp_list_ptr->data.operation_type == STRLEN_INS_ID )
		{
			printf("%s %s ", "STRLEN", 
											tmp_list_ptr->data.second_operand.a.str);

         if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
            tmp_list_ptr->data.first_operand.is_const)
         {
            printf("%s%d\n", "int@", 
                     tmp_list_ptr->data.first_operand.a.val_int);
         }
         else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
                  tmp_list_ptr->data.first_operand.is_const)
         {
            printf("%s%g\n", "float@", 
                     tmp_list_ptr->data.first_operand.a.val_real);
         }
         else if(tmp_list_ptr->data.first_operand.type == STRING_ID && 
                  tmp_list_ptr->data.first_operand.is_const)
         {
            //string_convert(&tmp_list_ptr->data.first_operand.a.str);
            printf("%s%s\n", "string@", tmp_list_ptr->data.first_operand.a.str);
         }
         else if(tmp_list_ptr->data.first_operand.type == BOOLEAN_ID && 
                  tmp_list_ptr->data.first_operand.is_const)
         {
            if(tmp_list_ptr->data.first_operand.a.val_bool)
            {
               printf("%s\n", "bool@true");
            }
            else
            {
               printf("%s\n", "bool@false");
            }
         }
         else
         {
            printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
         }
		}
		else if(tmp_list_ptr->data.operation_type == SUB_INS_ID )
		{
			printf("%s ", "SUB");

			printf("%s ", tmp_list_ptr->data.third_operand.a.str);

			if(tmp_list_ptr->data.second_operand.type == INTEGER_ID && 
					tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%d ", "int@", 
							tmp_list_ptr->data.second_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.second_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%g ", "float@", 
						tmp_list_ptr->data.second_operand.a.val_real);
			}
			else
			{
				printf("%s ", tmp_list_ptr->data.second_operand.a.str);
			}

			if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%d\n", "int@", 
							tmp_list_ptr->data.first_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%g\n", "float@", 
						tmp_list_ptr->data.first_operand.a.val_real);
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
			}

			
		}
		/*SUBS instruction*/
		else if(tmp_list_ptr->data.operation_type == SUBS_INS_ID )
		{
			printf("%s\n", "SUBS");
		}
		//else if(tmp_list_ptr->data->operation_type == TYPE_INS_ID )
		else if(tmp_list_ptr->data.operation_type == WRITE_INS_ID )
		{
			printf("%s ", "WRITE");
			
			if(tmp_list_ptr->data.first_operand.type == INTEGER_ID && 
				tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%d\n", "int@", 
							tmp_list_ptr->data.first_operand.a.val_int);
			}
			else if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%g\n", "float@", 
							tmp_list_ptr->data.first_operand.a.val_real);
			}
			else if(tmp_list_ptr->data.first_operand.type == STRING_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
            //string_convert(&tmp_list_ptr->data.first_operand.a.str);
				printf("%s%s\n", "string@", tmp_list_ptr->data.first_operand.a.str);
			}
			else if(tmp_list_ptr->data.first_operand.type == BOOLEAN_ID && 
						tmp_list_ptr->data.first_operand.is_const)
			{
				if(tmp_list_ptr->data.first_operand.a.val_bool)
				{
					printf("%s\n", "bool@true");
				}
				else
				{
					printf("%s\n", "bool@false");
				}
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
			}
		}

		else if(tmp_list_ptr->data.operation_type == LABEL_MAIN)
		{
			printf("%s\n", "LABEL $$MAIN");
		}

		else if(tmp_list_ptr->data.operation_type == INT_DIV_INS_ID)
		{
			//printf("%s %s %s\n", "INT2FLOAT", tmp_list_ptr->data.second_operand.a.str, tmp_list_ptr->data.second_operand.a.str);
			//printf("%s %s %s\n", "INT2FLOAT", tmp_list_ptr->data.first_operand.a.str, tmp_list_ptr->data.first_operand.a.str);

			printf("%s %s ", "DIV", tmp_list_ptr->data.third_operand.a.str);

			if(tmp_list_ptr->data.second_operand.type == DOUBLE_ID && 
				tmp_list_ptr->data.second_operand.is_const)
			{
				printf("%s%g ", "float@", 
							tmp_list_ptr->data.second_operand.a.val_real);
			}
			else
			{
				printf("%s ", tmp_list_ptr->data.second_operand.a.str);
			}

			if(tmp_list_ptr->data.first_operand.type == DOUBLE_ID && 
				tmp_list_ptr->data.first_operand.is_const)
			{
				printf("%s%g\n", "float@", 
							tmp_list_ptr->data.first_operand.a.val_real);
			}
			else
			{
				printf("%s\n", tmp_list_ptr->data.first_operand.a.str);
			}
			printf("%s %s %s\n", "FLOAT2R2EINT", 
						tmp_list_ptr->data.third_operand.a.str, 
						tmp_list_ptr->data.third_operand.a.str);

		}
		ilist_delete_first(instructs);
	}
	return;
}


void order_ilist(ilist_t *ins_list)
{
	ilist_first(ins_list);
	code_t *code_ptr;
	int depth_cnt = 0;
	ilist_elem_t *iter_ptr;

	while(ilist_active(ins_list))
	{
		code_ptr = ilist_copy(ins_list);
		if(code_ptr->operation_type == LABEL_INS_ID)
		{
			if(code_ptr->first_operand.id == LOOP_BEGIN_ID)
			{
				depth_cnt = code_ptr->first_operand.type;
				iter_ptr = ins_list->activ->rneigh;

				while(iter_ptr->data.operation_type != LABEL_INS_ID ||
					(iter_ptr->data.first_operand.id != LOOP_END_ID ||
										(iter_ptr->data.first_operand.id == LOOP_END_ID &&
											iter_ptr->data.first_operand.type != depth_cnt)))
				{
					if(iter_ptr->data.operation_type == DEFVAR_INS_ID)
					{
						if(iter_ptr->data.first_operand.a.str[0] != 'T')
						{
							ilist_pre_insert(ins_list, &(iter_ptr->data));
							iter_ptr->data.operation_type = NO_PRINT_ID;
						}
					}
					iter_ptr = iter_ptr->rneigh;
				}
			}
		}


		ilist_next(ins_list);
	}
}

void token_conversion(ilist_t *code_list, int conv_id, token_t *op_token, int *counter)
{
	char *temp_name = NULL;
	token_t *tmp_token = smalloc(sizeof(token_t));
	if(tmp_token == NULL)
		return;
	int temp_counter = *counter;
	int counter_length = 0;

	tmp_token->a.str = op_token->a.str;

	if(conv_id == INTEGER_ID)
	{
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		temp_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(temp_name == NULL)
			return;

		snprintf(temp_name, counter_length + strlen("LF@%tmp") + 1, "%s%d", 
					"LF@%tmp", *counter);

		(*counter)++;

		op_token->a.str = temp_name;

		generate_instruction(code_list, DEFVAR_INS_ID, 1, *op_token);
		generate_instruction(code_list, FLOAT2R2EINT_INS_ID, 2, *op_token, *tmp_token);

		
	}
	else if(conv_id == DOUBLE_ID)
	{
		while(temp_counter != 0)
		{
			temp_counter /= 10;
			counter_length++;
		}

		temp_name = smalloc(counter_length + strlen("LF@%tmp") + 1);
		if(temp_name == NULL)
			return;

		snprintf(temp_name, counter_length + strlen("LF@%tmp") + 1, "%s%d", 
					"LF@%tmp", *counter);

		(*counter)++;

		op_token->a.str = temp_name;

		generate_instruction(code_list, DEFVAR_INS_ID, 1, *op_token);
		generate_instruction(code_list, INT2FLOAT_INS_ID, 2, *op_token, *tmp_token);

		
	}			
	
	return;
}

/*
void //string_convert(char **str)
{
   if(*str == NULL)
      return;

   char *result_str;
   char val_as_chars[5];
   int c = 0;
   int j = 0;
   //int result_len = strlen(*str) + 1;

   result_str = smalloc(strlen(*str) + 1);

   if(result_str == NULL)
      return;

   for(int i = 0; *str[i] != '\0'; i++)
   {
      if(!isprint((*str)[i]))
      {
         c = (int) (*str)[i];

        
 snprintf(val_as_chars, 5, "\\%.*d", 3,  c);
         srealloc(result_str, j + 4);

         if(result_str == NULL)
            return;

         for(int k = 0; k < 4; k++)
         {
            result_str[j] = val_as_chars[k];
            j++;
            result_str[j] = '\0';
         }
      }
      else
      {
         result_str[j] = (*str)[i];
         j++;
         result_str[j] = '\0';
      }
      //result_len = strlen(result_str) + 1;
   }
   *str = result_str;

   return;
}*/