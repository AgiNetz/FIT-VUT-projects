// error.h
// Reseni IJC-DU1, priklad b), 24.3.2017
// Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
// Pøeloženo: gcc 5.4.0
// Popis: Hlavickovy soubor pro modul ppm.c obsahujici prototypy funkcii, definice typu a maximalni velikosti souborus

#ifndef PPM_H_INCLUDED
#define PPM_H_INCLUDED

#define MAX_SIZE 3000000

struct ppm {
    unsigned xsize;
    unsigned ysize;
    char data[];
};

struct ppm * ppm_read(const char *filename);
int ppm_write(struct ppm *p, const char * filename);


#endif // PPM_H_INCLUDED
