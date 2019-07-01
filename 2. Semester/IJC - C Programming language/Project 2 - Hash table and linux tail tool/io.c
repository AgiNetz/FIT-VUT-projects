/*
Nazev: io.c
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Modul zodpovedny za IO komunikaci
*/

#include <stdio.h>
#include <ctype.h>
#include "io.h"

/**
* @brief Nacte nasledujici slovo  ze zadaneho souboru
*
* Nacte nasledujici slovo ze zadaneho souboru. Slovo je posloupnost znaku oddelena isspace znakmi.
*
* @param s - retezec, do ktereho se ma nactene slovo ulozit
* @param max - velikost retezce s, maximalni delka slova je max - 1
* @param f - soubor, ze ktereho se ma cist
* @return Velikost nacteneho slova. Pri presahnuti maximalni delky slova vrati max. Pokud se precetl jen EOF, tak vrati EOF
*/
int get_word(char *s, int max, FILE *f)
{
    int c = 0;
    int i = 0;
    c = fgetc(f);
    if(isspace(c))
        while((c = fgetc(f)) != EOF && isspace(c));
    if(c == EOF)
        return EOF;
    s[i] = c;
    i++;
    while((c = fgetc(f)) != EOF && !isspace(c) && i < max - 1)
    {
        s[i] = c;
        i++;
    }
    s[i] = '\0'; //zakonceni slova
    if(i == max - 1)
    {
        c = fgetc(f);
        if(c != EOF && !isspace(c))
            i++; //pokud slovo jeste neskoncilo, pridame k delce 1, co bude signifikovat, ze slovo bolo pridlouhe
        while((c = fgetc(f)) != EOF && !isspace(c)); //preskoceni zvysku slova, kdyz presahuje max delku
    }
    return i;
}
