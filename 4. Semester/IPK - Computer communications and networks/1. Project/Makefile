CC = gcc
CFLAGS = -Werror -Wextra -Wall -pedantic -std=gnu99
PROGRAMS = ipk-client ipk-server

all: $(PROGRAMS)

ipk-client: ipk-client.c tump.h
	$(CC) $(CFLAGS) $< -o $@

ipk-server: ipk-server.c tump.h
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean
clean:
	rm $(PROGRAMS)