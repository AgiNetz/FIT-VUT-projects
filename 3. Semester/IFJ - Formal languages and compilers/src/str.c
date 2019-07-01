/**
 *  @file str.h
 *  @brief String header file for IFJ Project 2017 AT vut.fit.vutbr.cz.
 *  @date 28.10.2017, last rev. 6.12.2017
 *  @author Patrik Goldschmidt - xgolds00@stud.fit.vutbr.cz
 *
 *  Module provides library functions for manipulation with dynamic-sized
 *  strings in the C language.
 */

#include "ifj17_api.h"
#include "safe_malloc.h"
#include "str.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Internal string function.  Expands string "str" capacity by "siz" bytes.   */
int str_expand(string_t *str, int siz);

int str_add_char(string_t *str, char c)
{
   /* 
    * Default character length is 1, but processing non-printable characters,
    * white spaces orcharacter hash "#" takes form of "\xyz", which means to
    * to add 3 extra characters to the output string.
    */
   int chlen = 1;

   /* If char is type of described above - set chlen accordingly. */
   if(iscntrl(c) || isspace(c) || c == '#')
      chlen = 4;

   /*
    *  If size of the new string with added character (+1 or +4) and 
    * terminating NULL character (+1) would be greater than currently
    * allocated size, realloc data to current + STR_MEM_CHUNK size.
    */
   if(str->len + 1 + chlen > str->allocated)
   {
      int outcode = str_expand(str, STR_MEM_CHUNK);  // Expand funct outcode

      if(outcode == STR_FAILURE)
         return STR_FAILURE;
   }

   /* Add new character to the string - different based on type of char. */
   if(chlen == 1)
      str->data[(str->len)++] = c;
   else
   {
      int char_val = (int) c;       // Value of the character as an integer.
      char val_as_chars[chlen + 1]; // Decimal value expressed as chars.

      /* Convert decimal value to characters. */
      snprintf(val_as_chars, chlen + 1, "\\%.*d", chlen - 1, char_val) ;

      /* Add characters into the string.  */
      for(int i = 0; i < chlen; i++)
         str->data[(str->len)++] = val_as_chars[i];
   }

   return STR_SUCCESS;
}

int str_add_bslash(string_t *str)
{
   char val_as_chars[5];         // Decimal value expressed as chars.

   /* Check if the output string is large enough, realloc if not. */
   if(str->len + 4 > str->allocated)
   {
      int outcode = str_expand(str, STR_MEM_CHUNK);  // Expand funct outcode

      if(outcode == STR_FAILURE)
         return STR_FAILURE;
   }

   /* Convert decimal value to characters. */
   snprintf(val_as_chars, 5, "\\%.*d", 3, (int) '\\');

   /* Add characters into the string.  */
   for(int i = 0; i < 4; i++)
      str->data[(str->len)++] = val_as_chars[i];

   return STR_SUCCESS;
}

int str_append_cstring(string_t *str, const char *cstr)
{
   int cstr_len = strlen(cstr);
   /* Check if the new string after appending will not overcome alloc value. */
   if(str->len + cstr_len + 1 > str->allocated)
   {
      int outcode = str_expand(str, STR_MEM_CHUNK);  // Expand funct outcode

      if(outcode == STR_FAILURE)
         return STR_FAILURE;      
   }

   /* If the reallocation was not necessary or successful, append C string.   */
   strcat(str->data, cstr);
   str->len += cstr_len;

   return STR_SUCCESS;
}

void str_clear(string_t *str)
{
   memset(str->data, 0, str->allocated);
   str->len = 0;

   return;
}

int str_cmpsc(const string_t *str_t, const char *str)
{
   return strcmp(str_t->data, str);
}

int str_cmpss(const string_t *str1, const string_t *str2)
{
   return strcmp(str1->data, str2->data);
}

char * str_copy_to_cstring(string_t *str)
{
   /*  Allocate enough space for the new string + terminating NULL char.   */
   char * new_str = smalloc((str->len + 1) * sizeof(char));

   if(new_str == NULL)
      return NULL;

   strncpy(new_str, str->data, str->len + 1);

   return new_str;
}

int str_copy_to_string(string_t *str_dst, string_t *str_src)
{
   /*  If capacity of destination is not big enough, realloc + error check. */
   if(str_dst->allocated < str_src->allocated)
   {
      char *new_data = srealloc(str_dst->data, str_src->allocated *
                                    sizeof(char));

      if(new_data == NULL)
         return STR_FAILURE;

      /*  Assign new data to the string and set them to 0. */
      str_dst->data = new_data;
      memset(str_dst->data, 0, str_src->allocated);

      /*  Change allocated data information.  */
      str_dst->allocated = str_src->allocated;
   }

   /*  Copy data from destination to the source.   */
   strncpy(str_dst->data, str_src->data, str_src->len+1);
   str_dst->len = str_src->len;

   return STR_SUCCESS;
}

void str_free(string_t *str)
{
   sfree(str->data);
   str->data = NULL;
   str->allocated = 0;
   str->len = 0;

   return;
}

int str_expand(string_t *str, int siz)
{
   char *new_data = srealloc(str->data, (str->allocated + siz) * sizeof(char));

   /*  Check if the allocation was successful. */
   if(new_data == NULL)
      return STR_FAILURE;

   /* Set newly allocated memory to NULL.  */
   memset(new_data + str->allocated, 0, siz * sizeof(char));

   /*  Update allocated memory + control variable. */
   str->data = new_data;
   str->allocated += siz * sizeof(char);

   return STR_SUCCESS;
}

char * str_get_data(const string_t *str)
{
   return str->data;
}

int str_get_len(const string_t *str)
{
   return str->len;
}

int str_get_size(const string_t *str)
{
   return str->allocated;
}

int str_init(string_t *str)
{
   /*  Allocate memory for the string, set it to 0 + check. */
   char *data = scalloc(STR_MEM_CHUNK, sizeof(char));

   if(data == NULL)
      return STR_FAILURE;

   /* Set initial string information.   */
   str->len = 0;
   str->allocated = STR_MEM_CHUNK * sizeof(char);
   str->data = data;

   return STR_SUCCESS;
}

int str_init_cstring(string_t *str, const char *cstr)
{
   /*
    *  Calculate number of needed memory chunks.
    *  Terminating NULL character is not included in the calculation because
    *  at least 1 chunk is being added all the time. If we would include +1 in
    *  the calculation, in the case of strlen(cstr) + 1 == STR_MEM_CHUNK, 2
    *  chunks would be allocated instead of one.  Usage of this calculation
    *  formula effectively solves this problem.
    */
   int chunks_num = (strlen(cstr) / STR_MEM_CHUNK) + 1;

   /*  Allocate memory for the string and check.   */
   char *data = scalloc(chunks_num * STR_MEM_CHUNK, sizeof(char));

   if(data == NULL)
      return STR_FAILURE;

   /*  Set up the structure contents according to new string value.    */
   str->len = strlen(cstr);
   str->allocated = chunks_num * STR_MEM_CHUNK * sizeof(char);
   str->data = data;

   /*  Copy the contents from C-type string to the string structure.   */
   strncpy(str->data, cstr, str->len+1);

   return STR_SUCCESS;
}

int str_init_string(string_t *str_dst, string_t *str_src)
{
   /*  Undefined data pointer needs to be set to NULL. */
   memset(str_dst, 0, sizeof(string_t));

   return (str_copy_to_string(str_dst, str_src));
}

int str_set_cstring(string_t *str_dst, const char *cstr_src)
{
   int src_len = strlen(cstr_src);           // Pre-compute c-string size
   /*  If source C-string is longer than current string capacity - realloc. */
   if(src_len + 1 > str_dst->allocated)
   {
      /*  Using src_len instread of src_len+1 for a reason. See source code
       *  of str_init_cstring() for a closer description.  */
      int chunks_num = (src_len / STR_MEM_CHUNK) + 1;
      char *new_data = srealloc(str_dst->data, chunks_num * STR_MEM_CHUNK *
                                    sizeof(char));

      /*  Check if the reallocation was successful.   */
      if(new_data == NULL)
         return STR_FAILURE;

      /*  Allocation was successful - update string structure.    */
      str_dst->allocated = chunks_num * STR_MEM_CHUNK * sizeof(char);
      str_dst->data = new_data;
   }

   /*  Copy the source string to destination and update aux variables  */
   memset(str_dst->data, 0, str_dst->allocated);
   strncpy(str_dst->data, cstr_src, src_len + 1);
   str_dst->len = src_len;

   return STR_SUCCESS;
}
