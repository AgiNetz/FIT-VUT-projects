/**
 *  @file str.h
 *  @brief String header file for IFJ Project 2017 AT vut.fit.vutbr.cz.
 *  @date 28.10.2017, last rev. 6.12.2017
 *  @author Patrik Goldschmidt - xgolds00@stud.fit.vutbr.cz
 *
 *  Module provides library functions for manipulation with dynamic-sized
 *  strings in the C language.
 */

#ifndef STR_H_INCLUDED
#define STR_H_INCLUDED

#include "ifj17_api.h"
#include <stdbool.h>

/**
 *  @brief Defines output codes for various string functions.
 */
enum str_out_codes
{
   STR_SUCCESS,            ///< String operation has succeeded.
   STR_FAILURE,            ///< String operation has failed.
};

/**
 *  @brief Chunk of memory to be allocated/added during string functions
 *         execution in bytes.
 */
#define STR_MEM_CHUNK 128

/**
 *  @brief Adds character 'c' into the string_t data structure.  Treats special
 *         characters differently and may cause data reallocation.
 *
 *         If allocated space for the string is not big enough to hold current
 *         string + 1 character being added, string data are reallocated to new
 *         value.  Size of the new data will be current data + STR_MEM_CHUNK.
 *         If an added character is IFJ17 special character '\' or '#',
 *         whitespace character or non-printable character, it is automatically
 *         converted into form of "\xyz", where "xyz" is a decimal sequence of
 *         numbers representing particular character ASCII value.
 *
 *  @param *str Pointer to the string_t data structure.
 *  @param c Character to be added to the current string.
 *  @return 0 if character was successfully added to the string.
 *          Not 0 if character could not be added (memory allocation has
         failed.)
 */
int str_add_char(string_t *str, char c);

/**
 * @brief Adds backslash character as an escape sequence in the form of \xyz.
 * 
 * @param *str Pointer to the string_t data structure.
 * @return 0 if character was successfully added to the string.
 *         Not 0 if character could not be added (memory allocation has
 *         failed.)
 */
int str_add_bslash(string_t *str);

/**
 * @brief Appends C-type string to the string_t data structure.  If the
 *        string_t is not big enough, reallocation occurs.
 *
 * @param *str Pointer to the string_t data structure.
 * @param *cstr Pointer to the C-type string to be appended.
 * @return 0 if character was successfully added to the string.
 *         Not 0 if character could not be added (memory allocation has
 *         ailed.)
 */
int str_append_cstring(string_t *str, const char *cstr);

/**
 *  @brief Clears data from string_t data type.  Size of the allocated memory
 *         remains unchanged.
 *
 *  @param *str Pointer to the string_t data structure.
 *  @return void
 */
void str_clear(string_t *str);

/**
 *  @brief Compares string_t data type with C string type.
 *
 *  @param *str1 Pointer to the string_t data structure.
 *  @param *str2 Pointer to an array of chars.
 *  @return Same as strcmp() standard C function.
 *          -1 if str1 < str2
 *          0 if str1 is equal to str2
 *          1 if str1 > str2
 */
int str_cmpsc(const string_t *str_t, const char *str);

/**
 *  @brief Compares two string_t data types.
 *
 *  @param *str1 Pointer to the string_t data structure.
 *  @param *str2 Pointer to the string_t data structure.
 *  @return Same as strcmp() standard C function.
 *          -1 if str1 < str2
 *          0 if str1 is equal to str2
 *          1 if str1 > str2
 */
int str_cmpss(const string_t *str1, const string_t *str2);

/**
 *  @brief Allocates new C-type string with exact size of the current string
 *         in the string_t data structure + terminating NULL character and
 *         copies contents from the structure to the newly allocated memory.
 *
 *  @param *str Pointer to the string_t data structure.
 *  @return Pointer to newly allocated C string or NULL if the allocation has
 *          failed.
 */
char * str_copy_to_cstring(string_t *str);

/**
 *  @brief Duplicates content of string_t data structures.  If the destination
 *         is not big enough to hold source string, reallocation occurs.
 *         Data content is then copied from source to destination, also
 *         updating "len" variable.  New string_t cannot be initialized
 *         with this function, since it is pointing to undefined value.
 *         For new strings, use str_init_string for making a copy.
 *
 *  @param *str_dst Pointer to the destination string_t data structure.
 *  @param *str_src Pointer to the source string_t data structure.
 *  @return 0 if operation was successful, non-0 if error during reallocation
 *          occurred.
 */
int str_copy_to_string(string_t *str_dst, string_t *str_src);

/**
 *  @brief Frees allocated string data and sets control variables to 0.
 *
 *  @param *str Pointer to the string_t data structure.
 *  @return void.
 */
void str_free(string_t *str);

/**
 *  @brief Returns string_t data pointer.
 *
 *  @param *str Pointer to the string_t data structure.
 *  @return Pointer to string_t data.
 */
char * str_get_data(const string_t *str);

/**
 *  @brief Returns string_t data length without terminating NULL character.
 *
 *  @param *str Pointer to the string_t data structure.
 *  @param c Character to be added to the string.
 *  @return Data length without terminating NULL character.
 */
int str_get_len(const string_t *str);

/**
 *  @brief Returns allocated space for the string.
 *
 *  @param *str Pointer to the string_t data structure.
 *  @return Allocated space for the string_t data in bytes.
 */
int str_get_size(const string_t *str);

/**
 *  @brief Initializes string_t data structure.  Initial size of the new data
 *         is STR_MEM_CHUNK value. Auxiliary variables "len" and "allocated"
 *         are also set accordingly.  If memory allocation process fails,
 *         function exits with particular error code and structure contents
 *         remain unchanged.
 *
 *  @param *str Pointer to the string_t data structure.
 *  @return 0 if operation was successful, not 0 if memory allocation error
 *          occurred.
 */
int str_init(string_t *str);

/**
 *  @brief Initializes string_t data structure with particular C-type string.
 *         Function allocates multiply of STR_MEM_CHUNK to properly fit the
 *         C-string inside. Auxiliary variables "len" and "allocated" are set
 *         accordingly.  If memory allocation process fails, function exits
 *         with particular error code and structure contents remain unchanged.
 *
 *  @param *str Pointer to the string_t data structure.
 *  @param *cstr C-type string to be allocated.
 *  @return 0 if operation was successful, non-0 if memory allocation error
 *          occurred.
 */
int str_init_cstring(string_t *str, const char *cstr);

/**
 *  @brief Initialized newly created string with value from another string.
 *
 *  @param *str_dst Pointer to the string_t data structure to be initialized.
 *  @param *str_src Pointer
 *  @return 0 if operation was successful, non-0 if memory allocation error
 *          occurred.
 */
int str_init_string(string_t *str_dst, string_t *str_src);

/**
 *  @brief Copies contents from C-type string to string_t data structure.  If
 *         string_t is not big enough to hold whole source string, reallocation
 *         occurs.  If memory allocation process fails, function exits with
 *         particular error code and structure contents remain unchanged.
 *
 *  @param *str_dst Pointer to destination string_t data structure.
 *  @param *cstr_src Pointer to source C-type string.
 *  @return 0 if operation was successful, non-0 if memory allocation error
 *          occurred.
 */
int str_set_cstring(string_t *str_dst, const char *cstr_src);

#endif // STR_H_INCLUDED
