/**
 * @file safe_malloc.h
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

#ifndef SAFE_MALLOC_H_INCLUDED
#define SAFE_MALLOC_H_INCLUDED 1

#include <stdlib.h>

/** @brief Memory management module success identifier.  */
#define MEMMAN_SUCCESS 0

/** @brief Memory management module failure identifier.  */
#define MEMMAN_FAILURE 1

/** @brief Initial size initialized memory array.  */
#define MEMMAN_INIT_SIZ 512

/** @brief Extension memory array size during reallocation process.  */
#define MEMMAN_CHUNK_SIZ 256

typedef struct memory_management_data
{
   unsigned allocated;           ///< Size of the allocated array
   unsigned size;                ///< Current size of the array
   void **ptrs;                  ///< Array of pointers
} memman_data;

/**
 * @brief Initializes internal pointer array to the MEMMAN_INIT_SIZ value.
 *
 * @return 0 if the allocation was successful, non-0 if the allocation failed.
 */
int memman_init();

/**
 * @brief Clears all elements in the pointer array, freeing them and setting
 *        all pointers in the array to NULL.
 *
 * @return 0 void.
 */
void memman_clear();

/**
 * @brief Frees all elements in the memory management array including array
 *        itself.
 *
 * @return void.
 */
void memman_free_all();

/**
 * @brief Wrapper around calloc.  Acts literally the same for end the user but
 *        internally tracks allocated memory to the pointer array.
 *
 * @param num Number of blocks to be allocated.
 * @param size Size of a single block.
 * @return Pointer to the allocated memory if the allocation was successful,
 *         NULL if the allocation has failed.
 */
void *scalloc(size_t num, size_t size);

/**
 * @brief Wrapper around free.  Acts literally the same for end the end user
 *        but internally tracks allocated memory to the pointer array.  If the
 *        sfree() function is called, whole pointer array is searched, if the
 *        desired address exists, it gets freed in the same way as if free()
 *        was called.  If the desired address is not allocated through
 *        memory management module, nothing happens.
 *
 * @param addr Address to be freed.
 * @return void.
 */
void sfree(void *addr);

/**
 * @brief Wrapper around malloc.  Acts literally the same for end the user but
 *        internally tracks allocated memory to the pointer array.
 *
 * @param size Size of the memory to be allocated.
 * @return Pointer to the allocated memory if the allocation was successful,
 *         NULL if the allocation has failed.
 */
void * smalloc(size_t size);

/**
 * @brief Wrapper around realloc.  Acts literally the same for end the user but
 *        internally tracks allocated memory to the pointer array.
 *
 * @param addr Address of the address to be realocated.
 * @param size Size of the new block size.
 * @return Pointer to the allocated memory if the allocation was successful,
 *         NULL if the allocation has failed.
 */
void * srealloc(void *addr, size_t size);

#endif
