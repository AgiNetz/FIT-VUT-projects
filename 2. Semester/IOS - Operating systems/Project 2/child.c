/*
Nazev: child.c
Reseni IOS Projekt 2, 27.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Modul obsahujici kod jednoho Child procesu.
       Child proces se pri startu pokusi vstoupit do centra.
       Pokud bude v centru po jeho vstupu aspon 3x vic Adult
       procesu, vstup je mu povolen. V opacnem pripade musi
       cekat, kym neprijde dalsi Adult do centra. Po vstupu
       simuluje cinnost a nasledne odchazi.
*/

#include <stdio.h>
#include <stdlib.h>
#include "childcare.h"

void childReport(shmData_t*, int, const char*);

/**
* @brief Vypis sprav o akciich Child procesu
*
* Vypise spravu o aktualni cinnosti Child procesu, automaticky inkrementuje citac akcii.
* V hlavickovem souboru childcare.h jsou definovany makra na pouziti s touto funkcii
*
* @param shm - odkaz na alokovanou sdilenou pamet
* @param number - identifikace Child procesu
* @param message - sprava obsahujici informace o cinnosti procesu
* @see struct shmData
*/
void childReport(shmData_t* shm, int number, const char* message)
{
    shm->actionCounter++;
    if(strcmp(message, CH_WAITING) == 0)
    {
        fprintf(shm->out, "%d:\tC %d: %s : %d : %d\n", shm->actionCounter, number, message, shm->adultsInCenter, shm->childrenInCenter);
    }
    else
    {
        fprintf(shm->out, "%d:\tC %d: %s\n", shm->actionCounter, number, message);
    }
    fflush(shm->out);
}

/**
* @brief Main Child procesu
*
* Simuluje cinnost ditete v childcare centru. Blize popsano v popisu modulu.
*
* @param shm - odkaz na alokovanou sdilenou pamet
* @param number - poradove cislo priradene procesu generatorem, pouziva ho na identifikaci
* @see struct shmData
* @see adultMain
*/
void childMain(shmData_t* shm, int number)
{
    /*Prichod procesu do centra */
    sem_wait(&(shm->shmAccess));
    childReport(shm, number, CH_START);
    if((shm->childrenInCenter >= 3*shm->adultsInCenter) && !shm->noAdultsLeft) //Kontrola moznosti vstupu
    {
        childReport(shm, number, CH_WAITING);
		shm->waitingChildren++; //Zarazeni se do fronty cekajicich deti
        sem_post(&(shm->shmAccess));
        sem_wait(&(shm->childEnter)); //Nutnost cekat
        sem_wait(&(shm->shmAccess));
    }
	else
	{
		shm->childrenInCenter++; //jinak vstup do centra
	}
    childReport(shm, number, CH_ENTER);
    sem_post(&(shm->shmAccess));

    /*Simulace cinnosti v kriticke sekcii */
    usleep(rand() % (shm->cwt+1));

    /*Odchod procesu z centra */
    sem_wait(&(shm->shmAccess));
    childReport(shm, number, CH_TRYLEAVE);
    shm->childrenInCenter--; //odchod z centra
    childReport(shm, number, CH_LEAVE);
    if((3*(shm->adultsInCenter - 1) >= shm->childrenInCenter) && shm->leavingAdults) //kontrola jestli muze cekajici adult odejit
	{
		shm->leavingAdults--; //Ill do it for you technika
		shm->adultsInCenter--; //Ill do it for you technika
		sem_post(&(shm->adultLeave)); //Povoleni odchodu adultovi co ceka na odchod
	}

    /*Ukonceni procesu s cekanim na ostatni procesy, vsechny procesy konci spolecne */
    shm->runningProcesses--;
    if(shm->runningProcesses > 0)
    {
        sem_post(&(shm->shmAccess));
        sem_wait(&(shm->finish));
        sem_wait(&(shm->shmAccess));
    }
    childReport(shm, number, CH_FINISH);
    sem_post(&(shm->shmAccess));
    sem_post(&(shm->finish));
    exit(0);
}
