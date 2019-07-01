/*
Nazev: hash_function.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Knihovni funkce hash_function
*/

#include "htab.h"

/**
* @brief Rozptylovaci funkce tabulky htab_t
*
* Hashovaci funkce, ktery vysledek se pouziva pri indexovani tabulky htab_t. Uvedena implementace se ukazala jako vhodna
* na rozptylovani retezcu v tabulce rovnomerne.
*
* @param str - retezec, ktery se ma zahashovat
* @return Hash zadaneho retezce
*/
unsigned int hash_function(const char *str)
{
    unsigned int h=0;
    const unsigned char *p;
    for(p=(const unsigned char*)str; *p!='\0'; p++)
        h = 65599*h + *p;
    return h;
}
