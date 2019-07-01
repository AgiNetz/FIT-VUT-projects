# makefile
# Reseni IJC-DU1, priklad b), 24.3.2017
# Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
# Pøeloženo: gcc 5.4.0
# Popis: Makefile pro programy primes, primes-i a steg-decode

objects = primes.o eratosthenes.o error.o
objects-i = primes-i.o eratosthenes-i.o error.o
CFLAGS = -O2 -std=c99 -Wall -pedantic

primes: $(objects) ppm.o steg-decode.o primes-i.o eratosthenes-i.o
	cc -o primes $(objects) -lm $(CFLAGS)
	cc -o primes-i $(objects-i) -lm $(CFLAGS) -DUSE_INLINE
	cc -o steg-decode steg-decode.o ppm.o error.o eratosthenes.o -lm $(CFLAGS)

primes.o: primes.c bit_array.h eratosthenes.h error.h
	cc $(CFLAGS) -c primes.c
primes-i.o: primes.c bit_array.h eratosthenes.h error.h
	cc $(CFLAGS) -c primes.c -o primes-i.o -DUSE_INLINE
eratosthenes.o: eratosthenes.c eratosthenes.h bit_array.h error.h
	cc $(CFLAGS) -c eratosthenes.c -lm
eratosthenes-i.o: eratosthenes.c eratosthenes.h bit_array.h error.h
	cc $(CFLAGS) -c eratosthenes.c -o eratosthenes-i.o  -lm -DUSE_INLINE
error.o: error.c
	cc $(CFLAGS) -c error.c
ppm.o: ppm.c error.h ppm.h
	cc $(CFLAGS) -c ppm.c
steg-decode.o: steg-decode.c error.h eratosthenes.h ppm.h bit_array.h steg-decode.h
	cc $(CFLAGS) -c steg-decode.c

.phony: clean run
clean:
	rm primes primes-i steg-decode $(objects) ppm.o steg-decode.o primes-i.o eratosthenes-i.o
run:
	time ./primes;
	time ./primes-i;
