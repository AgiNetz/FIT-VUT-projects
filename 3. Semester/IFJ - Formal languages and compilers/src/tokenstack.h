/**
 * @file tokenstack.h
 * @brief Specialized stack of tokens for IFJ Project 2017 AT vut.fit.vutbr.cz.
 * @date 17.11.2017, last rev. 6.12.2017
 * @author Patrik Goldschmidt - xgolds00@stud.fit.vutbr.cz
 *
 * Module provides library functions for special stack adapted to the needs
 * of precedence syntactical analysis.  Stack is internally composed from
 * classic doubly-linked list and also provides special functions as
 * insert find top terminal or push after top terminal.
 */

#ifndef TOKENSTACK_H_INCLUDED
#define TOKENSTACK_H_INCLUDED

#include "ifj17_api.h"

/** @brief Tstack not-empty flag.   */
#define TSTACK_NOT_EMPTY 0

/** @brief Tstack empty flag. */
#define TSTACK_EMPTY 1

/** @brief Tstack operation success identifier. */
#define TSTACK_SUCCESS 0

/** @brief Tstack operation failure identifier. */
#define TSTACK_FAILURE 1

/** @brief Size of stack for preallocation.  */
#define TSTACK_INIT_SIZ 100

/** @brief Number of elements to be added to stack when it is full.  */
#define TSTACK_MEM_CHUNK 30

typedef struct tstack_item
{
   token_t data;
   struct tstack_item *next;
   struct tstack_item *prev;
} titem;

/**
 * @brief Implementation of a stack of tokens as a classic linked list.
 *        This stack implementation keeps pointer to the top terminal.
 */
typedef struct token_stack
{
   titem *bot;           ///< Bottom of the stack.  Always the sentinel token.
   titem *top;           ///< Current element at the top of the stack.
   titem *top_term;      ///< First terminal closest to the top.
} tstack;

/**
 * @brief Determines if the stack is empty.  Stack is considered empty only
 *        when sentinel value is present.
 *
 * @param[in] *tst Pointer to tstack data type.
 * @return Non-zero if the stack is empty, zero otherwise.
 */
int tstack_empty(tstack *tst);

/**
 * @brief Clears stack contents and deallocates it.  After freeing a stack
 *        it needs to be reinitialized for potential future use.
 *
 * @param[in] *tst Pointer to tstack data type.
 * @return void.
 */
void tstack_free(tstack *tst);

/**
 * @brief Returns pointer to the terminal closest to the top.
 *
 * @param[in] *tst Pointer to tstack data type.
 * @return Pointer closest
 */
token_t tstack_get_topterm(tstack *tst);

/**
 * @brief Initializes stack and preallocates it to the defined value.
 *
 * @param[in] Pointer to tstack data type.
 * @return Zero if the initialization process was successful. Non-zero if
 *          error during initialization process occurred (e.g. memory
 *          allocation error).
 */
int tstack_init(tstack *tst);

/**
 * @brief Deletes element on the top of the stack.  If the stack is empty -
 *        only sentinel value is present, stack contents stay intact.
 *
 * @param[in] *tst Pointer to tstack data type.
 * @return void.
 */
void tstack_pop(tstack *tst);

/**
 * @brief Adds desired token to the top of the stack.
 *
 * @param[in] *tst Pointer to tstack data type.
 * @param[in] *token Pointer to the token_t type to be added.
 * @return Zero if the token was successfully pushed. Non-zero if an error
 *         has occurred.
 */
int tstack_push(tstack *tst, token_t *token);

/**
 * @brief Adds desired token after terminal closest to the top.
 *
 * @param[in] *tst Pointer to tstack data type.
 * @param[in] *token Pointer to the token_t type to be added.
 * @return Zero if the token was successfully pushed. Non-zero if an error
 *         has occurred.
 */
int tstack_push_after_topterm(tstack *tst, token_t *token);

/**
 * @brief Returns token on the top of the stack.  Be aware, that due to special
 *        stack implementation, topping an empty stack will return sentinel
 *        token_t value which is present through whole life cycle on the
 *        bottom of the stack.  To find out if stack is empty, use
 *        tstack_empty() instead.
 *
 * @param[in] *tst Pointer to tstack data type.
 * @return Token_t data of the topmost stack element.
 */
token_t tstack_top(tstack *tst);

/**
 * @brief Deletes element on the top of the stack and returns it.  If the stack
 *		  is empty - only sentinel value is present, stack contents stay intact.
 *
 * @param[in] *tst Pointer to tstack data type.
 * @return Token_t data of the popped top stack element.
 */
token_t tstack_toppop(tstack *tst);

#endif // TOKENSTACK_H_INCLUDED
