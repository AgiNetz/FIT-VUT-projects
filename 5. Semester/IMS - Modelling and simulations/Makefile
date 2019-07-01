# Author:       Patrik Goldschmidt <xgolds00 AT stud.fit.vutbr.cz>
# Title:        IMS 1. project - Traffic problem simulation
# Subject:      IMS
# Organization: Brno University of Technology, Czech Republic
# Date:         12-07-2018

##################################  USAGE:  #################################
# make          Compiles project's binary files.
# make debug    Compiles project's debug version.
# make pack     Packs source files and headers into the tar.gz archive
# make zip      Packs source files and headers into the .zip archive
# make clean    Cleans object files and binaries

NAME=ims_proj
ARCH_NAME=02_xgolds00_xdanis05
EXPERIMENT_FOLDER=experiments
EXPERIMENT_FILENAME=exp
FILES=main.cpp simulation.cpp simulation.hpp passenger_generator.hpp 		\
		regular_train_passgen.cpp regular_train_passgen.hpp sim_settings.cpp \
		sim_settings.hpp express_train_passgen.cpp express_train_passgen.hpp \
		weather.hpp day_weather_generator.cpp day_weather_generator.hpp \
		passenger.cpp passenger.hpp shift_change.hpp shift_change.cpp \
		time_manager.cpp time_manager.hpp experiment.hpp experiment.cpp		\
		$(EXPERIMENT_FOLDER)/exp*
OBJFILES=main.o sim_settings.o simulation.o regular_train_passgen.o        \
			express_train_passgen.o day_weather_generator.o passenger.o \
			shift_change.o time_manager.o experiment.o
DEBUG=-g -DDEBUG
CXX=g++
CXXFLAGS= -std=c++14 -Wall -Wextra -pedantic -O2 -D_FORTIFY_SOURCE=2 \
			 -D_GLIBCXX_ASSERTIONS
LDFLAGS+=-lsimlib -lm


$(NAME): $(OBJFILES)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

debug: CXXFLAGS += $(DEBUG)
debug: $(NAME)

.PHONY: debug run clean pack zip

run: $(NAME)
	./$(NAME) $(EXPERIMENT_FILENAME)1.dat
	@cat $(EXPERIMENT_FILENAME)1.out

clean:
	rm -f $(NAME) $(OBJFILES) $(EXPERIMENT_FILENAME)*.out $(ARCH_NAME).tar.gz \
	$(ARCH_NAME).zip

pack: $(FILES) Makefile
	tar -czf $(ARCH_NAME).tar.gz $(FILES)

zip: $(FILES) Makefile
	zip $(ARCH_NAME).zip $(FILES)

main.o: main.cpp experiment.hpp

sim_settings.o: sim_settings.cpp sim_settings.hpp

simulation.o: simulation.cpp simulation.hpp weather.hpp

regular_train_passgen.o: regular_train_passgen.cpp regular_train_passgen.hpp \
								 passenger_generator.hpp passenger.hpp

express_train_passgen.o: express_train_passgen.cpp express_train_passgen.hpp \
								 passenger_generator.hpp passenger.hpp

day_weather_generator.o: day_weather_generator.cpp day_weather_generator.hpp \
								 weather.hpp

passenger.o: passenger.cpp passenger.hpp sim_settings.hpp simulation.hpp

shift_change.o: shift_change.cpp shift_change.hpp

time_manager.o: time_manager.cpp time_manager.hpp shift_change.hpp sim_settings.hpp simulation.hpp

experiment.o: experiment.cpp experiment.hpp simulation.hpp sim_settings.hpp