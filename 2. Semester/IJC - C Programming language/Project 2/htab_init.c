/*
Nazev: htab_init.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce htab_init
*/

#include "htab.h"

/**
* @brief Konstruktor pro strukturu htab_t, vytvori a inicializuje na nulove hodnoty
*
* @param size - pozadovana velikost tabulky
* @return Odkaz na nove vytvorenou tabulku, pri selhani NULL
*/
HashTable* htab_init(size_t size)
{
    HashTable* t = malloc(sizeof(HashTable) + size*sizeof(Item*));
    if(t == NULL) return t;
    memset(t, 0, sizeof(HashTable) + size*sizeof(Item*)); //vynulovani velikosti + nastaveni pointru na NULL
    t->arrSize = size;
    return t;
}
