/*
Nazev: childGenerator.c
Reseni IOS Projekt 2, 27.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Modul urceny na generovani zadaneho poctu Child procesu.
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "childcare.h"

static void sigusr2_handler() //Tento handler pouziva i generator, i child procesy na ukoceni pri chybe
{
    /*Prevzeti exit codu vsech childu a konec */
	while(wait(NULL) != -1);
	exit(EXIT_SYSCALL_FAIL);
}

/**
* @brief Main procesu na generovani Child procesu
*
* Maximalne kazdych CGT mikrosekund generuje Child proces, az do specifikovaneho poctu.
*
* @param shm - odkaz na alokovanou sdilenou pamet, obsahuje pocet Childu na vygenerovani
* @param CGT - maximalni interval na generaci dalsiho Child procesu
* @see struct shmData
* @see childMain
*/
void childGeneratorMain(shmData_t* shm, unsigned CGT)
{
    sem_wait(&(shm->genSync)); //Synchronizovany start obou generatoru
	signal(SIGUSR2, sigusr2_handler); //Nastaveni zpracovani signalu, ktery muze zaslat main pri chybe
    srand((int)time(0));

    /* Generace adult procesu s maximalnim casovym odstupem CGT */
    pid_t generatedChild;
    for(unsigned i = 0; i < shm->children; i++)
    {
        usleep(rand()%(CGT+1));
        generatedChild= fork();
        if(generatedChild == 0)
        {
            childMain(shm, i + 1);
            exit(0); //Zabiti adult procesu, pokud nahodou neskonci sam
        }
        else if(generatedChild == -1)
        {
            fprintf(stderr,"Nepovedlo se vytvorit Child proces!\n");
			kill(getppid(), SIGUSR1); //Zaslani signalu rodicovskemu procesu, signal znaci chybu
			while(1);
        }
    }
    while(wait(NULL) != -1); //Prevzeti exit codu vsech adult procesu pri jejich ukonceni
    exit(0);
}
