// eratosthenes.c
// Reseni IJC-DU1, priklad a), 23.3.2017
// Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
// Pøeloženo: gcc 5.4.0
// Popis: Definice funkce na nalezeni prvocisel do N s pouzitim bitoveho pole

#include <math.h>
#include "eratosthenes.h"

void Eratosthenes(bit_array_t sito)
{
    ba_set_bit(sito, 0, 1);
    ba_set_bit(sito, 1, 1);
    for(int i = 2; i <= sqrt(ba_size(sito)); i++)
    {
        if(ba_get_bit(sito, i))
            continue;
        int n = 2;
        while((unsigned int)n*i < ba_size(sito))
        {
            ba_set_bit(sito, n*i, 1);
            n++;
        }
    }
}

