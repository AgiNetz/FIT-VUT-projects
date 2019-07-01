/**
 * \file experiment.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Experiment class
 */

#ifndef EXPERIMENT_HPP
#define EXPERIMENT_HPP

#include "simulation.hpp"
#include <fstream>
#include <string>

const double KM_PER_MINUTE = 0.46;
const double HOURLY_RATE = 5;
const double FUEL_CONSUMPTION_PER_100KM = 4.6;
const double FUEL_PRICE = 1.292;
const double MAINTENANCE_COST_PER_YEAR = 710;
const double PL1_FEE_PER_MONTH = 400;
const double CITY_FARE = 1.6;
const double OUTSKIRTS_FARE = 2.4; 

class Experiment
{
public:
   Experiment(std::string experiment_params);
   ~Experiment();

   void Run();
   int CalculateWorkingHours(int pl1_schedule[], int pl2_schedule[]);
private:
   std::string output_file;
   int simulation_minutes;
   int taxis_number;
   void EvaluateModel(Simulation *sim);
};

#endif