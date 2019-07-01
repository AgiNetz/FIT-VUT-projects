/**
 *  @file symtable.h
 *  @brief Symbol table source file for IFJ Project 2017 AT vut.fit.vutbr.cz.
 *  @date 10.10.2017, last rev. 6.12.2017
 *  @author Patrik Goldschmidt - xgolds00@stud.fit.vutbr.cz
 *
 *  Module provides library functions for symbol table implemented as a hash
 *  table.
 */

#ifndef SYMTABLE_H_INCLUDED
#define SYMTABLE_H_INCLUDED

#include "ifj17_api.h"

/**
 *  @brief FNV hash algorithm offset for 32-bit implementation.
 *
 *  Based on: http://www.isthe.com/chongo/tech/comp/fnv/
 */
#define FNV_32_OFFSET 2166136261U

/**
 *  @brief FNV hash algorithm prime for 32-bit implementation.
 *
 *  Based on: http://www.isthe.com/chongo/tech/comp/fnv/
 */
#define FNV_32_PRIME 16777619

/** @brief Flag for successful iterator set. */
#define SYMTAB_ITERATOR_SET 0

/** 
 * @brief Flag for unsuccessful iterator set, which points to NULL because
 *        destination table was empty.
 */
#define SYMTAB_ITERATOR_NOT_SET 1

/**
 *  @brief Defines symbol table size.
 *
 *  Symbol table size was chosen according to the hash algorithm performance
 *  and considering the actual symbol table and interpretation process needs.
 */
#define SYMTAB_SIZE 300

/** @brief Defines symtab_insert() function outcodes.   */
enum stab_insert_outcodes
{
   STAB_INSERT_OK,                  ///< Symbol table insert success
   STAB_INSERT_FAIL,                ///< Symbol table insert failure
};

/*
 *  @brief Symbol table item structure.  Identifier of the symbol serves as a
 *         key to compute hashes.
 */
typedef struct symb_table_item
{
   symbol_t data;                      ///< Data containing symbol_t
   struct symb_table_item *item_next;  ///< Pointer to the next table item
} stab_item_t;

typedef struct symb_table_iterator_handle
{
   stab_item_t *item;                  ///< Pointer to the item to be returned
   int row;                            ///< Current processed row
} stab_iterator_t;

/**
 *  @brief Symbol table definition as a array of symbols.
 */
typedef stab_item_t *symtab_t[SYMTAB_SIZE];

/**
 *  @brief Calculates hash function based on FNV 32-bit algorithm.
 *
 *  Function implemented using pseudocode on:
 *      http://www.isthe.com/chongo/tech/comp/fnv/
 *
 *  @param[in] *key Key to calculate hash value from. Requires valid C-string.
 *  @return Calculated hash value.
 */
unsigned int hash_fnv32(const char *key);

/**
 *  @brief Clears the whole symbol table to its initial state correctly freeing
 *         all its elements and setting stab_item_t pointers to NULL.
 *
 *  @param[in] *tab Pointer to the table of symbols.
 *  @return void.
 */
void stab_clear(symtab_t *tab);

/**
 *  @brief Deletes the symbol with particular identifier from the table. If the
 *         symbol with desired ID does not exist, table stays intact.
 *
 *  @param *tab Pointer to the table of symbols.
 *  @param *id Symbol identifier to be deleted.
 *  @return void.
 */
void stab_delete_symbol(symtab_t *tab, char *id);

/**
 *  @brief Returns pointer to symbol_t structure according to identifier
 *         parameter.
 *
 *  @param[in] *tab Pointer to the table of symbols.
 *  @param[in] *id Identifier of the symbol to be returned.
 *  @return Pointer to the symbol_t data structure or NULL if the symbol with
 *          particular id does not exist.
 */
symbol_t * stab_get_data(symtab_t *tab, char *id);

/**
 *  @brief Initializes symbol table (Sets whole array to NULL).
 *
 *  @param[in] *tab Pointer to the symbol_t data type.
 *  @return void.
 */
void stab_init(symtab_t *tab);

/**
 *  @brief Inserts symbol to the symbol table.  If the symbol already exists,
 *         its content will get updated.  Function will allocate new
 *         stab_item_t data type and new C-string according to input parameter
 *         symbol.
 *
 *  @param[in] *tab Pointer to the table of symbols.
 *  @param[in] *symbol Pointer to allocated symbol structure.
 *  @return 0 if the operation was successful.  Not 0 if the memory allocation
 *          fails and symbol is not inserted.  In this case, symbol table stays
 *          intact.
 */
int stab_insert(symtab_t *tab, symbol_t *symbol);

/**
 *  @brief Function used for symtable iteration. Call sequentially returns
 *         element after element from the symbol table, effectively iterating
 *         though it and returning NULL at the end, when all items have been
 *         already iterated.  Function will then keep to return NULL, until
 *         reinitialized at the start of the table using stab_iterate_set().
 *
 * @param[in] *tab Pointer to the table of symbols.
 * @param[in] *it  Pointer to the symbol table iterator structure.
 * @return Pointer to the symbol_t data stored in the stab_item_t pointed
 *         by the iterator or NULL, if whole table was been already iterated. 
 */
symbol_t * stab_iterate_next(symtab_t *tab, stab_iterator_t *it);


/**
 *  @brief Sets the iterator on the first found item in the hash table.
 *         Searching starts at row 0 and carries on up to SYMTAB_SIZE-1.  If
 *         the desired symtable is empty, iterator structure will point to the
 *         NULL and non-0 value will be returned.  This function can also be
 *         used to reinitialize iterator after all items in the table using
 *         stab_iterate_next() were already traversed.
 *
 * @param[in] *tab Pointer to the table of symbols.
 * @param[in] *it  Pointer to the symbol table iterator structure.
 * @return 0, if the iterator was successfully set to the 1st element in the
 *         table.  Non-0 if the table is empty and iterator target was set to 
 *         NULL.
 */
int stab_iterate_set(symtab_t *tab, stab_iterator_t *it);

/**
 *  @brief Searches for a symbol based on the input key.
 *
 *  @param[in] *tab Pointer to the table of symbols.
 *  @param[in] *id Symbol identifier to be searched for.
 *  @return Pointer to item in the symbol table or NULL if the item with
 *          particular key does no exist.
 */
stab_item_t * stab_search(symtab_t *tab, char *id);

#endif // SYMTABLE_H_INCLUDED