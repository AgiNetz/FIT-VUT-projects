/*
Nazev: htab_lookup_add.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce htab_lookup_add
*/

#include "htab.h"

/**
* @brief Pokusi se najit zaznam se zadanym klicem v tabulce t. Pri selhani zaznam vytvori a prida.
*
* Projde tabulku hledajic zaznam s klicem key. Pri selhani zaznam s tymto klicem vytvori a prida ho do tabulky.
*
* @param t - tabulka, ve ktere se bude vyhledavat
* @param key - klic, podle ktereho se bude vyhledavat
* @return Odkaz na nalezeny nebo nove vytvoreny zaznam v tabulce
* @see htab_find
*/
Item* htab_lookup_add(HashTable* t, const char* key)
{
    unsigned int index = hash_function(key)%htab_bucket_count(t);
    Item* newItem = htab_find(t, key);
    if(newItem != NULL)
        return newItem;
    newItem = malloc(sizeof(Item));
    if(newItem == NULL)
        return NULL;
    newItem->key = malloc(sizeof(char)*strlen(key) + 1);
    if(newItem->key == NULL)
    {
        free(newItem);
        return NULL;
    }
    strcpy(newItem->key, key);
    newItem->next = t->pointers[index];
    newItem->data = 0;
    t->pointers[index] = newItem;
    t->n++;
    return newItem;
}

