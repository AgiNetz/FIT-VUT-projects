/**
 * @file safe_malloc_test.c
 * @brief Stack of tokens data type for IFJ Project 2017 AT vut.fit.vutbr.cz.
 * @date 16.11.2017
 * @author Patrik Goldschmidt - xgolds00@fit.vutbr.cz
 *
 * Test module for safe_malloc testing.
 */

#include "../../src/safe_malloc.h"
#include <stdio.h>
#include <stdlib.h>

void memman_contents();
void memman_info();

extern memman_data mem_data;

int main()
{
   memman_init();

   /* Print initial contents. */
   printf("--------------------- INITIAL INFO ---------------------\n");
   memman_info();
   memman_contents();

   /* Allocate one integer.   */
   int *val = smalloc(sizeof(int));

   *val = 10;

   printf("Inicialized value: %d\n", *val);

   printf("--------------------- AFTER 1 MALLOC ---------------------\n");
   memman_info();
   memman_contents();

   /* Free the value.   */
   sfree(val);

   printf("------------------- FREE PREVIOUSLY MALLOCKED------------------\n");
   memman_info();
   memman_contents();

   for(int i = 0; i < 30; i++)
      val = smalloc(sizeof(int));


   printf("------------------- MAKE TEN MALLOCS------------------\n");
   memman_info();
   memman_contents();

   memman_clear();

   printf("------------------- CLEAR ALL ELEMENTS ------------------\n");
   memman_info();
   memman_contents();

   /* Force metadata table realloc. */
   void *random_alloc;

   for(int i = 0; i < 512; i++)
      random_alloc = smalloc(sizeof(memman_data));

   /* Make compiler happy. */
   (void)random_alloc;

   printf("------------------- ALLOC 512 ------------------\n");
   memman_info();

   printf("------------------- FORCE METADATA REALLOC ------------------\n");
   random_alloc = smalloc(20);

   memman_info();
   memman_contents();

   /* Force another reealloc. */
   printf("------------------- FORCE ANOTHER REALLOC ------------------\n");

   for(int i = 0; i < 257; i++)
      smalloc(sizeof(memman_data));

   memman_info();
   //memman_contents();

   printf("--------------- Clear everything ---------------\n");
   memman_clear();

   memman_info();
   memman_contents();

   printf("--------------- Make 2 indenepdent mallocs by 20 --------------\n");

   for(int i = 0; i < 20; i++)
      random_alloc = smalloc(sizeof(memman_data));

   for(int i = 0; i < 20; i++)
      smalloc(sizeof(memman_data));

   memman_info();
   memman_contents();

   printf("--------------- FREE element on index \"19\" ---------------\n");

   printf("Removing element: \t \t%p\n", random_alloc);
   printf("Moved to the 19th index: \t%p\n", mem_data.ptrs[39]);
   sfree(random_alloc);

   memman_info();
   memman_contents();

   /* Free everything including structure itself. */
   memman_free_all();

   return EXIT_SUCCESS;
}

void memman_contents()
{
   for(unsigned i = 0; i < mem_data.size; i++)
   {
      printf("%d. \t%p\n", i, mem_data.ptrs[i]);
   }
}

void memman_info()
{
   printf("Allocated: %u\n", mem_data.allocated);
   printf("Size: \t   %u\n", mem_data.size);
}
