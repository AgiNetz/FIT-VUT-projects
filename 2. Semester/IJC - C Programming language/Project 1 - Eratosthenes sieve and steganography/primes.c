// primes.c
// Reseni IJC-DU1, priklad a), 24.3.2017
// Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
// Pøeloženo: gcc 5.4.0
// Popis: Hlavni program, ucel je nalezeni poslednich 10 prvocisel v rozmezi od 0 do 303000000 pomoci Eratosthenova sita

#include <stdio.h>
#include "eratosthenes.h"

int main()
{
    ba_create(primes, 303000000);
    Eratosthenes(primes);
    int counter = 10;
    unsigned long toPrint[10];
    for(int i = ba_size(primes) - 1; i > 1 && counter > 0; i--)
    {
        if(ba_get_bit(primes, i) == 0)
        {
            counter--;
            toPrint[counter] = i;
        }
    }
    for(;counter < 10; counter++)
        printf("%lu\n", toPrint[counter]);
    return 0;
}
