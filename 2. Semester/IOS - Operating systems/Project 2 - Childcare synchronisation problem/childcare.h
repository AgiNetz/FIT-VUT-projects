/*
Nazev: childcare.h
Reseni IOS Projekt 2, 27.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Hlavickovy soubor obsahujici prototypy, definice typu, makra a potrebne includy.
*/

#ifndef CHILDCARE_H_INCLUDED
#define CHILDCARE_H_INCLUDED

#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

/*Makra pro adultReport a childReport funkce, slouzi na sprehledneni kodu adult a child procesu */
#define CH_START "started"
#define CH_ENTER "enter"
#define CH_TRYLEAVE "trying to leave"
#define CH_LEAVE "leave"
#define CH_WAITING "waiting"
#define CH_FINISH "finished"

/*Definice exit kodu */
#define EXIT_WRONG_ARG 1
#define EXIT_SYSCALL_FAIL 2

/**
* @brief Struktura specifikujici obsah sdilene pameti
*/
typedef struct shmData {
    unsigned adults; //Pocet adultu na vygenerovani
    unsigned children; //Pocet childu na vygenerovani
    unsigned adultsInCenter; //Aktualni pocet adultu v centru
    unsigned childrenInCenter; //Aktualni pocet childu v centru
    unsigned awt; //Doba, po kterou adult simuluje cinnost v centru
    unsigned cwt; //Doba, po kterou child simuluje cinnost v centru
    unsigned actionCounter; //Citac akcii
    unsigned runningProcesses; //Pocet jeste neukoncenych child a adult procesu, pouzito na synchronizaci konce
    unsigned remainingAdults; //Pocet adultu, ktery se jeste vygeneruje
    unsigned leavingAdults; //Pocet adultu, ktery ceka na odchod z centra
	unsigned waitingChildren; //Pocet childu, ktery ceka na vstup do centra
	unsigned noAdultsLeft; //Stavova promena specifikujici, jestli uz dosli dospeli
    sem_t genSync; //Semafor na synchronizaci startu generatoru procesu
    sem_t adultLeave; //Semafor na kterem ceka rada adult procesu, ktere chteji z centra odejit ale nemohou
    sem_t childEnter; //Semafor na kterem ceka rada child procesu, ktere chteji do centra vstoupit ale nemohou
    /*Semafor slouzici na synchronizaci pristupu k sdilene pameti. Jelikoz 95 procent pristupu k teto pameti
    vyzaduje i zapisova prava, role ctenare a pisare neni rozdelena.*/
    sem_t shmAccess;
    sem_t finish; //Semafor slouzici na synchronizaci ukonceni vsech procesu
	FILE* out; //Ukazatel na vystupni soubor, kde se zapisuji spravy o akcich adult a child procesu
} shmData_t;

/*Prototypy funkcii */
void adultMain(shmData_t*, int);
void childMain(shmData_t*, int);
void adultGeneratorMain(shmData_t*, unsigned);
void childGeneratorMain(shmData_t*, unsigned);

#endif // CHILDCARE_H_INCLUDED
