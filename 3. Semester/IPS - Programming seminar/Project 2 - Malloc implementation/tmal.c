/**
 * Implementace Thread Memory Allocator
 * 2. projekt IPS/2017
 * Autor: Tomas Danis - xdanis05
 */

#include "tmal.h"
#include <stdlib.h>
#include <string.h>

#define align(x) (((x) + (sizeof(size_t)-1))/(sizeof(size_t)))*sizeof(size_t)

struct blk_pool_t *blks_table = NULL;

struct blk_pool_t *tal_alloc_blks_table(unsigned nthreads)
{
   blks_table = malloc(nthreads * sizeof(struct blk_pool_t));
   return blks_table;
}

void blk_ctor(struct blk_t *blk)
{
   blk->next_idx = -1;
   blk->prev_idx = -1;
   blk->size = 0;
   blk->ptr = NULL;
   blk->used = false;
}

struct blk_t *tal_init_blks(unsigned tid, unsigned nblks, size_t theap)
{
   /* Allocate memory for metadata */
   blks_table[tid].blks = malloc(nblks * sizeof(struct blk_t));
   if(blks_table[tid].blks == NULL) return NULL;

   /* Initialize metadata */
   for(unsigned i = 0; i < nblks; i++)
   {
         blk_ctor(&BLK(tid, i));
   }

   /* Allocate the memory for the thread in one block */
   blks_table[tid].blks[0].ptr = malloc(theap);
   if(blks_table[tid].blks[0].ptr == NULL)
   {
      free(blks_table[tid].blks);
      return NULL;
   }

   blks_table[tid].blks[0].size = theap;

   blks_table[tid].heap_size = theap;
   blks_table[tid].nblks = nblks;

   return blks_table[tid].blks;
}

int tal_blk_split(unsigned tid, int blk_idx, size_t req_size)
{
   /*No splitting if the block is exactly the requested size */
   if(BLK(tid, blk_idx).size == req_size) return -1;

   /* Error if size of the block is smaller than the requested size */
   if(BLK(tid, blk_idx).size < req_size) return -1;

   /* Find the index of an unused metadata block */
   unsigned i = 0;
   while(i < blks_table[tid].nblks && BLK(tid, i).ptr != NULL) i++;

   if(i == blks_table[tid].nblks) return -1; //No such block

   BLK(tid, i).ptr = (char*)(BLK(tid, blk_idx).ptr) + req_size; //New block start
   BLK(tid, i).size = BLK(tid, blk_idx).size - req_size; //Size correction
   BLK(tid, i).used = false;
   BLK(tid, i).prev_idx = blk_idx; //New block's prev is the old block
   BLK(tid, i).next_idx = BLK(tid, blk_idx).next_idx; //New block's next is the next of the old block
   BLK(tid, BLK(tid, i).next_idx).prev_idx = i; //Old block's next's prev is the new block

   BLK(tid, blk_idx).next_idx = i; //Old block's next is the new block
   BLK(tid, blk_idx).size = req_size; //Size set

   return i;
}

void tal_blk_merge(unsigned tid, int left_idx, int right_idx)
{
   if((char *)(BLK(tid, left_idx).ptr) + BLK(tid, left_idx).size != BLK(tid, right_idx).ptr) return; //Blocks are not contiguous

   BLK(tid, left_idx).next_idx = BLK(tid, right_idx).next_idx; //Left block points to the one right to the right block
   BLK(tid, left_idx).size += BLK(tid, right_idx).size; //Unifying the blocks
   BLK(tid, right_idx).ptr = NULL; //"Freeing" the right block
   BLK(tid, BLK(tid, right_idx).next_idx).prev_idx = left_idx; //The block right to the right block points now to the left block
}

void *tal_alloc(unsigned tid, size_t size)
{
   if(size == 0) return NULL;

   /* Align the requested size */
   size_t aligned_size = align(size);
   int i = 0;

   /* Find big enough free block */
   while(i != -1)
   {
      if(BLK(tid, i).size >= aligned_size && !BLK(tid, i).used)
      {
         if(BLK(tid, i).size - aligned_size >= sizeof(size_t)) //We cant split if the remaining block would be unusable
            if(tal_blk_split(tid, i, aligned_size) == -1) return NULL; //Couldnt split block because metadata blocks are used up
         BLK(tid, i).used = true;
         return BLK(tid, i).ptr;
      }
      else
      {
         i = BLK(tid, i).next_idx;
      }
   }

   return NULL;
}

void *tal_realloc(unsigned tid, void *ptr, size_t size)
{
   if(ptr == NULL) return tal_alloc(tid, size);

   if(size == 0)
   {
      tal_free(tid, ptr);
      return NULL;
   }

   size = align(size);

   /* Find the metadata block for the given data block, if valid */
   int i = 0;
   while(i != -1 && BLK(tid, i).ptr != ptr) i = BLK(tid, i).next_idx;

   if(i == -1) return NULL;

   if(size == BLK(tid, i).size)
   {
      return ptr;
   }
   else if(size < BLK(tid, i).size)
   {
      int remainder = tal_blk_split(tid, i, size);
      if(remainder == -1) return NULL; //Could not split block because metadata are all used up
      int rNext = BLK(tid, remainder).next_idx;
      if(rNext != -1 && !BLK(tid, rNext).used)
         tal_blk_merge(tid, remainder, rNext);
      return ptr;
   }
   else
   {
      int next = BLK(tid, i).next_idx;
      int toMerge = next;
      if(next != -1 && !(BLK(tid, next).used && BLK(tid, next).size >= size - BLK(tid,i).size))
      {
         if(BLK(tid, next).size - size + BLK(tid, i).size >= sizeof(size_t)) //Only split if the block will still be usable
         {
            toMerge = tal_blk_split(tid, i, align(size - BLK(tid,i).size));
            if(toMerge == -1) return NULL;
         }

         tal_blk_merge(tid, i, toMerge);
         return ptr;
      }
      else
      {
         void *newMemory = tal_alloc(tid, size);
         if(newMemory == NULL) return NULL;
         memcpy(newMemory, ptr, BLK(tid,i).size);
         tal_free(tid, ptr);
         return newMemory;
      }
   }
}

void tal_free(unsigned tid, void *ptr)
{
   if(ptr == NULL) return;

   /* Find the metadata block for the given data block, if valid */
   int i = 0;
   while(i != -1 && BLK(tid, i).ptr != ptr) i = BLK(tid, i).next_idx;

   if(i == -1) return; //No block with this address

   if(!BLK(tid, i).used) return; //This block is unused

   BLK(tid, i).used = false; //Free the block

   /*Merge to the left and to the right, if possible */
   int j = BLK(tid, i).next_idx;
   if(j != -1 && !BLK(tid, j).used)
      tal_blk_merge(tid, i, j);

   j = BLK(tid, i).prev_idx;
   if(j != -1 && !BLK(tid, j).used)
      tal_blk_merge(tid, j, i);
}
