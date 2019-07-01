#	Template Makefile for IFJ project 2017
#	Author: Patrik Goldschmidt

NAME=template
DEPS=$(NAME).c $(NAME).h $(NAME)_test.c
DEBUG_FLAG=-g
CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra
LDFLAGS=

$(NAME): $(DEPS)
	$(CC) $(CFLAGS) -o $(NAME) $(DEPS) $(LDFLAGS)

debug: $(DEPS)
	$(CC) $(CFLAGS) $(DEBUG_FLAG) -o $(NAME) $(DEPS) $(LDFLAGS)

.PHONY: clean debug pack

clean:
	rm -rf $(NAME) $(NAME).tar.gz

pack: $(DEPS) Makefile
	tar -czf $(NAME).tar.gz $(DEPS) Makefile
