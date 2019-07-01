/**
 * @file tokenstack.c
 * @brief Specialized stack of tokens for IFJ Project 2017 AT vut.fit.vutbr.cz.
 * @date 17.11.2017, last rev. 6.12.2017
 * @author Patrik Goldschmidt - xgolds00@stud.fit.vutbr.cz
 *
 * Module provides library functions for special stack adapted to the needs
 * of precedence syntactical analysis.  Stack is internally composed from
 * classic doubly-linked list and also provides special functions as
 * insert find top terminal or push after top terminal.
 */

#include "safe_malloc.h"
#include "tokenstack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int tstack_empty(tstack *tst)
{
   /*
    * If the bottom pointer is same as top pointer - sentinel is on the top.
    * Sentinel on the top is considered an empty stack.
    */
   if(tst->bot == tst->top)
      return TSTACK_EMPTY;
   else
      return TSTACK_NOT_EMPTY;
}

/*
 * Expands the stack by preallocating another "num" number of elements.
 * If the function returns != 0, allocation of some elements failed, but
 * successfully allocated elements are linked to the end of the stack already.
 */
int tstack_expand(tstack *tst, int num)
{
   titem *topptr_bkup = tst->top->next;      // Backup of top next pointer.
   titem *tprev;                             // Previous tstack item.
   titem *tnew = tst->top;                   // Newly allocated tstack item.

   /* Pre-allocate stack elements + check.  */
   for(int i = 0; i < num; i++)
   {
      tprev = tnew;
      tnew  = scalloc(1, sizeof(titem));

      /* If the allocation fails, recover top link and break.  */
      if(tnew == NULL)
      {
         tprev->next = topptr_bkup;
         return TSTACK_FAILURE;
      }

      tprev->next = tnew;
      tnew->prev  = tprev;
   }

   /* Recover initial link on the top element.  */
   tnew->next = topptr_bkup;

   return TSTACK_SUCCESS;
}

void tstack_free(tstack *tst)
{
   titem *titem_next = tst->bot;

   for(titem *titem_tbd; titem_next != NULL; )
   {
      titem_tbd  = titem_next;
      titem_next = titem_next->next;
      sfree(titem_tbd);
   }

   tst->bot = NULL;
   tst->top = NULL;
   tst->top_term = NULL;

   return;
}

token_t tstack_get_topterm(tstack *tst)
{
   return tst->top_term->data;
}

int tstack_init(tstack *tst)
{
   int outcode;               // Output code for tstack_expand() call.
   titem *titem_sent;         // Sentinel list item.

   /* Allocate stack sentinel + check.   */
   titem_sent = scalloc(1, sizeof(titem));

   if(titem_sent == NULL)
      return TSTACK_FAILURE;

   /* Set up sentinel IDs.  Next and previous pointers are already NULL.   */
   titem_sent->data.id = EXPR_END_ID;

   /* Set initial values of stack pointers.  */
   tst->bot       = titem_sent;
   tst->top       = titem_sent;
   tst->top_term  = titem_sent;

   /* Pre-allocate stack elements + check.  */
   outcode = tstack_expand(tst, TSTACK_INIT_SIZ);

   if(outcode)
   {
      tstack_free(tst);
      return TSTACK_FAILURE;
   }

   return TSTACK_SUCCESS;
}

void tstack_pop(tstack *tst)
{
   /* Pop only if the sentinel value is not currently on the top. */
   if(tst->bot != tst->top)
   {
      /* If the popped token is terminal, find another closest to the top. */
      if(tst->top == tst->top_term)
      {
         titem *topterm = tst->top->prev;

         while(topterm->prev != NULL)
         {
            if(topterm->data.id < TERM_DELIM)
            {
               tst->top_term = topterm;
               break;
            }

            topterm = topterm->prev;
         }

         /* If terminal was not found in the cycle above, terminal closest to
          * the top is the sentinel value.
          */
         if(topterm == tst->bot)
            tst->top_term = tst->bot;
      }

      /* Make a pop.   */
      tst->top = tst->top->prev;
   }

   return;
}

int tstack_push(tstack *tst, token_t *token)
{
   /* If the stack has already reached its top, preallocate another chunk.   */
   if(tst->top->next == NULL)
   {
      int outcode;            // Output code for tstack_expand() call.

      outcode = tstack_expand(tst, TSTACK_MEM_CHUNK);

      if(outcode)
      {
         tstack_free(tst);
         return TSTACK_FAILURE;
      }
   }

   /* Update data and pointers -> make push. */
   tst->top = tst->top->next;
   memcpy(&(tst->top->data), token, sizeof(token_t));

   /* If the token is terminal, update terminal pointer on the top as well. */
   if(token->id < TERM_DELIM)
      tst->top_term = tst->top;

   return TSTACK_SUCCESS;
}

int tstack_push_after_topterm(tstack *tst, token_t *token)
{
   titem *tmp;                // Temporary titem data for push process.

   /* Check if there are enough pre-allocated items for the next push.   */
   if(tst->top->next == NULL)
   {
      int outcode;            // Output code for tstack_expand() function.

      /* Stack has reached its capacity. Preallocate more items. */
      outcode = tstack_expand(tst, TSTACK_MEM_CHUNK);

      if(outcode)
      {
         tstack_free(tst);
         return TSTACK_FAILURE;
      }
   }

   /* Take 1st item in the stack after the top to "tmp" and relink.  */
   tmp = tst->top->next;
   tst->top->next = tmp->next;
   tmp->next->prev = tst->top;

   /* Relink with top terminal - make insert.   */
   tmp->next = tst->top_term->next;
   tmp->prev = tst->top_term;
   tst->top_term->next = tmp;

   /* Relink previous link of next element with the inserted one. */
   tmp->next->prev = tmp;

   /* Update data of the inserted item.   */
   memcpy(&(tmp->data), token, sizeof(token_t));

   /* Previous top element was also top terminal - make a new top. */
   if(tst->top == tst->top_term)
      tst->top = tmp;

   /* Inserted element is also a terminal - make new top terminal. */
   if(tmp->data.id < TERM_DELIM)
      tst->top_term = tmp;

   return TSTACK_SUCCESS;
}

token_t tstack_top(tstack *tst)
{
   return tst->top->data;
}

token_t tstack_toppop(tstack *tst)
{
   token_t to_return = tstack_top(tst);
   tstack_pop(tst);
   return to_return;
}
