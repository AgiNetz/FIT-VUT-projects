/**
 * @file safe_malloc.c
 * @brief Safe malloc module for IFJ Project 2017 AT vut.fit.vutbr.cz.
 * @date 6.12.2017
 * @author Patrik Goldschmidt - xgolds00@stud.fit.vutbr.cz
 *
 * MEMMAN = Memory Management Module
 * Module provides a way to track all allocated memory during the program
 * execution and also allows to correctly free it all using one call. 
 *
 * Module is internally implemented as an array, which holds pointers to all
 * allocated memory that has been done though memory management module
 * functions.  Upon a free, address gets searched, freed, removed from the
 * array and last element is placed to this array index to keep array
 * proper array consistency. 
 */

#include "safe_malloc.h"
#include <stdlib.h>
#include <string.h>

static memman_data mem_data;

/*
 * Expands memory management data structure by MEMMAN_CHUNK_SIZ number of
 * elements.
 */ 
static int memman_expand()
{
   /* Expand current size by MEMMAN_CHUNK_SIZ + error checking. */
   void **newm = realloc(mem_data.ptrs, (mem_data.allocated + MEMMAN_CHUNK_SIZ)
                                       * sizeof(void *));

   if(newm == NULL)
      return MEMMAN_FAILURE;

   /* Memory was expanded correctly, update pointer allocated size.   */
   mem_data.ptrs = newm;
   mem_data.allocated += MEMMAN_CHUNK_SIZ;

   return MEMMAN_SUCCESS;
}

int memman_init()
{
   /* Alloc memory for initial number of elements + error checking.  */
   mem_data.ptrs = calloc(MEMMAN_INIT_SIZ, sizeof(void *));

   if(mem_data.ptrs == NULL)
      return MEMMAN_FAILURE;

   mem_data.allocated = MEMMAN_INIT_SIZ;
   mem_data.size = 0;

   return MEMMAN_SUCCESS;
}

void memman_clear()
{
   /* Iterate through whole array sequentially and free approached items.  */
   for(unsigned i = 0; i < mem_data.size; i++)
      free(mem_data.ptrs[i]);

   /* Set size of array of pointers to 0 and all pointers to NULL.   */
   mem_data.size = 0;
   memset(mem_data.ptrs, 0, mem_data.allocated);

   return;
}

void memman_free_all()
{
   memman_clear();
   free(mem_data.ptrs);
   mem_data.ptrs = NULL;
   mem_data.allocated = 0;

   return;
}

void * scalloc(size_t num, size_t size)
{
   int outcode = 0;;               // Output code of memman_expend function

   /* Check if metadata array has enough space for the new item.   */
   if(mem_data.size + 1 > mem_data.allocated)
      outcode = memman_expand();

   /* Check if the reallocation has not failed. */
   if(outcode == MEMMAN_FAILURE)
      return NULL;

   /* Allocate space for the new element + check.  */
   void *newptr = calloc(num, size);

   if(newptr == NULL)
      return NULL;

   /* Allocation was OK - save pointer to the array and increment size.  */
   mem_data.ptrs[mem_data.size] = newptr;
   mem_data.size++;

   return newptr;
}

void sfree(void *addr)
{
   /* Search for the desired address.  */
   unsigned i = 0;                // Index of the address in the pointer array.

   for(; addr != mem_data.ptrs[i] && i < mem_data.size; i++);

   if(i != mem_data.size)
   {
      /* When i != size of the array - memory address was found.  */
      free(addr);
      mem_data.ptrs[i] = mem_data.ptrs[mem_data.size - 1];
      mem_data.ptrs[mem_data.size - 1] = NULL;
      mem_data.size--;
   }
}

void * smalloc(size_t size)
{
   int outcode = 0;               // Output code of memman_expend function

   /* Check if metadata array has enough space for the new item.   */
   if(mem_data.size + 1 > mem_data.allocated)
      outcode = memman_expand();

   /* Check if the reallocation has not failed. */
   if(outcode == MEMMAN_FAILURE)
      return NULL;

   /* Allocate space for the new element + check.  */
   void *newptr = malloc(size);

   if(newptr == NULL)
      return NULL;

   /* Allocation was OK - save pointer to the array and increment size.  */
   mem_data.ptrs[mem_data.size] = newptr;
   mem_data.size++;

   return newptr;
}

void * srealloc(void *addr, size_t size)
{
   /* Search for the desired address.  */
   unsigned i = 0;                // Index of the address in the pointer array.
   void *newptr = NULL;           // Pointer to the allocated memory

   /* If input pointer is NULL.  */
   if(addr == NULL)
      return malloc(size);

   /* If the input address is not NULL, find the element and realloc.   */
   for(; addr != mem_data.ptrs[i] && i < mem_data.size; i++);

   /* If the element was found, realloc it, otherwise return NULL.   */
   if(i != mem_data.size)
   {
      /* Make realloc + error checking.   */
      newptr = realloc(addr, size);

      if(newptr == NULL)
         return NULL;

      /* Change the pointer to the realloced one.  */
      mem_data.ptrs[i] = newptr;
   }
   
   return newptr;   
}