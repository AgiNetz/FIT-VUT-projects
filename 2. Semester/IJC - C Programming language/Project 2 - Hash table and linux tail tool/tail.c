/*
Nazev: tail.c
Reseni IJC-DU2, uloha 1a, 17.04.2017
Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
Prelozeno: gcc 5.4.0
Popis: Implementace POSIX programu tail. Podporuje vstup ze soubor i stdin. Je mozne zadat
argument -n, kterym se specifikuje pocet radku, ktere vypsat. Pri vstupu z stdin existuje
implementacni limit delky radku 1024 bajtu.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TAIL_BUFSIZ 8192
#define STD_TAIL_BUFSIZ 1025

/**
 * @brief Funkce na zpracovani argumentu programu
 *
 * Zpracuje argumenty programu a do poskytnutych promennych ulozi relevantni informace
 *
 * @param argc - Pocet argumentu programu
 * @param argv - Pole obsahujici jednotlive argumenty programu
 * @param n - Parameter, do ktereho se ulozi pocet radku na vypsani, pokud je zadany. Implicitne 10.
 * @param fileIndex - Parameter, do ktereho se ulozi index jmena suboru na cteni v argv. -1 pro vstup ze stdin.
 * @return Vraci 0 pri uspechu, <0 pri selhani
 */

int argProc(int argc, char* argv[], int* n, int* fileIndex)
{
    if(argc > 4)
        return -1;
    bool fileGiven = false;
	bool nSet = false;
    for(int i = 1; i < argc; i++)
    {
        if(strcmp("-n", argv[i]) == 0)
        {
	if(nSet)
	return -1;
            if(i != argc - 1)
            {
                char** err = argv; //referencni inicializace, dulezite je jestli strtoul hodnotu zmeni
                *n = strtoul(argv[i+1], err ,10);
                if(err != argv)
                    return -1;
                i++;
		nSet = true;
            }
            else
                return -1;
        }
        else
        {
            if(!fileGiven) //nacetli jsme uz jine jmeno souboru?
            {
                *fileIndex = i;
                fileGiven = true;
            }
            else
                return -1;
        }
    }
    return 0;
}

/**
 * @brief Varianta programu tail pro cteni z stdin
 *
 * Vypise poslednich n radku ze zadaneho souboru. Delka radku je omezena na 1024 bajtu.
 *
 * @param n - Pocet radku na vypis
 * @return Vraci 0 pri uspechu, <0 pri selhani
 */
int tail_std(int n)
{
    char(* buffer)[STD_TAIL_BUFSIZ] = malloc(sizeof(char) * STD_TAIL_BUFSIZ *n); //cyklicky buffer
    if(buffer == NULL)
    {
        fprintf(stderr, "Nepovedlo se alokovat pamet!\n");
        return 2;
    }
	if(n == 0)
	{
		return 0;
	}
    memset(buffer, 0, sizeof(char) * STD_TAIL_BUFSIZ * n); //nulovani, pri vypisu je potrebne
    int currentBuf = 0; //index v cyklickem bufferu
    bool error = false;
    while(fgets(buffer[currentBuf], STD_TAIL_BUFSIZ, stdin))
    {
        if(memchr(buffer[currentBuf], '\n', STD_TAIL_BUFSIZ) == NULL) //scan na newline znak
        {
            buffer[currentBuf][STD_TAIL_BUFSIZ-2] = '\n'; //zalomeni radku, posledni znak je 0, predposledni musi byt znak konce radku
            if(!error)
            {
                fprintf(stderr, "Delka radku presahuje %d znaku! Prilis dlouhe radky budou orezany!\n", STD_TAIL_BUFSIZ);
                //pokud se nenasel newline, vypsani chybu pri prvnim vyskytu
                error = true;
            }
            int c = 0;
            while(((c = fgetc(stdin)) != '\n') || (c != EOF)) //preskoceni zvysku radku
                break;
        }
        currentBuf = (currentBuf + 1)%n;
    }
    for(int i = (currentBuf + 1)%n; i != currentBuf; i = (i + 1)%n)
    {
        printf("%.1024s", buffer[i]); //vypis, pokud bylo radku mene jako 10, nevypise se nic jelikoz buffer byl vynulovan
    }
    free(buffer);
    return 0;
}

/**
 * @brief Varianta programu tail pro cteni ze souboru
 *
 * Vypise poslednich n radku ze zadaneho souboru. Delka radku je omezena jen dostupnou pameti. Optimalizovany i pro velke soubory.
 *
 * @param fileName - Jmeno vstupniho souboru
 * @param n - Pocet radku na vypis
 * @return Vraci 0 pri uspechu, <0 pri selhani
 */
int tail_file(char* fileName, int n)
{
    FILE* f = fopen(fileName, "rb");
    if(f == NULL)
    {
        fprintf(stderr, "Nepovedlo se otevrit soubor!\n");
        return 1;
    }
	if(n == 0)
	{
		fclose(f);
		return 0;
	}
    bool start = false;
    char* buffer = malloc(1 + sizeof(char) * TAIL_BUFSIZ); //pocatecni velikost bufferu
    if(buffer == NULL)
    {
        fprintf(stderr, "Nepovedlo se alokovat pamet!\n");
        fclose(f);
        return 1;
    }
    int lines = 0;
    long offset = -1024; //cteme od konce, poslednich -offset znaku
    int bufMultiplier = 1;
    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f); //velikost souboru
    while(!start)
    {
        lines = 0;
        if(fseek(f, offset, SEEK_END)) //presun na -offset. znak od konce, necteme cely soubor zbytecne
        {
            if(fileSize < -offset)
            {
                fseek(f, 0, SEEK_SET); //kdyby jsme sli za zacatek souboru, fseek nic nedela, nastavime se na zacatek souboru a cteme ho cely
                start = true;
            }
            else
            {
                fprintf(stderr, "Nepovedlo se precist soubor!\n");
                free(buffer);
                fclose(f);
                return 1;
            }
        }
        int charsRead = fread(buffer, sizeof(char), -offset, f);
        if(charsRead == 0)
            break;
        buffer[charsRead] = 0; //fread nezakoncuje retezce, v bufferu bylo udelano misto na \0
        for(int i = charsRead-2; i >= 0; i--)
        {
            if(buffer[i] == '\n') //ratani poctu radku
            {
                lines++;
            }
            if(lines == n) //dosazeni poctu radku - vypis a konec
            {
                printf("%s", &buffer[i+1]);
                free(buffer);
                fclose(f);
                return 0;
            }
        }
        if(start) //pokud jsme cetli soubor od zacatku a jsme tu, obsahuje mene nez 10 tak ho vypiseme cely
        {
            printf("%s", buffer);
        }
        else //kdyz jsme neprecetli 10 radku, zvetsime pocet bajtu, ktere cteme od konce
        {
            offset *= 2;
            if(-offset > bufMultiplier*TAIL_BUFSIZ)
            {
                bufMultiplier *= 2;
                if((buffer = realloc(buffer, bufMultiplier*TAIL_BUFSIZ + 1)) == NULL) //v pripade potreby zvetsime buffer + kontrola
		{
                    fprintf(stderr, "Nepovedlo se alokovat dodatecni pamet!\n");
                    return 1;
                }
            }
        }
    }
    free(buffer);
    fclose(f);
    return 0;
}

/**
 * @brief Funkce main
 *
 * Zpracuje argumenty, a podle nich vola ruzne implementace programu tail
 *
 * @param argc - Pocet argumentu programu
 * @param argv - Pole obsahujici jednotlive argumenty programu
 * @return Vraci 0 pri uspechu, <0 pri selhani
 * @see argProc
 * @see tail_std
 * @see tail_file
 */
int main(int argc, char* argv[])
{
    int n = 10;
    int fileIndex = -1; //Pokud hodnotu argProc() nezmeni, cteni z stdin
    if(argProc(argc, argv, &n, &fileIndex))
    {
        fprintf(stderr, "Nastala chyba pri cteni argumentu!\n");
        return -1;
    }
    if(n < 0)
    {
        fprintf(stderr, "Pocet tisknutych radku musi byt kladny!\n");
        return -1;
    }
    if(fileIndex == -1)
        return tail_std(n); //Cteni z stdin
    return tail_file(argv[fileIndex], n); //Cteni ze souboru, jehoz jmeno bylo zadane argumentem
}
