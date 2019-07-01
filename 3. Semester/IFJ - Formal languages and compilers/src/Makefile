#	Makefile for IFJ17 project, group 006
#	Author: Danis Tomas			- xdanis05@stud.fit.vutbr.cz
#	Author: Goldschmidt Patrik	- xgolds00@stud.fit.vutbr.cz
#	Date:	24.11.2017

#Make instructions:
#	-	make all		- Compiles current version of the IFJ17 project
#	-	make debug	- Compiles debug version of IFJ17 project
#	-	make clean	- Cleans all object and executable files created during 
#						  compilation process
#	- make pack		- Packs all source files and misc files into the archive.
#						  This option is only available using git file structure.

########################    Variables declaration    ##########################

PROJ=ifj17_grp006
CC=gcc
CFLAGS=-std=c99 -pedantic -Wall -Wextra -Werror
LDFLAGS=
DEBUG=-g

SRCFILES=$(CFILES) $(HFILES) Makefile
CFILES=error.c expr.c generator.c ilist.c main.c parser.c safe_malloc.c 		 \
		 scanner.c str.c symtable.c tokenstack.c vdstack.c 
HFILES=error.h expr.h generator.h ifj17_api.h ilist.h parser.h safe_malloc.h   \
		 scanner.h str.h symtable.h tokenstack.h vdstack.h 
OBJFILES=error.o expr.o generator.o ilist.o main.o parser.o safe_malloc.o 		 \
			scanner.o str.o symtable.o tokenstack.o vdstack.o 
MISCFILES=rozdeleni rozsireni
DOCSFILES=dokumentace.pdf

SRCDIR=../src/
MISCDIR=../misc/
DOCSDIR=../docs/
TESTDIR=../testy/
TEST_NORMAL_DIR=tests_normal/
TEST_ERROR_DIR=tests_error/

LOGIN=xgolds00
NORMALTESTS=normaltests.sh
ERRORTESTS=errortests.sh

########################    Debug flag conditional    #########################

ifeq ($(MAKECMDGOALS),debug)
	CFLAGS += $(DEBUG)
endif

#############################    Main targets    ##############################

all: $(PROJ)

.PHONY: all clean debug pack test

$(PROJ): $(OBJFILES)
	$(CC) $(CFLAGS) $^ -o $(PROJ)

#########################    Object files targets    ##########################

error.o: error.c error.h ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

expr.o: expr.c expr.h ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

generator.o: generator.c generator.h ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

ilist.o: ilist.c ilist.h ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

main.o: main.c ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

parser.o: parser.c parser.h ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

safe_malloc.o: safe_malloc.c safe_malloc.h
	$(CC) $(CFLAGS) -o $@ -c $<

scanner.o: scanner.c scanner.h ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

str.o: str.c str.h ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

symtable.o: symtable.c symtable.h ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

tokenstack.o: tokenstack.c tokenstack.h ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

vdstack.o: vdstack.c vdstack.h ifj17_api.h
	$(CC) $(CFLAGS) -o $@ -c $<

#############################    Other targets    #############################

clean:
	@rm -rf $(OBJFILES) $(PROJ) $(LOGIN).tgz
	@echo "Contents successfully cleared."

debug: $(PROJ)

pack: $(SRCFILES)
	@tar -cf $(LOGIN).tar $(SRCFILES) && cd $(MISCDIR) && 				\
	tar -rf $(SRCDIR)$(LOGIN).tar $(MISCFILES) && cd $(DOCSDIR) && 	\
	tar -rf $(SRCDIR)$(LOGIN).tar $(DOCSFILES) && cd $(SRCDIR) && 		\
	gzip $(LOGIN).tar && mv $(LOGIN).tar.gz $(LOGIN).tgz && 				\
	echo "Source files successfully packed into $(LOGIN).tgz"

test: $(TESTDIR)$(NORMALTESTS)
	$(TESTDIR)./$(NORMALTESTS)

teste: $(TESTDIR)$(ERRORTESTS)
	$(TESTDIR)./$(ERRORTESTS)

testclean:
	@rm -rf $(TESTDIR)$(TEST_NORMAL_DIR)*_ifjcode.txt 	\
			  $(TESTDIR)$(TEST_NORMAL_DIR)*_user.txt		\
			  $(TESTDIR)$(TEST_ERROR_DIR)*_ifjcode.txt
	@echo "Test files successfully cleared."