#	Symbol table example program
#	Author: Patrik Goldschmidt

NAME=safe_malloc
CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -g
LDFLAGS=
SRCDIR=../../src/

all: $(NAME)_test.c $(SRCDIR)$(NAME).c $(SRCDIR)$(NAME).h
	$(CC) $(CFLAGS) -o $(NAME)_example $(SRCDIR)$(NAME).c \
	$(SRCDIR)$(NAME).h $(NAME)_test.c $(LDFLAGS)

clean:
	rm -rf $(NAME)_example
