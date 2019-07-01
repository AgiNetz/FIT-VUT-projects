// ppm.c
// Reseni IJC-DU1, priklad b), 24.3.2017
// Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
// P�elo�eno: gcc 5.4.0
// Popis: Modul slouzici na manipulaci se soubory typu ppm, specificky na jejich cteni a zapis do nich

#include <stdio.h>
#include <stdlib.h>
#include "error.h"
#include "ppm.h"

struct ppm* ppm_read(const char * filename)
{
    FILE* fhandle;
    if((fhandle = fopen(filename, "rb")) == NULL)
        goto fopenFail;
    char magicNumber[2];
    unsigned xsize;
    unsigned ysize;
    unsigned depth;
    if(fscanf(fhandle, "%2s %u %u %u ", magicNumber, &xsize, &ysize, &depth) != 4)
        goto scanFail;
    if(magicNumber[0] != 'P' || magicNumber[1] != '6')
        goto formatFail;
    if(depth != 255)
        goto depthFail;
    if(xsize * ysize * 3 > MAX_SIZE)
        goto sizeFail;
    struct ppm* p;
    if((p = malloc(sizeof(struct ppm) + 3*xsize*ysize)) == NULL)
        goto mallocFail;

    p->xsize = xsize;
    p->ysize = ysize;
    fread(p->data, sizeof(char), 3*xsize*ysize, fhandle);
    if(ferror(fhandle) != 0)
        goto readFail;
    if(feof(fhandle) != 0 || fgetc(fhandle) != EOF)
        goto consistencyFail;

    fclose(fhandle);
    return p;

fopenFail:
    warning_msg("Nepovedlo se otevřít soubor %s!\n", filename);
    goto fnopen;
scanFail:
    warning_msg("Nepovedlo se načíst údaje o obrázku!\n");
    goto fiopen;
formatFail:
    warning_msg("Nepodporovaný formát souboru!\n");
    goto fiopen;
depthFail:
    warning_msg("Nepodporovaná farební hloubka souboru!\n");
    goto fiopen;
sizeFail:
    warning_msg("Soubor je příliš velký!\n");
    goto fiopen;
mallocFail:
    warning_msg("Nepovedlo se alokovat paměť!\n");
    goto fiopen;
readFail:
    warning_msg("Nepovedlo se přečíst data ze souboru!\n");
    goto memAlloced;
consistencyFail:
    warning_msg("Velikost obrázku neodpovídá údajům v hlavičce!\n");
memAlloced:
    free(p);
fiopen:
    fclose(fhandle);
fnopen:
    return NULL;
}

int ppm_write(struct ppm *p, const char * filename)
{
    if(p == NULL)
        goto pNull;
    FILE* fhandle;
    if((fhandle = fopen(filename, "w")) == NULL)
        goto wfopenFail;
    if(fprintf(fhandle, "P6\n%u %u\n255\n", p->xsize, p->ysize) < 0)
        goto wfprintfFail;
    fwrite(p->data, sizeof(char), 3*(p->xsize)*(p->ysize), fhandle);
    if(ferror(fhandle) != 0)
        goto wfprintfFail2;

    fclose(fhandle);
    return 0;
pNull:
    warning_msg("Ukazatel na strukturu p je NULL!\n");
    goto wfnopen;
wfopenFail:
    warning_msg("Nepovedlo se otevřít soubor %s!\n", filename);
    goto wfnopen;
wfprintfFail:
    warning_msg("Nepovedlo se zapsat hlavičku souboru!\n");
    goto wfiopen;
wfprintfFail2:
    warning_msg("Nepovedlo se zapsat obsah souboru!\n");
wfiopen:
    fclose(fhandle);
wfnopen:
    return -1;
}
