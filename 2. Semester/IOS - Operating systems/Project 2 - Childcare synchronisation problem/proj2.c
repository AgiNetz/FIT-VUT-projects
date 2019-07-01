/*
Nazev: proj2.c
Reseni IOS Projekt 2, 27.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Hlavni modul programu. Nacte a zpracuje argumenty, vytvori a inicializuje sdilenou pamet,
       vytvori a inicializuje semafory pro synchronizaci a vygeneruje 2 pomocne procesy slouzici
       na generaci Adult a Child procesu.
*/

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include "childcare.h"

/* Makra definovany pro nacteni argumentu s zachovanim citelnosti kodu*/
#define numberOfAdults args[0]
#define numberOfChildren args[1]
#define maxAdultGenerationInterval args[2]
#define maxChildGenerationInterval args[3]
#define maxAdultOperationInterval args[4]
#define maxChildOperationInterval args[5]

/*Maximalni hodnoty argumentu */
#define MAXAGT 5000u
#define MAXCGT 5000u
#define MAXAWT 5000u
#define MAXCWT 5000u

/*Jmeno sdilene pameti */
#define SHMName "/xdanis05proj2shm"

void cleanup(shmData_t*);

void sigusr1_handler()
{
	kill(0, SIGUSR2); //Poslat SIGUSR2 vsem procesum v skupine, SIGUSR2 nastaven na ukonceni potomku a vlastni smrt
}

/**
* @brief Volana pri ukoncovani programu. Dealokuje vsechny pouzivane zdroje.
*
* @param shm - odkaz na alokovanou sdilenou pamet
* @see struct shmData
*/
void cleanup(shmData_t* shm)
{
    if(shm->out != NULL)
        fclose(shm->out);
    /*Dealokace semaforu
    Pokud nektery ze semaforu neexistuje, sem_destroy selze, ale to nam nevadi */
    sem_destroy(&(shm->genSync));
    sem_destroy(&(shm->adultLeave));
    sem_destroy(&(shm->childEnter));
    sem_destroy(&(shm->shmAccess));
    sem_destroy(&(shm->finish));
    /*Vynuceni zapisu a uvolneni sdilene pameti*/
    msync(shm, sizeof(shmData_t), 0);
    munmap(shm, sizeof(shm));
    shm_unlink(SHMName);
}

int main(int argc,char* argv[])
{
    if(argc != 7)
    {
        fprintf(stderr, "Zadany pocet argumentu neodpovida ocekavanemu formatu!\n");
        return EXIT_WRONG_ARG;
    }

    /*Nacteni argumentu a jejich konverze na cislo.
      Jednotlive prvky pole maji pridelene makrem
      definovane jmeno pro jednoduchost pouziti.*/
    unsigned int args[6] = {0};
    for(int i = 0; i < 6; i++)
    {
        char* err = NULL;
        args[i] = strtoul(argv[i+1], &err, 10);
        if(*err != 0)
        {
            fprintf(stderr, "Argument cislo %d neni cele cislo!\n", i+1);
            return EXIT_WRONG_ARG;
        }
    }

    /*Kontrola rozmezi argumentu*/
    if(maxAdultGenerationInterval > MAXAGT)
    {
        fprintf(stderr, "Maximalni interval pro generovani Adult procesu musi byt v rozmezi 0 <= AGT <= %u\n", MAXAGT);
        return EXIT_WRONG_ARG;
    }
    if(maxAdultGenerationInterval > MAXCGT)
    {
        fprintf(stderr, "Maximalni interval pro generovani Child procesu musi byt v rozmezi 0 <= CGT <= %u\n", MAXCGT);
        return EXIT_WRONG_ARG;
    }
    if(maxAdultGenerationInterval > MAXAWT)
    {
        fprintf(stderr, "Interval simulace cinnosti Adult procesu musi byt v rozmezi 0 <= AWT <= %u\n", MAXAWT);
        return EXIT_WRONG_ARG;
    }
    if(maxAdultGenerationInterval > MAXCWT)
    {
        fprintf(stderr, "Interval simulace cinnosti Adult procesu musi byt v rozmezi 0 <= CWT <= %u\n", MAXCWT);
        return EXIT_WRONG_ARG;
    }

    /*Uprava jednotek intervalu pro generaci a operaci procesu */
    maxAdultGenerationInterval *= 1000;
    maxChildGenerationInterval *= 1000;
    maxAdultOperationInterval *= 1000;
    maxChildOperationInterval *= 1000;

    /*Alokace, nastaveni, namapovani a inicializace sdilene pameti*/
    int fd = shm_open(SHMName, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR); //alokace
    if(fd == -1)
    {
        fprintf(stderr, "Nepovedlo se vytvorit sdilenou pamet!\n");
        return EXIT_SYSCALL_FAIL;
    }
    ftruncate(fd, sizeof(shmData_t)); //nastaveni velikosti
    shmData_t* shm = mmap(NULL, sizeof(shmData_t), PROT_READ | PROT_WRITE, MAP_SHARED ,fd ,0); //mapovani
    close(fd); //file descriptor uz nepotrebujeme

    /* Inicializace sdilene pameti */
    shm->out = fopen("proj2.out", "w");
    if(shm->out == NULL)
    {
        fprintf(stderr, "Nepovedlo se otevrit soubor pro vypis!\n");
        cleanup(shm);
        return EXIT_SYSCALL_FAIL;
    }
    shm->adults = numberOfAdults;
    shm->children = numberOfChildren;
    shm->awt = maxAdultOperationInterval;
    shm->cwt = maxChildOperationInterval;
    shm->remainingAdults = numberOfAdults;
	shm->runningProcesses = numberOfAdults + numberOfChildren;


    /*Vytvoreni a inicializace semaforu potrebnych pro synchronizaci*/
    if(sem_init(&(shm->genSync), 1, 0) != 0)
    {
        fprintf(stderr, "Nepovedlo se vytvorit semafor pro synchronizaci generatoru!\n");
        cleanup(shm);
        return EXIT_SYSCALL_FAIL;
    }
    if(sem_init(&(shm->childEnter), 1, 0) != 0)
    {
        fprintf(stderr, "Nepovedlo se vytvorit semafor pro kontrolu vstupu Childu do centra!\n");
        cleanup(shm);
        return EXIT_SYSCALL_FAIL;
    }
    if(sem_init(&(shm->adultLeave), 1, 0) != 0)
    {
        fprintf(stderr, "Nepovedlo se vytvorit semafor pro kontrolu vystupu Adultu z centra!\n");
        cleanup(shm);
        return EXIT_SYSCALL_FAIL;
    }
    if(sem_init(&(shm->shmAccess), 1, 1) != 0)
    {
        fprintf(stderr, "Nepovedlo se vytvorit semafor pro synchronizaci pristupu k pameti!\n");
        cleanup(shm);
        return EXIT_SYSCALL_FAIL;
    }
    if(sem_init(&(shm->finish), 1, 0) != 0)
    {
        fprintf(stderr, "Nepovedlo se vytvorit semafor pro synchronizaci ukonceni procesu!\n");
        cleanup(shm);
        return EXIT_SYSCALL_FAIL;
    }

	signal(SIGUSR1, sigusr1_handler); //Nastaveni obsluhy SIGUSR1, ktery mohou poslat generatory pri chybe

	/*Vytvoreni procesu na generaci Adult a Child procesu */
    pid_t adultGenerator;
    pid_t childGenerator;

    /*Vytvoreni procesu na generaci Adultu */
    if((adultGenerator = fork()) == 0)
    {
        adultGeneratorMain(shm, maxAdultOperationInterval);
        exit(0); //Ukonceni generatoru pokud nahodou neskoncil sam
    }
    else if(adultGenerator == -1)
    {
        fprintf(stderr, "Nepovedlo se vytvorit proces na generovani Adult procesu!\n");
        cleanup(shm);
        return EXIT_SYSCALL_FAIL;
    }

    /*Vytvoreni procesu na generaci adultu */
    if((childGenerator = fork()) == 0)
    {
        childGeneratorMain(shm, maxChildOperationInterval);
        exit(0); //Ukonceni generatoru pokud nahodou neskoncil sam
    }
    else if(childGenerator == -1)
    {
        fprintf(stderr, "Nepovedlo se vytvorit proces na generovani Child procesu!\n");

        /*Pokud selhalo vytvoreni procesu na generaci Childu, musime zabit proces na generovani Adultu a skoncit */
        kill(adultGenerator, SIGTERM);
        wait(NULL); //Prevzati exit codu od generatoru Adultu
        cleanup(shm);
        return EXIT_SYSCALL_FAIL;
    }

    /*Generatory procesu maji synchronizovani start. Vyhneme se teda potizim pri selhani vytvoreni nektereho z nich */
    sem_post(&(shm->genSync));
    sem_post(&(shm->genSync));

    /*Cekani na skonceni obou generatoru. Pokud nektery skonci s nenulovym kodem, nastala chyba a koncime taky s chybou */
	int childExitCode;
	for(int i = 0; i < 2; i++)
	{
		wait(&childExitCode);
		if(childExitCode != 0)
		{
			wait(NULL);
			cleanup(shm);
			return EXIT_SYSCALL_FAIL;
		}
	}

	/*Po prevzeti exit codu obou generatoru, uvolneni zdroju a uspesny konec */
    cleanup(shm);
    return EXIT_SUCCESS;
}
