// bit_array.h
// Reseni IJC-DU1, priklad a), 23.3.2017
// Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
// Pøeloženo: gcc 5.4.0
// Popis: Definice typu bitove pole, maker a inline funkcii potrebnych pro praci s nim

#ifndef BIT_ARRAY_H_INCLUDED
#define BIT_ARRAY_H_INCLUDED


#include <limits.h>
#include "error.h"

typedef unsigned long bit_array_t[];

#define BA_GET_BIT_(ar, i) ((((ar)[1 + ((i)/(CHAR_BIT*sizeof((ar)[0])))] & ((1lu << (i)%(CHAR_BIT*sizeof((ar)[0])))))) >> ((i)%(CHAR_BIT*sizeof((ar)[0]))))

#define BA_SET_BIT_(ar, i, b) (b) ? ((ar)[1 + ((i)/(CHAR_BIT*sizeof((ar)[0])))] |= (1lu << (i)%(CHAR_BIT*sizeof((ar)[0])))) : \
((ar)[1lu + ((i)/(CHAR_BIT*sizeof((ar)[0])))] &= ~(1lu << (i)%(CHAR_BIT*sizeof((ar)[0]))))

#define ba_create(name, size) unsigned long name[1 +(((size)+CHAR_BIT*sizeof(unsigned long) - 1)/(CHAR_BIT*sizeof(unsigned long)))] = {size};

#ifndef USE_INLINE

#define ba_size(name) (name)[0]

#define ba_set_bit(name, i, b) ((((unsigned long)(i) < ba_size((name))) && ((i) >= 0)) ? (BA_SET_BIT_((name), i, b)) : (error_msg("Index %lu mimo rozsah 0..%lu", (unsigned long)i, ba_size((name))),0))

#define ba_get_bit(name, i) ((((unsigned long)(i) < ba_size((name))) && ((i) >= 0)) ? (BA_GET_BIT_((name), i)) : (error_msg("Index %lu mimo rozsah 0..%lu", (unsigned long)i, ba_size((name))),0))


#else

static inline unsigned long ba_size(bit_array_t arr)
{
    return arr[0];
}

static inline void  ba_set_bit(bit_array_t arr, int i, int b)
{
    ((((unsigned long)(i) < ba_size((arr))) && ((i) >= 0)) ? (BA_SET_BIT_((arr), i, b)) : (error_msg("Index %d mimo pole rozsahu 0..%d", i, ba_size((arr))),0));
}

static inline unsigned long ba_get_bit(bit_array_t arr, int i)
{
    return ((((unsigned long)(i) < ba_size((arr))) && ((i) >= 0)) ? (BA_GET_BIT_((arr), i)) : (error_msg("Index %d mimo pole rozsahu 0..%d", i, ba_size((arr))),0));
}
#endif // USE_INLINE

#endif // BIT_ARRAY_H_INCLUDED
