/*
Nazev: adultGenerator.c
Reseni IOS Projekt 2, 27.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Modul urceny na generovani zadaneho poctu Adult procesu.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "childcare.h"

static void sigusr2_handler() //Tento handler pouziva i generator, i adult procesy na ukoceni pri chybe
{
    /*Prevzeti exit codu vsech childu a konec */
	while(wait(NULL) != -1);
	exit(EXIT_SYSCALL_FAIL);
}

/**
* @brief Main procesu na generovani Adult procesu
*
* Maximalne kazdych AGT mikrosekund generuje Adult proces, az do specifikovaneho poctu.
*
* @param shm - odkaz na alokovanou sdilenou pamet, obsahuje pocet Adultu na vygenerovani
* @param AGT - maximalni interval na generaci dalsiho Adult procesu
* @see struct shmData
* @see adultMain
*/
void adultGeneratorMain(shmData_t* shm, unsigned AGT)
{
    sem_wait(&(shm->genSync)); //Synchronizovany start obou generatoru
    signal(SIGUSR2, sigusr2_handler); //Nastaveni zpracovani signalu, ktery muze zaslat main pri chybe
	srand((int)time(0));

    /* Generace adult procesu s maximalnim casovym odstupem AGT */
    pid_t generatedAdult;
    for(unsigned i = 0; i < shm->adults; i++)
    {
        usleep(rand()%(AGT+1));
        generatedAdult = fork();
        if(generatedAdult == 0)
        {
			sem_wait(&(shm->shmAccess));
            shm->remainingAdults--;
			sem_post(&(shm->shmAccess));
            adultMain(shm, i+1);
            exit(0); //Zabiti adult procesu, pokud nahodou neskonci sam
        }
        else if(generatedAdult == -1)
        {
            fprintf(stderr,"Nepovedlo se vytvorit Adult proces!\n");
			kill(getppid(), SIGUSR1); //Zaslani signalu rodicovskemu procesu, signal znaci chybu
			while(1);
        }
    }
	while(wait(NULL) != -1); //Prevzeti exit codu vsech adult procesu pri jejich ukonceni
    exit(0);
}
