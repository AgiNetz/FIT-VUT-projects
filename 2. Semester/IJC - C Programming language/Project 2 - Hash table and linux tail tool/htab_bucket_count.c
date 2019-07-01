/*
Nazev: htab_bucket_count.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce htab_bucket_count
*/

#include "htab.h"

/**
* @brief Zjisti velikost tabulky t
*
* @param t - Tabulka, jejiz velikost zjistujeme
* @return Velikost tabulky t
*/
size_t htab_bucket_count(HashTable* t)
{
    return t->arrSize;
}
