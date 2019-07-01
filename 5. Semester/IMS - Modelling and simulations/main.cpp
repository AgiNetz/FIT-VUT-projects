/**
 * \file main.cpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 05.12.2018
 * \brief Simulation C++/SIMLIB program for IMS project at FIT BUT, Czech
 * republic.
 */

#include "experiment.hpp"
#include <iostream>

int main(int argc, char **argv) {
   if(argc == 2)
   {
      Experiment exp(argv[1]);
      exp.Run();
   }

   return EXIT_SUCCESS;
}