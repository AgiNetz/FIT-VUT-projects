# Nazev: makefile
# Reseni IJC-DU2, uloha 2, 24.4.2017
# Autor: Tomas Danis, 1r FIT, xdanis05@stud.fit.vutbr.cz
# Popis: Makefile pro programy tail, tail2, wordcount a wordcount-dynamic. Taktez vytvori knihovny libhtab.a a libhtab.so.

CFLAGS = -std=c99 -pedantic -Wall -Wextra -Werror -fPIC -O2
CXXFLAGS = -std=c++11 -pedantic -Wall -O2
LIBOBJ = htab_init.o htab_move.o htab_foreach.o htab_size.o htab_bucket_count.o htab_clear.o htab_free.o htab_remove.o htab_find.o htab_lookup_add.o hash_function.o
NONLIBOBJ = wordcount.o io.o
CC = gcc
XX = g++
STATIC = libhtab.a $(NONLIBOBJ)
DYNAMIC = libhtab.so $(NONLIBOBJ)
PROGRAMS = tail tail2 wordcount wordcount-dynamic libhtab.a libhtab.so

all: $(PROGRAMS)
tail: tail.c
	$(CC) $(CFLAGS) -o $@ $<
tail2: tail2.cc
	$(XX) $(CXXFLAGS) -o $@ $<
libhtab.a: $(LIBOBJ)
	ar rcs $@ $(LIBOBJ)
	ranlib $@
libhtab.so: $(LIBOBJ)
	$(CC) -shared $(CFLAGS) -o $@ $(LIBOBJ)
wordcount: $(STATIC)
	$(CC) -static $(CFLAGS) -o $@ $(NONLIBOBJ) -L. -lhtab
wordcount-dynamic: $(DYNAMIC)
	$(CC) $(CFLAGS) -o $@ $(NONLIBOBJ) -L. -lhtab
	

io.o: io.c io.h htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
wordcount.o: wordcount.c io.h htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
htab_init.o: htab_init.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
htab_move.o: htab_move.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
htab_size.o: htab_size.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
htab_bucket_count.o: htab_bucket_count.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
htab_lookup_add.o: htab_lookup_add.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
htab_find.o: htab_find.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
htab_foreach.o: htab_foreach.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
htab_remove.o: htab_remove.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
htab_clear.o: htab_clear.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
htab_free.o: htab_free.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<
hash_function.o: hash_function.c htab.h
	$(CC) $(CFLAGS) -o $@ -c $<

.phony: clean all
clean:
	rm -f $(PROGRAMS) $(LIBOBJ) $(NONLIBOBJ) 
