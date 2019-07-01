/*
Nazev: htab_move.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce htab_move
*/

#include "htab.h"

/**
* @brief Move konstruktor, vytvoreni a inicializace tabulky s daty z t2
*
* Pomoci funkce htab_init vytvori a inicializuje novou tabulku o zadane velikosti. Nasledne do ni
* prekopiruje data z tabulky t2, ze ktere jsou odstraneny. Po dokonceni je t2 prazdna, ale zustava
* alokovana.
*
* @param newsize - Pozadovana velikost nove tabulky
* @param t2 - tabulka obsahujici data na prekopirovani do nove tabulky
* @return Odkaz na nove vytvorenou tabulku, pri selhani NULL
* @see htab_init
*/
HashTable* htab_move(size_t newsize, HashTable* t2)
{
    HashTable* t = htab_init(newsize);
    if(t == NULL) return t;
    for(size_t i = 0; i < htab_bucket_count(t2); i++)
    {
        Item* iterator = t2->pointers[i];
        t2->pointers[i] = NULL; //preruseni spojeni v t2
        while(iterator != NULL) //prochazeni zretezenym seznamem
        {
            unsigned int index = hash_function(iterator->key)%newsize;
            Item* movedItem = iterator;
            iterator = iterator->next;
            //pridani polozky na zacatek tabulky t
            movedItem->next = t->pointers[index];
            t->pointers[index] = movedItem;
            t2->n--;
            t->n++;
        }
    }
    return t;
}
