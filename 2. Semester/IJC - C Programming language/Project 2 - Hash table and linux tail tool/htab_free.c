/*
Nazev: htab_free.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce htab_free
*/

#include "htab.h"

/**
* @brief Odstrani vsechny zaznamy z tabulky a smaze ji.
*
* @param t - tabulka na odstraneni
* @see htab_clear
*/
void htab_free(HashTable* t)
{
    htab_clear(t);
    free(t);
}
