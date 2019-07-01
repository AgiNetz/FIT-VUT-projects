/*
Nazev: adult.c
Reseni IOS Projekt 2, 27.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Modul obsahujici kod jednoho Adult procesu.
       Adult proces pri startu vstoupi do centra a similuje cinnost.
       Po ukonceni simulace cinnosti se pokousi z centra odejit.
       Pokud po jeho odchode zustane pocet Childu v centru mensi
       nez trojnasobek poctu Adultu v centru, muze odejit. V opacnem
       pripade musi cekat, az odejde dostatecny pocet Childu z centra.
*/

#include <stdio.h>
#include <stdlib.h>
#include "childcare.h"

void adultReport(shmData_t*, int, const char*);

/**
* @brief Vypis sprav o akciich Adult procesu
*
* Vypise spravu o aktualni cinnosti Adult procesu, automaticky inkrementuje citac akcii.
* V hlavickovem souboru childcare.h jsou definovany makra na pouziti s touto funkcii
*
* @param shm - odkaz na alokovanou sdilenou pamet, obsahuje pocet Adultu na vygenerovani
* @param number - identifikace Adult procesu
* @param message - sprava obsahujici informace o cinnosti procesu
* @see struct shmData
*/
void adultReport(shmData_t* shm, int number, const char* message)
{
    shm->actionCounter++;
    if(strcmp(message, CH_WAITING) == 0) //zprava waiting ma jiny format nez ostatni zpravy
    {
        fprintf(shm->out, "%d:\tA %d: %s : %d : %d\n", shm->actionCounter, number, message, shm->adultsInCenter, shm->childrenInCenter);
    }
    else
    {
        fprintf(shm->out, "%d:\tA %d: %s\n", shm->actionCounter, number, message);
    }
    fflush(shm->out); //okamzity zapis do souboru
}

/**
* @brief Main Adult procesu
*
* Simuluje cinnost dospeleho v childcare centru. Blize popsano v popisu modulu.
*
* @param shm - odkaz na alokovanou sdilenou pamet
* @param number - poradove cislo priradene procesu generatorem, pouziva ho na identifikaci
* @see struct shmData
* @see childMain
*/
void adultMain(shmData_t* shm, int number)
{
    /*Prichod procesu do centra */
    sem_wait(&(shm->shmAccess));
    adultReport(shm, number, CH_START);
	shm->adultsInCenter++;
    adultReport(shm, number, CH_ENTER);
	if(shm->waitingChildren) //Pokud nejake deti cekaji, vpustit je.
	{
		int n = (3 < shm->waitingChildren) ? 3 : shm->waitingChildren; //Maximalne vpustit 3 deti
		for(int i = 0; i < n; i++)
		{
			sem_post(&(shm->childEnter)); //Povoleni vstupu cekajicim detem
		}
        shm->waitingChildren -= n;
        shm->childrenInCenter += n; //Ill do it for you technika
	}
    sem_post(&(shm->shmAccess));

    /*Simulace cinnosti v kriticke sekcii */
    usleep(rand() % (shm->awt+1));

    /*Odchod procesu z centra */
    sem_wait(&(shm->shmAccess));
    adultReport(shm, number, CH_TRYLEAVE);
    if(3*(shm->adultsInCenter - 1) < shm->childrenInCenter) //Pokus o odchod
    {
        adultReport(shm, number, CH_WAITING);
        shm->leavingAdults++; //Zarazeni se skupiny cekajich na odchod
		sem_post(&(shm->shmAccess));
        sem_wait(&(shm->adultLeave)); //Ceka se na odchod dostatecneho poctu deti
        sem_wait(&(shm->shmAccess));
    }
	else
		shm->adultsInCenter--; //Pokud je povoleno, odejdeme

    adultReport(shm, number, CH_LEAVE);

    /*Pokud je centrum bez Adultu a dalsi se negeneruji, povolit vstup vsem detem. Zabranuje zablokovani aplikace */
    if(shm->adultsInCenter == 0 && shm->remainingAdults == 0)
	{
		shm->noAdultsLeft = 1; //nastaveni stavove promenne
		for(unsigned i = 0; i < shm->waitingChildren; i++) //Vpusteni cekajicich deti
			sem_post(&(shm->childEnter));
	}

	/*Ukonceni procesu s cekanim na ostatni procesy, vsechny procesy konci spolecne */
    shm->runningProcesses--;
    if(shm->runningProcesses > 0)
    {
        sem_post(&(shm->shmAccess));
        sem_wait(&(shm->finish));
        sem_wait(&(shm->shmAccess));
    }
    adultReport(shm, number, CH_FINISH);
    sem_post(&(shm->shmAccess));
    sem_post(&(shm->finish));
    exit(0);
}
