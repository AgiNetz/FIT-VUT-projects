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

#ifndef VDSTACK_H_INCLUDED
#define VDSTACK_H_INCLUDED

#include "ifj17_api.h"

/** @brief Vdstack not-empty flag.   */
#define VDSTACK_NOT_EMPTY 0

/** @brief Vdstack empty flag. */
#define VDSTACK_EMPTY 1

/** @brief Vdstack operation success identifier. */
#define VDSTACK_SUCCESS 0

/** @brief Vdstack operation failure identifier. */
#define VDSTACK_FAILURE 1

/**
 * @brief Determines if the stack is empty.  Stack is considered empty only if
 *        input variable "vds" is NULL.
 *
 * @param[in] vds Vdstack data type.
 * @return Non-zero if the stack is empty, zero otherwise.
 */
int vdstack_empty(vdstack *vds);

/**
 * @brief Clears stack contents - deallocating all elements and setting "tst"
 *        pointer to NULL, making the stack empty and ready for next push.
 *
 * @param[in] vds Vdstack data type.
 * @return void.
 */
void vdstack_clean(vdstack *vds);

/**
 * @brief Initializes stack, setting it to NULL.
 *
 * @param[in] vds Vdstack data type.
 * @return void.
 */
void vdstack_init(vdstack *vds);

/**
 * @brief Deletes element on the top of the stack.  If the stack is empty,
 *        nothing happens.  Allocated space for the element is freed.
 *
 * @param[in] vds Vdstack data type.
 * @return void.
 */
void vdstack_pop(vdstack *vds);

/**
 * @brief Adds desired variable info structure to the top of the stack.
 *
 * @param[in] vds Vdstack data type.
 * @param[in] *info Pointer variable info structure to be added.
 * @return Zero if the info structure was successfully pushed. Non-zero if
 *         an error (during memory allocation) has occurred.
 */
int vdstack_push(vdstack *vds, struct var_info *info);

/**
 * @brief Returns pointer to the variable info structure on the top of the
 *        stack.  If the stack is empty, NULL is returned.
 *
 * @param[in] vds Vdstack data type.
 * @return Pointer to the struct var_info of the topmost stack element or NULL
 *         if the stack is empty.
 */
struct var_info * vdstack_top(vdstack *vds);

/**
 * @brief Returns pointer to the variable info structure on the top of the
 *        stack and deletes pointer from the stack.  In this case, returned
 *        element is not freed by the function and deallocation needs to be
 *        handled by the caller.
 *
 * @param[in] vds Vdstack data type.
 * @return Pointer to the struct var_info of the topmost stack element or NULL
 *         if the stack is empty.
 */
struct var_info * vdstack_toppop(vdstack *vds);

#endif // VDSTACK_H_INCLUDED