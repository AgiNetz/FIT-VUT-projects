CC = g++
CFLAGS = -Werror -Wextra -Wall -pedantic -std=c++11 -static-libstdc++
PROGRAMS = ipk-mtrip
O_FILES = ipk-mtrip.o prober.o evaluator.o
LINK.o = $(LINK.cpp)

all: $(PROGRAMS)

ipk-mtrip: $(O_FILES)
	$(CC) $(CFLAGS) $(O_FILES) -o $@

ipk-mtrip.o: ipk-mtrip.cpp prober.hpp evaluator.hpp api.hpp
	$(CC) $(CFLAGS) -c $< -o $@

prober.o: prober.cpp prober.hpp api.hpp
	$(CC) $(CFLAGS) -c $< -o $@

evaluator.o: evaluator.cpp evaluator.hpp api.hpp
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm $(PROGRAMS) $(O_FILES)
