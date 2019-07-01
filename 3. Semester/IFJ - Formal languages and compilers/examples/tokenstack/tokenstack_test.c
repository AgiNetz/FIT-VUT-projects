
/**
 * @file tokentest.c
 * @brief Stack of tokens data type for IFJ Project 2017 AT vut.fit.vutbr.cz.
 * @date 16.11.2017
 * @author Patrik Goldschmidt - xgolds00@fit.vutbr.cz
 *
 * Test module for tokenstack stuff.
 */

#include "../../src/tokenstack.h"
#include <stdio.h>
#include <stdlib.h>

void tstack_print(tstack *tst);

int tstack_count_alloc(tstack *tst);

int main()
{
   /* Create and initialize stack.  */
   tstack tokenst;                     // Stack of tokens
   token_t tok = {                     // Token
      .id = 13000,
      .a.val_int = 3,
   };

   tstack_init(&tokenst);

   printf("--------------------  Test after init:  --------------------\n");
   tstack_print(&tokenst);
   printf("Allocated size\t: %d\n", tstack_count_alloc(&tokenst));
   printf("Stack state is\t: ");
   printf(tstack_empty(&tokenst) ? "empty" : "not-empty");
   putchar('\n');

   /* Top empty stack - should return sentinel. */
   printf("Top element is\t: %d\n", tstack_top(&tokenst).id);
   printf("Top terminal is\t: %d\n", tstack_get_topterm(&tokenst).id);

   /* Insert few elements. */
   printf("--------------------  Test after inserts:  --------------------\n");
   tstack_push(&tokenst, &tok);
   tstack_push(&tokenst, &tok);
   tstack_push(&tokenst, &tok);


   tstack_print(&tokenst);
   printf("Allocated size\t: %d\n", tstack_count_alloc(&tokenst));
   printf("Stack state is\t: ");
   printf(tstack_empty(&tokenst) ? "empty" : "not-empty");
   putchar('\n');

   printf("Top element is\t: %d\n", tstack_top(&tokenst).id);
   printf("Top terminal is\t: %d\n", tstack_get_topterm(&tokenst).id);

   printf("--------------------  Test terminal insert:  --------------------\n");
   /* Change values: */
   tok.id = 7;
   tok.a.val_real = 14.47;

   tstack_push(&tokenst, &tok);

   tstack_print(&tokenst);
   printf("Top element is\t: %d\n", tstack_top(&tokenst).id);
   printf("Top terminal is\t: %d\n", tstack_get_topterm(&tokenst).id);

   printf("--------------------  Pop terminal insert:  --------------------\n");
   tstack_pop(&tokenst);

   tstack_print(&tokenst);
   printf("Top element is\t: %d\n", tstack_top(&tokenst).id);
   printf("Top terminal is\t: %d\n", tstack_get_topterm(&tokenst).id);

   printf("-------------------  Force stack expansion:  ------------------\n");
   for(int i = 0; i < TSTACK_INIT_SIZ; i++)
   {
      tok.id = i;
      tok.a.val_int = i;

      tstack_push(&tokenst, &tok);
   }

   tstack_print(&tokenst);
   printf("Allocated size\t: %d\n", tstack_count_alloc(&tokenst));
   printf("Stack state is\t: ");
   printf(tstack_empty(&tokenst) ? "empty" : "not-empty");
   putchar('\n');

   printf("Top element is\t: %d\n", tstack_top(&tokenst).id);
   printf("Top terminal is\t: %d\n", tstack_get_topterm(&tokenst).id);

   printf("------------  Unpop all and try stack overpopping:  -----------\n");

   /* Unpop everything out + make extra pops when empty.   */
      for(int i = 0; i < tstack_count_alloc(&tokenst); i++)
   {
      tstack_pop(&tokenst);
   }

   tstack_print(&tokenst);
   printf("Allocated size\t: %d\n", tstack_count_alloc(&tokenst));
   printf("Stack state is\t: ");
   printf(tstack_empty(&tokenst) ? "empty" : "not-empty");
   putchar('\n');

   printf("Top element is\t: %d\n", tstack_top(&tokenst).id);
   printf("Top terminal is\t: %d\n", tstack_get_topterm(&tokenst).id);



   printf("----------------  Push after topterm few times:  --------------\n");
   tok.id = 14;
   tstack_push_after_topterm(&tokenst, &tok);

   tok.id = 15;
   tstack_push_after_topterm(&tokenst, &tok);

   tok.id = 16;
   tstack_push_after_topterm(&tokenst, &tok);

   tstack_print(&tokenst);
   printf("Top element is\t: %d\n", tstack_top(&tokenst).id);
   printf("Top terminal is\t: %d\n", tstack_get_topterm(&tokenst).id);

   printf("--------------  Push new terminals after topterm:  ------------\n");
   tok.id = 7;
   tstack_push_after_topterm(&tokenst, &tok);

   tok.id = 8;
   tstack_push_after_topterm(&tokenst, &tok);

   tok.id = 9;
   tstack_push_after_topterm(&tokenst, &tok);

   tstack_print(&tokenst);
   printf("Top element is\t: %d\n", tstack_top(&tokenst).id);
   printf("Top terminal is\t: %d\n", tstack_get_topterm(&tokenst).id);

   printf("--------------  Free stack:  ------------\n");
   /* Free stack and check. */
   tstack_free(&tokenst);

   printf("Stack bottom\t\t: \t %p\n", (void *) tokenst.bot);
   printf("Stack top\t\t: \t %p\n", (void *) tokenst.top);
   printf("Stack top terminal\t: \t %p\n", (void *) tokenst.top_term);

   return EXIT_SUCCESS;
}

void tstack_print(tstack *tst)
{
   titem *tmp = tst->bot;

   while(tmp != tst->top)
   {
      printf("%d -> ", tmp->data.id);
      tmp = tmp->next;
   }

   printf("%d -> END\n", tmp->data.id);
}

int tstack_count_alloc(tstack *tst)
{
   int count = 0;

   for(titem *tmp = tst->bot->next; tmp != NULL; tmp = tmp->next)
      count++;

   return count;
}
