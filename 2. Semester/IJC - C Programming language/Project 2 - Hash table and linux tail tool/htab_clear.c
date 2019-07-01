/*
Nazev: htab_clear.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce htab_clear
*/

#include "htab.h"

/**
* @brief Odstrani vsechny zaznamy ze zadane tabulky. Tabulka zustane prazdna a alokovana.
*
* @param t - tabulka, ze ktere se bude odstranovat
* @see htab_free
*/
void htab_clear(HashTable* t)
{
    for(size_t i = 0; i < htab_bucket_count(t); i++)
    {
        Item* iterator = t->pointers[i];
        while(iterator != NULL)
        {
            free(iterator->key);
            Item* tmp = iterator;
            iterator = iterator->next;
            free(tmp);
            t->n--;
        }
        t->pointers[i] = NULL;
    }
}
