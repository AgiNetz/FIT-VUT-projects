/*
Nazev: htab_remove.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce htab_remove
*/

#include "htab.h"

/**
* @brief Najde a odstrani z tabulky t zaznam s klicem key.
*
* @param t - tabulka, ze ktere se bude odstranovat
* @param key - klic zaznamu, ktery se bude odstranovat
* @return True kdyz se podarilo zaznam najit a odstranit. False kdyz zaznam neexistoval.
*/
bool htab_remove(HashTable* t,const char* key)
{
    unsigned int index = hash_function(key)%htab_bucket_count(t);
    Item* iterator = t->pointers[index];
    Item* previous = NULL;
    while(iterator != NULL)
    {
        if(strcmp(iterator->key, key) == 0)
        {
            if(previous == NULL)
            {
                t->pointers[index] = iterator->next;
                free(iterator->key);
                free(iterator);
                t->n--;
                return false;
            }
            previous->next = iterator->next;
            free(iterator->key);
            free(iterator);
            t->n--;
            return true;
        }
        previous = iterator;
        iterator = iterator->next;
    }
    return false;
 }
