/*
Nazev: htab_size.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce htab_size
*/

#include "htab.h"

/**
* @brief Zjisti pocet zaznamu v tabulce t
*
* @param t - Tabulka, jejiz pocet zaznamu zjistujeme
* @return Pocet zaznamu v tabulce t
*/
size_t htab_size(HashTable* t)
{
    return t->n;
}
