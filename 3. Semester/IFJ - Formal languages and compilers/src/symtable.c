/**
 *  @file symtable.c
 *  @brief Symbol table source file for IFJ Project 2017 AT vut.fit.vutbr.cz.
 *  @date 10.10.2017, last rev. 6.12.12017
 *  @author Patrik Goldschmidt - xgolds00@stud.fit.vutbr.cz
 *
 *  Module provides library functions for symbol table implemented as a hash
 *  table.
 */

#include "safe_malloc.h"
#include "symtable.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

unsigned int hash_fnv32(const char *key)
{
    unsigned char *s = (unsigned char *)key;    // Cast to unsigned char
    unsigned int hash = FNV_32_OFFSET;          // Initial hash value

    /*  XOR with octet and multiply with FNV prime for the whole string.    */
    while(*s)
    {
        hash ^= *s++;
        hash *= FNV_32_PRIME;
    }

    return hash;
}

void stab_clear(symtab_t *tab)
{
   stab_item_t *item_tbd;            // Item to be deleted.
   stab_item_t *item_next;           // Next item in the list.

   /* Iterate through whole table and each linked list.  */
   for(int i = 0; i < SYMTAB_SIZE; i++)
   {
      /* If particular field is not empty already, clean it.   */
      if((*tab)[i] != NULL)
      {
         /* Load the first item in the list into the "next" pointer. */
         item_next = (*tab)[i];

         /* If next item exists, clean it.  */
         while(item_next != NULL)
         {
            item_tbd = item_next;
            item_next = item_tbd->item_next;
            /*  sfree the string data.   */
            sfree(item_tbd->data.id);
            item_tbd->data.id = NULL;
            sfree(item_tbd);
         }

         /* Set list root pointer to NULL after everything is sfreed. */
         (*tab)[i] = NULL;
      }
   }

   return;
}

void stab_delete_symbol(symtab_t *tab, char *id)
{
   int index = hash_fnv32(id) % SYMTAB_SIZE;    // Index of the searched item
   stab_item_t *item_tbd  = (*tab)[index];      // Item to be deleted
   stab_item_t *item_prev = NULL;               // Item before the deleted one

   /* Browse through the linked list and search for the key match. */
   while(item_tbd != NULL && strcmp(item_tbd->data.id, id))
   {
      item_prev = item_tbd;
      item_tbd  = item_tbd->item_next;
   }

   /* If the item was found - loop did not end on item_tbd == NULL.  */
   if(item_tbd != NULL)
   {
      /* If deleted item is 1st in the list - straightly change root ptr.  */
      if(item_prev == NULL)
         (*tab)[index] = item_tbd->item_next;
      /* If deleted value is deeper in the list - change previous item link. */
      else
         item_prev->item_next = item_tbd->item_next;

      sfree(item_tbd->data.id);
      item_tbd->data.id = NULL;
      sfree(item_tbd);
   }

   return;
}

symbol_t * stab_get_data(symtab_t *tab, char *id)
{
   /* Save element and return based on whetver it is NULL or not.  */
   stab_item_t *item = stab_search(tab, id);

   return (item) ? &(item->data) : NULL;
}

void stab_init(symtab_t *tab)
{
    memset(*tab, 0, SYMTAB_SIZE * sizeof(stab_item_t *));

    return;
}

int stab_insert(symtab_t *tab, symbol_t *symbol)
{
    /*  Check if the symbol in the table already exists. */
    stab_item_t *item = stab_search(tab, symbol->id);

    if(item != NULL)
    {
        /*  If the symbol in the table exists, update it.    */
        /*  Back up old reference to the ID.    */
        char *id_tmp = item->data.id;

        /*  Copy the contents of the input to update the one in the table.   */
        memcpy(&item->data, symbol, sizeof(symbol_t));
        item->data.id = id_tmp;
    }
    else
    {
        /*  If the symbol in the table does not exist, create one + check.  */
        unsigned int index = hash_fnv32(symbol->id) % SYMTAB_SIZE;
        /*  Create copy of the symbol identifier.   */
        char *sym_id = smalloc(sizeof(char) * (strlen(symbol->id) + 1));

        if(sym_id == NULL)
            return STAB_INSERT_FAIL;

        item = smalloc(sizeof(stab_item_t));

        if(item == NULL)
            return STAB_INSERT_FAIL;

        strcpy(sym_id, symbol->id);

        /*  Update new item contents + insert item on the start of the list. */
        memcpy(&item->data, symbol, sizeof(symbol_t));
        item->data.id = sym_id;
        item->item_next = (*tab)[index];

        (*tab)[index] = item;
    }

    return STAB_INSERT_OK;
}

symbol_t * stab_iterate_next(symtab_t *tab, stab_iterator_t *it)
{
   /* Do not proceed to the next item if pointer is already NULL. */
   if(it->item == NULL)
      return NULL;

   /* Otherwise, try to find next item in the current row.  */
   if(it->item->item_next != NULL)
   {
      /* Next item in the current row exist. */
      it->item = it->item->item_next;

      return &it->item->data;
   }
   else
   {
      /* There is no other item in the current row - search other rows. */
      /* Preemptively set item to NULL to facilitate unsuccessful search.  */
      it->item = NULL;
      
      for(; it->row < SYMTAB_SIZE && it->item == NULL; it->row++)
      {
         /* If item in the row is found -> assign it to the iterator.   */
         if((*tab)[it->row] != NULL)
            it->item = (*tab)[it->row];
      }

      return &it->item->data;
   }
}

int stab_iterate_set(symtab_t *tab, stab_iterator_t *it)
{
   /* Set the initial iterator values. */
   it->item = NULL;
   it->row = 0;

   /* Search for the first element. */
   for(; it->row < SYMTAB_SIZE && it->item == NULL; it->row++)
   {
      /* If item in the row is found -> assign it to the iterator.   */
      if((*tab)[it->row] != NULL)
         it->item = (*tab)[it->row];
   }

   /* Check if the symtable is not empty and return based on it. */
   if(it->item != NULL)
      return SYMTAB_ITERATOR_SET;
   else
      return SYMTAB_ITERATOR_NOT_SET;
}

stab_item_t * stab_search(symtab_t *tab, char *id)
{
   /* Calculate index for the searched symbol and assign value from array.   */
   stab_item_t *item = (*tab)[hash_fnv32(id) % SYMTAB_SIZE];

   /* Browse through the linked list and search for the match. */
   for(; item != NULL; item = item->item_next)
   {
      /* If the id is matched (strcmp() == 0), break immediately.   */
      if(!strcmp(item->data.id, id))
         break;
   }

   return item;
}
