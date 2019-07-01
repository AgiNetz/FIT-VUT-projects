/**
 *  \file str_test.c
 *  \brief Testing interface for str.h IFJ module.
 *  \author Patrik Goldschmidt
 *  \date 26.10.2017
 */

#include "../../src/str.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
    char *teststr;                          // C-type string used for testing.

    /*  Create local string instance on local stack.  */
    string_t my_string;

    /*  Initialize string.  */
    str_init(&my_string);

    /*  Initial tests.  */
    printf("String contents\t\t: \"%s\"\n", str_get_data(&my_string));
    printf("String size    \t\t: %d elems\n", str_get_len(&my_string));
    printf("Allocated space\t\t: %d bytes\n", str_get_size(&my_string));
    printf("-------------------------------------------------\n");

    /*  String add char test.   */
    str_add_char(&my_string, 'p');
    str_add_char(&my_string, 'i');
    str_add_char(&my_string, 'c');
    str_add_char(&my_string, 'o');
    str_add_char(&my_string, 'v');
    str_add_char(&my_string, 'i');
    str_add_char(&my_string, 'n');
    str_add_char(&my_string, 'a');

    printf("Add char test  \t\t: \"%s\"\n", str_get_data(&my_string));
    printf("String size    \t\t: %d elems\n", str_get_len(&my_string));
    printf("-------------------------------------------------\n");

    /*  Clear string.   */
    str_clear(&my_string);

    printf("Clear string test\t: \"%s\"\n", str_get_data(&my_string));
    printf("String size    \t\t: %d elems\n", str_get_len(&my_string));
    printf("-------------------------------------------------\n");

    /*	Test adding special characters.	*/
    str_add_char(&my_string, ' ');
    str_add_char(&my_string, '#');
    str_add_char(&my_string, '\\');
    str_add_char(&my_string, '\007');

    printf("Special character test\t: \"%s\"\n", str_get_data(&my_string));
    printf("String size    \t\t: %d elems\n", str_get_len(&my_string));
    printf("-------------------------------------------------\n");

    /*  Free string.    */
    str_free(&my_string);

    /*  Reinit string with certain string more than preallocated value.  */
    str_init_cstring(&my_string, "0123456789012345678901234567890123456789012 \
                     34567890123456789012345678901234567890123456789012345678 \
                     01234567890123456789012345678901234567890123456789012345 \
                     01234567890123456789012345678901234567890123456789012345 \
                     01234567890123456789012345678901234567890123456789012345 \
                     01234567890123456789012345678901234567890123456789012345 \
                     01234567890123456789012345678901234567890123456789012345 \
                     01234567890123456789012345678901234567890123456789012345 \
                     01234567890123456789012345678901234567890123456789012345 \
                     01234567890123456789012345678901234567890123456789012345 \
                     01234567890123456789012345678901234567890123456789");

    printf("String C-string init\t: \"Some numbers, not gonna print it\"\n");
    printf("String size    \t\t: %d elems\n", str_get_len(&my_string));
    printf("Allocated space\t\t: %d bytes\n", str_get_size(&my_string));
    printf("-------------------------------------------------\n");

    /*  Clear string.    */
    str_clear(&my_string);

    printf("Clear string test\t: \"%s\"\n", str_get_data(&my_string));
    printf("String size    \t\t: %d elems\n", str_get_len(&my_string));
    printf("Allocated space\t\t: %d bytes\n", str_get_size(&my_string));
    printf("-------------------------------------------------\n");

    /* Set up string to string. */
    str_set_cstring(&my_string, "Ahoj Pista.");

    printf("Setup string test\t: \"%s\"\n", str_get_data(&my_string));
    printf("String size    \t\t: %d elems\n", str_get_len(&my_string));
    printf("Allocated space\t\t: %d bytes\n", str_get_size(&my_string));
    printf("-------------------------------------------------\n");

    /* Append C string to the current string. */
    str_append_cstring(&my_string, " Ja som Ditto.");

    printf("Setup string test\t: \"%s\"\n", str_get_data(&my_string));
    printf("String size    \t\t: %d elems\n", str_get_len(&my_string));
    printf("Allocated space\t\t: %d bytes\n", str_get_size(&my_string));
    printf("-------------------------------------------------\n");

    /*  Free string.    */
    str_free(&my_string);

    printf("Free string test\t: \"%s\"\n", str_get_data(&my_string));
    printf("String size    \t\t: %d elems\n", str_get_len(&my_string));
    printf("Allocated space\t\t: %d bytes\n", str_get_size(&my_string));
    printf("-------------------------------------------------\n");

    /*  Init string and force it to reallocate using add.   */
    str_init(&my_string);

    for(int i = 0; i < 4; i++)
        for(int j = 33; j < 123; j++)
        str_add_char(&my_string, j);

    printf("Add to string test\t: \"%s\"\n", str_get_data(&my_string));
    printf("String size    \t\t: %d elems\n", str_get_len(&my_string));
    printf("Allocated space\t\t: %d bytes\n", str_get_size(&my_string));
    printf("-------------------------------------------------\n");

    /*  Copy string to C-type string.   */
    teststr = str_copy_to_cstring(&my_string);

    printf("Copy to C-string\t: \"%s\"\n", teststr);

    /*  Copy string to another string.  */
    string_t my_str_test;

    /*  DONT DO THIS, "my_str_test" was not initialized, thus cant be used,
        because it is pointing to undefined address.
        !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    str_copy_to_string(&my_str_test, &my_string);

    */
    /*  Use init from string instead.   */
    str_init_string(&my_str_test, &my_string);

    printf("Copy another string\t: \"%s\"\n", str_get_data(&my_str_test));
    printf("-------------------------------------------------\n");
    printf("Compare strings\t\t: Str1, Str2 -> %d\n", str_cmpss(&my_string,
                                                                &my_str_test));
    printf("Compare strings\t\t: Str1, Cstr -> %d\n", str_cmpsc(&my_string,
                                                                teststr));
    printf("-------------------------------------------------\n");

    /*  Free the strings at the end!    */
    str_free(&my_str_test);
    str_free(&my_string);

    /* !!! Dont forget to free the created string!  */
    free(teststr);

    printf("\nAll functions have been successfully tested!\n");

    return EXIT_SUCCESS;
}
