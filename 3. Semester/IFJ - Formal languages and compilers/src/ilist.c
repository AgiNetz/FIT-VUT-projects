/**
 * @file ilist_t.c
 * @brief Doubly-linked list for IFJ Project 2017 AT vut.fit.vutbr.cz.
 * @date 21.11.2017
 * @author Patrik Goldschmidt - xgolds00@stud.fit.vutbr.cz
 *
 * Module provides doubly linked list interface for used for storing
 * instructions prior to their generation and optimization.
 */

#include "ilist.h"
#include "safe_malloc.h"
#include <stdlib.h>
#include <string.h>

int ilist_active(ilist_t *list)
{
   if(list->activ != NULL)
      return ILIST_ACTIVITY_ON;
   else
      return ILIST_ACTIVITY_OFF;
}

code_t * ilist_copy(ilist_t *list)
{
   /* Check if the list is active.  */
   if(list->activ == NULL)
      return NULL;

   return &list->activ->data;
}

code_t * ilist_copy_first(ilist_t *list)
{
   if(list->first == NULL)
      return NULL;

   return &list->first->data;
}


code_t *ilist_copy_last(ilist_t *list)
{
   if(list->last == NULL)
      return NULL;

   return &list->last->data;
}

void ilist_delete_first(ilist_t *list)
{
   /* Check if the list is not empty.  */
   if(list->first != NULL)
   {
      /* Back up pointer to the next element.  */
      ilist_elem_t *new_first = list->first->rneigh;

      /* If active element is about to be deleted, set list to inactive.  */
      if(list->activ == list->first)
         list->activ = NULL;

      /* Deallocate 1st element and replace it with next.   */
      sfree(list->first);
      list->first = new_first;

      /*
       * If the current 1st element is not NULL (list contained more than
       * 1 element), set its left pointer to NULL;
       */
      if(list->first != NULL)
         list->first->lneigh = NULL;
      /* If the deleted element was the only one, set last to NULL too.   */
      else
         list->last = NULL;

      list->size--;
   }

   return;
}

void ilist_delete_last(ilist_t *list)
{
   /* Check if the list is not empty.  */
   if(list->last != NULL)
   {
      /* Back up pointer to the previous element.  */
      ilist_elem_t *new_last = list->last->lneigh;

      /* If active element is about to be deleted, set list to inactive.  */
      if(list->activ == list->last)
         list->activ = NULL;

      /* Deallocate last element and replace it with next.   */
      sfree(list->last);
      list->last = new_last;

      /*
       * If the current last element is not NULL (list contained more than
       * 1 element), set its right pointer to NULL;
       */
      if(list->last != NULL)
         list->last->rneigh = NULL;
      /* If the deleted element was the only one, set first to NULL too.   */
      else
         list->first = NULL;

      list->size--;
   }

   return;  
}

void ilist_edit(ilist_t *list, code_t *instr)
{
   if(list->activ != NULL)
      memcpy(&list->activ->data, instr, sizeof(code_t));

   return;
}

int ilist_empty(ilist_t *list)
{
   if(list->size == 0)
      return ILIST_EMPTY;
   else
      return ILIST_NOT_EMPTY;
}

void ilist_first(ilist_t *list)
{
   list->activ = list->first;

   return;
}

void ilist_free(ilist_t *list)
{
   /* Temporary next list element holder. */
   ilist_elem_t *next_elem;

   /* Iterate until end of the list is reached. */
   while(list->first != NULL)
   {
      next_elem = list->first->rneigh;
      sfree(list->first);
      list->first = next_elem;
   }

   /* Set other list pointers to NULL and size to 0 as well.  */
   list->activ = NULL;
   list->last  = NULL;
   list->size  = 0;

   return;   
}

void ilist_init(ilist_t *list)
{
   list->first = NULL;
   list->last  = NULL;
   list->activ = NULL;
   list->size  = 0;

   return;
}

int ilist_insert_first(ilist_t *list, code_t *instr)
{
   /* Allocate memory for new element + error handling.   */
   ilist_elem_t *new_elem = smalloc(sizeof(ilist_elem_t));

   if(new_elem == NULL)
      return ILIST_INSERT_FAILURE;

   /* Set up the element data value and pointer addresses.   */
   memcpy(&new_elem->data, instr, sizeof(code_t));
   new_elem->lneigh = NULL;
   new_elem->rneigh = list->first;

   /* If the list is not empty.   */
   if(list->first != NULL)
      list->first->lneigh = new_elem;
   /* If the list is empty.  */
   else
      list->last = new_elem;

   /* Replace first element with the created one.  */
   list->first = new_elem;

   list->size++;

   return ILIST_INSERT_SUCCESS;
}

int ilist_insert_last(ilist_t *list, code_t *instr)
{
   /* Allocate memory for new element + error handling.   */
   ilist_elem_t *new_elem = smalloc(sizeof(ilist_elem_t));

   if(new_elem == NULL)
      return ILIST_INSERT_FAILURE;

   /* Set up the element data value and pointer addresses.   */
   memcpy(&new_elem->data, instr, sizeof(code_t));
   new_elem->lneigh = list->last;
   new_elem->rneigh = NULL;

   /* If the list is not empty.   */
   if(list->last != NULL)
      list->last->rneigh = new_elem;
   /* If the list is empty.  */
   else
      list->first = new_elem;

   /* Replace first element with the created one.  */
   list->last = new_elem;

   list->size++;

   return ILIST_INSERT_SUCCESS;
}

void ilist_last(ilist_t *list)
{
   list->activ = list->last;

   return;
}

ilist_t * ilist_merge(ilist_t *list1, ilist_t *list2)
{
   /* Check if the both lists arent the same.   */
   if(list1 == list2)
      return list1;

   /* Check the status of both lists before merging.  */
   if(list1->last != NULL && list2->first != NULL)
   {
      /******** Lists are not empty ********/
      /* Link last element of 1st to first element of 2nd.  */
      list1->last->rneigh = list2->first;
      list2->first->lneigh = list1->last;

      /* Set last element of 1st to last element of 2nd. */
      list1->last = list2->last;

      /* Add 2 list sizes. */
      list1->size += list2->size;

      /* Set list2 to initial state.  */
      ilist_init(list2);
   }
   else if(list1->last == NULL && list2->first != NULL)
   {
      /******** Second list is not empty, but first is. ********/
      /* Merge 2nd to 1st while 1st is empty, just swap pointers.   */
      ilist_t *tmplist = list1;
      list1 = list2;
      list2 = tmplist;
   }

   return list1;
}

void ilist_next(ilist_t *list)
{
   if(list->activ != NULL)
      list->activ = list->activ->rneigh;

   return;
}

void ilist_post_delete(ilist_t *list)
{
   /* Check if the list is active and the active element is not last.   */
   if(list->activ != NULL && list->activ->rneigh != NULL)
   {
      /* Back up pointer to the deleted element.   */
      ilist_elem_t *elem_to_del = list->activ->rneigh;
      /* Set pointer to next element after the deletion process. */
      list->activ->rneigh = elem_to_del->rneigh;

      /* If the deleted element is not last, modify links with the active.  */
      if(elem_to_del != list->last)
         list->activ->rneigh->lneigh = list->activ;
      /* If the deleted element is the last, link last to active.  */
      else
         list->last = list->activ;

      sfree(elem_to_del);

      list->size--;
   }

   return;
}

int ilist_post_insert(ilist_t *list, code_t *instr)
{
   /* Check if the list is active.  */
   if(list->activ != NULL)
   {
      /* Allocate memory for new element + error handling.  */
      ilist_elem_t *new_elem = smalloc(sizeof(ilist_elem_t));

      if(new_elem == NULL)
         return ILIST_INSERT_FAILURE;

      /* Fill up new element data and create links. */
      memcpy(&new_elem->data, instr, sizeof(code_t));
      new_elem->lneigh = list->activ;
      new_elem->rneigh = list->activ->rneigh;

      /* If the active element was last, change it to new element.   */
      if(list->activ == list->last)
         list->last = new_elem;
      /*
       * If the active element was not last, link the next element with the
       * one being added.
       */
      else
         new_elem->rneigh->lneigh = new_elem;

      /* Finally - link the new element to the active one.  */
      list->activ->rneigh = new_elem;

      list->size++;
   }

   return ILIST_INSERT_SUCCESS;
}

void ilist_pre_delete(ilist_t *list)
{
   /* Check if the list is active and the active element is not first.   */
   if(list->activ != NULL && list->activ->lneigh != NULL)
   {
      /* Back up pointer to the deleted element.   */
      ilist_elem_t *elem_to_del = list->activ->lneigh;
      /* Set pointer to next element after the deletion process. */
      list->activ->lneigh = elem_to_del->lneigh;

      /* If the deleted element is not first, modify links with the active.  */
      if(elem_to_del != list->first)
         list->activ->lneigh->rneigh = list->activ;
      /* If the deleted element is the first, link first to active.  */
      else
         list->first = list->activ;

      sfree(elem_to_del);

      list->size--;
   }

   return;
}

int ilist_pre_insert(ilist_t *list, code_t *instr)
{
   /* Check if the list is active.  */
   if(list->activ != NULL)
   {
      /* Allocate memory for new element + error handling.  */
      ilist_elem_t *new_elem = smalloc(sizeof(ilist_elem_t));

      if(new_elem == NULL)
         return ILIST_INSERT_FAILURE;

      /* Fill up new element data and create links. */
      memcpy(&new_elem->data, instr, sizeof(code_t));
      new_elem->lneigh = list->activ->lneigh;
      new_elem->rneigh = list->activ;

      /* If the active element was first, change it to new element.   */
      if(list->activ == list->first)
         list->first = new_elem;
      /*
       * If the active element was not first, link the previous element with
       * the one being added.
       */
      else
         new_elem->lneigh->rneigh = new_elem;

      /* Finally - link the new element to the active one.  */
      list->activ->lneigh = new_elem;

      list->size++;
   }

   return ILIST_INSERT_SUCCESS;
}

void ilist_prev(ilist_t *list)
{
   if(list->activ != NULL)
      list->activ = list->activ->lneigh;

   return;
}

int ilist_size(ilist_t *list)
{
   return list->size;
}