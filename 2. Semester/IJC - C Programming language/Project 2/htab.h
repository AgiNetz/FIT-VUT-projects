/*
Nazev: lib.h
Reseni IJC-DU2, uloha 2, 23.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Rozhrani knihovny wordcount
*/

#ifndef LIB_H_INCLUDED
#define LIB_H_INCLUDED

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
* Polozka tabulky htab_t. Asociouje retezec s celym cislem.
* @see htab_t
*/
typedef struct htab_listitem {
char* key;
unsigned int data;
struct htab_listitem* next;
} Item;

/**
* Hashovaci tabulka. Obsahuje pole zretezenych seznamu, do kterych se prvky pridaveji podle hashovaci funkce.
* @see htab_listitem
* @see hash_function
*/
typedef struct htab_t {
size_t arrSize;
size_t n;
Item* pointers[];
} HashTable;

typedef struct htab_t htab_t;

//Konstruktor htab_t, vytvori a inicializuje tabulku
HashTable* htab_init(size_t);

//Vytvoreni nove tabulky se zadanou velikosti z jiz existujici tabulky
HashTable* htab_move(size_t, HashTable*);

//Funkce vracejici pocet zaznamu v tabulce
size_t htab_size(HashTable*);

//Funkce vracejici pocet zretezenych velikost tabulky
size_t htab_bucket_count(HashTable*);

//Vyhledani prvku v tabulce. Pokud neexistuje, vytvori se
Item* htab_lookup_add(HashTable*, const char*);

//Vyhledani prvku v tabulce
Item* htab_find(HashTable*, const char*);

//Zavolani funkce pro kazdy prvek tabulky t, nemeni obsah tabulky
void htab_foreach(HashTable*, void (*)(const char*, unsigned int*));

//Smazani prvku se zadanym klicem z tabulky
bool htab_remove(HashTable*, const char*);

//Smazani vsech prvku tabulky, tabulka zustane alokovana
void htab_clear(HashTable*);

//Destruktor htab_t, smazani vsech prvku tabulky a tabulky samotne
void htab_free(HashTable*);

//Rozptylovaci funkce pro retezce pouzita pri rozdelovani prvku tabulky htab_t
unsigned int hash_function(const char*);


#endif // LIB_H_INCLUDED
