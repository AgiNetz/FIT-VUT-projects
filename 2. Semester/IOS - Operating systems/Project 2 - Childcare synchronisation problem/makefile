CFLAGS = -std=gnu99 -Wall -Wextra -Werror -pedantic
OBJ = adult.o adultGenerator.o child.o childGenerator.o proj2.o
PROGRAMS = proj2

all: $(PROGRAMS)

proj2: $(OBJ)
	gcc $(CFLAGS) -lrt -o $@ $(OBJ)

adult.o: adult.c childcare.h
adultGenerator.o: adultGenerator.c childcare.h
child.o: child.c childcare.h
childGenerator.o: childGenerator.c childcare.h
proj2.o: proj2.c childcare.h

.phony: clean
clean:
	rm $(OBJ) $(PROGRAMS)

