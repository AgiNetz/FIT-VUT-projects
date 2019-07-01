/*
Nazev: io.h
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Rozhrani modulu io.c, obsahuje prototypy funkcii
*/

#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

//Funkce na nacteni slova ze zadaneho souboru, maximalni delka slova zadana parametrem, slovo je oddelene isspace znakmi
int get_word(char*, int, FILE*);

#endif // IO_H_INCLUDED
