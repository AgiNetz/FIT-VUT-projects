/*
Nazev: wordcount.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Hlavni program. Spocita pocet vyskytu kazdeho slova ve vstupu a nasledne je vypise.
*/

#include <stdio.h>
#include <stdlib.h>
#include "htab.h"
#include "io.h"


//Velikost tabulky byla zvolena na zaklade prumerne slovne zasoby dospelyho cloveka, ktera se pohybuje v rozmezi 20000-35000. 
//Pri optimalni rozptylovaci funkcii pri teto velikosti 1 bucket nepresahne velikost 5, co umoznuje rychle a efektivni vyhledavani
#define HTAB_SIZE 8000
#define MAX_WORD_SIZE 256

/**
* @brief Vytiskne na stdout polozku z tabulky htab_t
*
* @param s - Klic polozky
* @param data - Data k danemu klici
*/
void print_word(const char* s, unsigned int* data)
{
    printf("%s\t%u\n", s, *data);
}

int main()
{
    htab_t* wordTable = htab_init(HTAB_SIZE);
    if(wordTable == NULL)
    {
        fprintf(stderr, "Nepovedlo se alokovat tabulku!");
    }

    char* word = malloc(MAX_WORD_SIZE*sizeof(char));

    if(word == NULL)
    {
        fprintf(stderr, "Nepovedlo se alokovat pamet pro slova!");
    }

    bool wordOverflow = false;
    int length = 0;
    while((length = get_word(word, MAX_WORD_SIZE, stdin)) != EOF)
    {
        if(length == MAX_WORD_SIZE) //slovo presahlo max delku
            wordOverflow = true;
        struct htab_listitem* zaznam = htab_lookup_add(wordTable, word);
        zaznam->data++;
    }
    if(wordOverflow)
        fprintf(stderr, "Jedno nebo vice slov presahuje maximalni delku slova! Tyhle slova byli zkraceny.\n");
    htab_foreach(wordTable, print_word); //tisk tabulky

    free(word);
    htab_free(wordTable);
    return 0;
}
