/**
 * @file vdstack.c
 * @brief Variable depth stack IFJ Project 2017 AT vut.fit.vutbr.cz.
 * @date 27.11.2017
 * @author Goldschmidt Patrik - xgolds00@stud.fit.vutbr.cz
 *
 * Variable Depth Stack (vdstack) module supports numerous count of scopes
 * throughout the program interpretation.  This functionality is required by
 * SCOPE extension which our interpreter ought to support.  Realization is
 * done via classic linked list, where elements points to their predecessors.
 */

#include "safe_malloc.h"
#include "vdstack.h"
#include <stdlib.h>
#include <string.h>

int vdstack_empty(vdstack *vds)
{
   if(*vds == NULL)
      return VDSTACK_EMPTY;
   else
      return VDSTACK_NOT_EMPTY;
}

void vdstack_clean(vdstack *vds)
{
   vdstack_item *item_tbd = *vds;          // Item to be deleted

   while(*vds != NULL)
   {
      *vds = (*vds)->prev;
      sfree(item_tbd);
      item_tbd = *vds;
   }

   return;
}

void vdstack_init(vdstack *vds)
{
   *vds = NULL;

   return;
}

void vdstack_pop(vdstack *vds)
{
   vdstack_item *item_tbd = *vds;          // Item to be deleted
   *vds = (*vds)->prev;

   sfree(item_tbd);

   return;
}

int vdstack_push(vdstack *vds, struct var_info *info)
{
   /* Allocate new vdstack item + check.  */
   vdstack_item *item_new = smalloc(sizeof(vdstack_item));

   if(item_new == NULL)
      return VDSTACK_FAILURE;

   /* Copy contents of input structure to the vdstack item data.  */
   memcpy(&item_new->data, info, sizeof(struct var_info));

   /* Set up new item predecessor and update vds pointer.   */
   item_new->prev = *vds;
   *vds = item_new;

   return VDSTACK_SUCCESS;
}

struct var_info * vdstack_top(vdstack *vds)
{
   return &(*vds)->data;
}

struct var_info * vdstack_toppop(vdstack *vds)
{
   vdstack_item *item_ret = *vds;

   *vds = (*vds)->prev;

   return &item_ret->data;
}
