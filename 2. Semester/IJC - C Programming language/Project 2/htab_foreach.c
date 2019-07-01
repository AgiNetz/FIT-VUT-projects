/*
Nazev: htab_foreach.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce htab_foreach
*/

#include "htab.h"

/**
* @brief Pro kazdou polozku v tabulce t zavola funkcii func
*
* @param t - tabulka obsahujici zaznamy
* @param func - funkce, ktera sa bude volat pro kazdy prvek v tabulce. Vraci void a bere klic a data z prvku tabulky.
*/
void htab_foreach(HashTable* t, void (*func)(const char*, unsigned int*))
{
    for(size_t i = 0; i < htab_bucket_count(t); i++)
    {
        Item* iterator = t->pointers[i];
        while(iterator != NULL)
        {
            func(iterator->key, &(iterator->data));
            iterator = iterator->next;
        }
    }
}
