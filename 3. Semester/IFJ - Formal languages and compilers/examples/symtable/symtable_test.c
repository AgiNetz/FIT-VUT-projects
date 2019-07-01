/**
 *  @file testmodule.c
 *  @brief Symbol table source file for IFJ Project 2017 AT vut.fit.vutbr.cz.
 *  @date 10.10.2017
 *  @author Patrik Goldschmidt - xgolds00@fit.vutbr.cz
 *
 *  Module tests symbol tab library functions for a compiler project AT vutbr.
 */

#include "../../src/symtable.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_stab(symtab_t *tab);

int main()
{
    char *sym_id_dynamic = calloc(20, sizeof(char));
    symbol_t symb;

    printf("--------------------  Fresh table test:  ---------------------\n");
    /*  Create and initialize table.   */
    symtab_t symbols;
    stab_init(&symbols);

    /*  Print the table initially.  */
    print_stab(&symbols);

    printf("------------------  Multiple inserts test:  ------------------\n");

    /*  Make a first insert. */
    char *sym_id = "Pracovanie";
    symb.id = sym_id;
    stab_insert(&symbols, &symb);

    /*  Insert couple more. */
    strncpy(sym_id_dynamic, "Gosh", 20);
    symb.id = sym_id_dynamic;
    stab_insert(&symbols, &symb);

    strncpy(sym_id_dynamic, "Ditto", 20);
    symb.id = sym_id_dynamic;
    stab_insert(&symbols, &symb);

    strncpy(sym_id_dynamic, "Johny", 20);
    symb.id = sym_id_dynamic;
    stab_insert(&symbols, &symb);

    strncpy(sym_id_dynamic, "UsingTheMaxLengthh", 20);
    symb.id = sym_id_dynamic;
    stab_insert(&symbols, &symb);

    strncpy(sym_id_dynamic, "Duplicate", 20);
    symb.id = sym_id_dynamic;
    stab_insert(&symbols, &symb);

    strncpy(sym_id_dynamic, "Duplicate", 20);
    symb.id = sym_id_dynamic;
    stab_insert(&symbols, &symb);

    /*  Print the table after couple of inserts.    */
    print_stab(&symbols);

    printf("------------------  Element search test:  --------------------\n");
    printf("Searching element \"Dutto\"\t: \t");
    (stab_search(&symbols, "Dutto")) ? printf("Found!\n") :
                                       printf("Not Found :(\n");

    printf("Searching element \"Ditto\"\t: \t");
    (stab_search(&symbols, "Ditto")) ? printf("Found!\n") :
                                       printf("Not Found :(\n");

    printf("-----------------  Element get data test:  -------------------\n");
    (stab_get_data(&symbols, "Gosh")) ?
    printf("Data of \"Gosh\" is:\t%s\n", stab_get_data(&symbols, "Gosh")->id)
                                      :
    printf("Element \"Gosh\" not found!");

    (stab_get_data(&symbols, "John")) ?
    printf("Data of \"John\" is:\t%s\n", stab_get_data(&symbols, "John")->id)
                                      :
    printf("Element \"John\" not found!\n");

    printf("--------------------  Table clear test:  ---------------------\n");
    stab_clear(&symbols);
    /*  Print after clearing.   */
    print_stab(&symbols);

    printf("-------------------  Insert after clear:  --------------------\n");

    strncpy(sym_id_dynamic, "Newka", 20);
    symb.id = sym_id_dynamic;
    stab_insert(&symbols, &symb);

    print_stab(&symbols);

    printf("----------------  Delete symbol \"Newka\":  ------------------\n");

    stab_delete_symbol(&symbols, "Newka");

    print_stab(&symbols);

    /*  Free dynamically allocated string.  */
    free(sym_id_dynamic);

    return EXIT_SUCCESS;
}

void print_stab(symtab_t *tab)
{
    stab_item_t *item;
    bool printed = false;

    for(int i = 0; i < SYMTAB_SIZE; i++)
    {
        item = (*tab)[i];

        if((*tab)[i])
            printf("[%d]: ", i);

        for(; item; item = item->item_next)
        {
            printed = true;
            printf("\"%s\" --> ", item->data.id);
        }

        if((*tab)[i])
            printf("NULL\n");
    }

    if(!printed)
        printf("Table is empty.\n");

    return;
}
