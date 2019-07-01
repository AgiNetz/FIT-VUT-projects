// steg-decode.c
// Reseni IJC-DU1, priklad b), 24.3.2017
// Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
// Pøeloženo: gcc 5.4.0
// Popis: Hlavni program, ktereho ucel je najit v ppm souboru zakodovanou zpravu metodou podobnou steganografii

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "steg-decode.h"
#include "error.h"
#include "eratosthenes.h"
#include "bit_array.h"

void decode(struct ppm *p)
{
    ba_create(key, MAX_SIZE);
    unsigned long size = 3*8*p->xsize*p->ysize;
    Eratosthenes(key);
    int c = 0;
    int pos = 0;
    for(long i = 2; i < size; i++)
    {
        if(ba_get_bit(key, i) == 0)
        {
            c += ((BA_GET_BIT_(&(p->data[i-1]), 0)) << pos);
            pos++;
            if(pos == CHAR_BIT)
            {
                if(c == 0)
                {
                    fputc(c, stdout);
                    return;
                }
		if(isprint(c))
                    fputc(c, stdout);
		else
		    error_msg("Zprava obsahuje netisknutelne znaky!\n");	
                c = 0;
                pos = 0;
            }
        }
    }
    if(c != 0 || pos != 0)
        error_msg("Zprava neni spravne zakodovana!\n");
}

int main(int argc, char *argv[])
{
    if(argc != 2)
        error_msg("Ocekavany argument je jeden soubor typu P6 PPM!\n");
    struct ppm *p = ppm_read(argv[1]);
    if(p == NULL)
        error_msg("Nastala chyba pri nacitavani obrazku!\n");
    decode(p);
    free(p);
    p = NULL;
    return 0;
}
