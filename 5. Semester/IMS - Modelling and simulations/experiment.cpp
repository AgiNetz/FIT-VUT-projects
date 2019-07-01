/**
 * \file experiment.cpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Experiment class
 */

#include "experiment.hpp"
#include "sim_settings.hpp"

Experiment::Experiment(std::string params)
{
   std::ifstream config (params.c_str(), std::ifstream::in);
   std::string field_name;

   /* Load output file */
   std::string output_file;
   config >> field_name >> output_file;
   this->output_file = output_file;

   /* Load simulation length */
   int simulation_days;
   config >> field_name >> simulation_days;
   this->simulation_minutes = simulation_days * SimSettings::DAY_MINUTES;

   /* Load parking lot 1 shifts */
   config >> field_name;
   for(int i = 0; i < 6; i++)
   {
      config >> SimSettings::OUR_PARKING_LOT_1_TAXIS[i];
   }

   /* Load parking lot 2 shifts */
   config >> field_name;
   for(int i = 0; i < 6; i++)
   {
      config >> SimSettings::OUR_PARKING_LOT_2_TAXIS[i];
   }

   config >> field_name >> this->taxis_number;

   config.close();

}
Experiment::~Experiment() {}

void Experiment::Run()
{
   Simulation sim;

   sim.init(0, this->simulation_minutes);
   sim.setOutputFile(this->output_file.c_str());
   sim.start();
   sim.dumpStats();
   this->EvaluateModel(&sim);
}

int Experiment::CalculateWorkingHours(int pl1_schedule[], int pl2_schedule[])
{
   int shift_change_times[] = {
      SimSettings::DAY_EARLY_BIRD_START, 
      SimSettings::DAY_MORNING_PEAK_START,
      SimSettings::DAY_DAYTIME_ATTENUATION_START,
      SimSettings::DAY_EVENING_PEAK_START,
      SimSettings::DAY_LATE_EVENENING_ATTENUATION_START,
      SimSettings::DAY_NIGHT_STAGNATION_START
   };

   int working_hours = 0;
   for(int i = 0; i < 5; i++)
   {
      working_hours += (shift_change_times[i+1] - shift_change_times[i])*pl1_schedule[i];
      working_hours += (shift_change_times[i+1] - shift_change_times[i])*pl2_schedule[i];
   }

   working_hours += (shift_change_times[0] + SimSettings::DAY_MINUTES - shift_change_times[5])*pl1_schedule[5];
   working_hours += (shift_change_times[0] + SimSettings::DAY_MINUTES - shift_change_times[5])*pl2_schedule[5];
   return (working_hours+59)/60;
}

void Experiment::EvaluateModel(Simulation *sim)
{
   /* Income from rides */
   double income = sim->our_passengers_city.Number()*CITY_FARE + sim->our_passengers_outskirts.Number()*OUTSKIRTS_FARE;
   std::cout << "Income from rides: " << income << "€" << std::endl << std::endl;

   /* Working hours pay */
   double costs = CalculateWorkingHours(SimSettings::OUR_PARKING_LOT_1_TAXIS, SimSettings::OUR_PARKING_LOT_2_TAXIS)*HOURLY_RATE*(
      this->simulation_minutes/SimSettings::DAY_MINUTES);
   std::cout << "Driver pay: " << costs << "€" << std::endl;

   /* Fuel costs */
   int fuel = SimSettings::sim->driven_minutes.Sum()*KM_PER_MINUTE*FUEL_CONSUMPTION_PER_100KM*FUEL_PRICE/100;
   std::cout << "Fuel costs: " << fuel << "€" << std::endl;
   costs += fuel;

   int maintenance = this->taxis_number * MAINTENANCE_COST_PER_YEAR * this->simulation_minutes/(SimSettings::DAY_MINUTES*365);
   std::cout << "Car maintenance costs: " << maintenance << "€" << std::endl;
   costs += maintenance;

   int pl1_cars = 0;
   for(int i = 0; i < 6; i++)
   {
      if(SimSettings::OUR_PARKING_LOT_1_TAXIS[i] > pl1_cars)
         pl1_cars = SimSettings::OUR_PARKING_LOT_1_TAXIS[i];
   }

   int pl1_fee = pl1_cars * PL1_FEE_PER_MONTH * this->simulation_minutes*12/(SimSettings::DAY_MINUTES*365);
   std::cout << "Parking lot 1 fees: " << pl1_fee << "€" << std::endl << std::endl;
   costs += pl1_fee;

   std::cout << "Total income: " << income << "€" << std::endl;
   std::cout << "Costs and fees: " << costs << "€" << std::endl << std::endl;
   std::cout << "Profit: " << income - costs << "€" << std::endl;
}