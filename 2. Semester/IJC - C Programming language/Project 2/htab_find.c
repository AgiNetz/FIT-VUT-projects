/*
Nazev: htab_find.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce htab_find
*/

#include "htab.h"

/**
* @brief Najde v tabulce t zaznam s klicem key.
*
* @param t - tabulka, ve ktere se bude vyhledavat
* @param key - klic, podle ktereho se bude vyhledavat
* @return Odkaz na nalezeny zaznam v tabulce. Jinak NULL
* @see htab_lookup_add
*/
Item* htab_find(HashTable* t, const char* key)
{
    unsigned int index = hash_function(key)%htab_bucket_count(t);
    Item* iterator = t->pointers[index];
    while(iterator != NULL)
    {
        if(strcmp(iterator->key, key) == 0)
            return iterator;
        iterator = iterator->next;
    }
    return NULL;
}
