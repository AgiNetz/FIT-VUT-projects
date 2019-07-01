/**
 * @file ilist_t.h
 * @brief Doubly-linked list for IFJ Project 2017 AT vut.fit.vutbr.cz.
 * @date 21.11.2017
 * @author Patrik Goldschmidt - xgolds00@stud.fit.vutbr.cz
 *
 * Module provides doubly linked list interface for used for storing
 * instructions prior to their generation and optimization.
 */

#ifndef ILIST_H_INCLUDED
#define ILIST_H_INCLUDED

#include "ifj17_api.h"

/** @brief Defines instruction list insertion success output code.   */
#define ILIST_INSERT_SUCCESS 0

/** @brief Defines instruction list insertion failure output code.   */
#define ILIST_INSERT_FAILURE 1

/** @brief Inactive ilist_t flag.   */
#define ILIST_ACTIVITY_OFF 0

/** @brief Active ilist_t flag.  */
#define ILIST_ACTIVITY_ON 1

/** @brief Instruction list not empty flag.  */
#define ILIST_NOT_EMPTY 0

/** @brief Instruction list empty flag.   */
#define ILIST_EMPTY 1

/**
 * @brief Finds out if the list is active.
 *
 * @param *list Pointer to the list data type.
 * @return Non-zero if the list is active, zero otherwise.
 */
int ilist_active(ilist_t *list);

/**
 * @brief Returns contents of the active element of the list.
 *
 * @param *list Pointer to the list data type.
 * @return Pointer to code_t data structure or NULL, if the list is empty or
 *         not active.
 */
code_t * ilist_copy(ilist_t *list);

/**
 * @brief Returns contents of the first element of the list.
 *
 * @param *list Pointer to the list data type.
 * @return Pointer to code_t data structure or NULL, if the list is empty.
 */
code_t * ilist_copy_first(ilist_t *list);

/**
 * @brief Returns contents of the last element of the list.
 *
 * @param *list Pointer to the list data type.
 * @return Pointer to code_t data structure or NULL, if the list is empty.
 */
code_t *ilist_copy_last(ilist_t *list);

/**
 * @brief Deletes first item of the list. If the list is empty, does nothing.
 *
 * @param *list Pointer to the list data type.
 * @return void.
 */
void ilist_delete_first(ilist_t *list);

/**
 * @brief Deletes last item of the list. If the list is empty, does nothing.
 *
 * @param *list Pointer to the list data type.
 * @return void.
 */
void ilist_delete_last(ilist_t *list);

/**
 * @brief Edits value of the current active element of the list.  If the list
 *        is not active, does nothing.
 *
 * @param *list   Pointer to the list data type.
 * @param *instr  Pointer to the instruction which contets will be used.
 * @return void.
 */
void ilist_edit(ilist_t *list, code_t *instr);

/**
 * @brief Finds out if instruction list is empty.
 *
 * @param *list   Pointer to the list data type.
 * @return Non-0 if list is not empty, 0 otherwise.
 */
int ilist_empty(ilist_t *list);

/**
 * @brief Sets list activity to the first element of the list.  If the list is
 *        empty, does nothing.
 *
 * @param *list Pointer to the list data type.
 * @return void.
 */
void ilist_first(ilist_t *list);

/**
 * @brief Frees all data in the list and sets control pointers to NULL.
 *
 * @param *list Pointer to the list data type.
 * @return void.
 */
void ilist_free(ilist_t *list);

/**
 * @brief Initializes doubly-linked list, setting all control pointers to NULL.
 *
 * @param *list Pointer to the list data type.
 * @return void.
 */
void ilist_init(ilist_t *list);

/**
 * @brief Inserts element to the start of the list.
 *
 * @param *list   Pointer to the list data type.
 * @param *instr  Pointer to the code_t data type to be inserted.
 * @return 0 if insertion was OK, non-0 if an error ocurred and insertion was
 *           not made (e.g memory allocation error)
 */
int ilist_insert_first(ilist_t *list, code_t *instr);

/**
 * @brief Inserts element to the end of the list.
 *
 * @param *list   Pointer to the list data type.
 * @param *instr  Pointer to the code_t data type to be inserted.
 * @return 0 if insertion was OK, non-0 if an error ocurred and insertion was
 *           not made (e.g memory allocation error)
 */
int ilist_insert_last(ilist_t *list, code_t *instr);

/**
 * @brief Sets list activity to the last element of the list.
 *
 * @param *list Pointer to the list data type.
 * @return void.
 */
void ilist_last(ilist_t *list);

/**
 * @brief Merges 2 lists into single one, setting control variables of 2nd to
 *        initial state.  Keeps original active element of list 1.
 *
 * @param *list1 Pointer to the list, which will contain merged contents.
 * @param *list2 Pointer to the list, which will be merged to list1.
 * @return Pointer to the list containing both merged lists.  In fact, this is
 *         same pointer as the list1, which came as input parameter.
 */
ilist_t * ilist_merge(ilist_t *list1, ilist_t *list2);

/**
 * @brief Sets list activity to the next element of the list.
 *
 * @param *list Pointer to the list data type.
 * @return void.
 */
void ilist_next(ilist_t *list);

/**
 * @brief Deletes element after the current active element.  If the list is not
 *        active, or active element is the last, does nothing.
 *
 * @param *list Pointer to the list data type.
 * @return void.
 */
void ilist_post_delete(ilist_t *list);

/**
 * @brief Inserts element after the current active element of the list.  If the
 *        list has no active element, does nothing.
 *
 * @param *list   Pointer to the list data type.
 * @param *instr  Pointer to the code_t data type to be inserted.
 * @return 0 if insertion was OK, non-0 if an error ocurred and insertion was
 *           not made (e.g memory allocation error)
 */
int ilist_post_insert(ilist_t *list, code_t *instr);

/**
 * @brief Deletes element before the current active element.  If the list is
 *        not active, or active element is the first, nothing happens.
 *
 * @param *list Pointer to the list data type.
 * @return void.
 */
void ilist_pre_delete(ilist_t *list);

/**
 * @brief Inserts element before the current active element.  If the list is
 *        not active nothing happens.
 *
 * @param *list   Pointer to the list data type.
 * @param *instr  Pointer to the code_t data type to be inserted.
 * @return 0 if insertion was OK, non-0 if an error ocurred and insertion was
 *           not made (e.g memory allocation error)
 */
int ilist_pre_insert(ilist_t *list, code_t *instr);

/**
 * @brief Sets list activity to the previous element of the list.
 *
 * @param *list Pointer to the list data type.
 * @return void.
 */
void ilist_prev(ilist_t *list);

/**
 * @brief Returns number of elements contained in the list.
 *
 * @param *list Pointer to the list data type.
 * @return Number of elements in the list.
 */
int ilist_size(ilist_t *list);

#endif // IFJ17_API_H_INCLUDED