/**
 * \file day_weather_generator.cpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 08.12.2018
 * \brief Generator for generating variable weather definition.
 */

#include "day_weather_generator.hpp"
#include "sim_settings.hpp"
#include "simulation.hpp"
#include "weather.hpp"
#include <simlib.h>

DayWeatherGenerator::DayWeatherGenerator(void *simulation_obj) :
   simobj(simulation_obj) {

}

void DayWeatherGenerator::Behavior() {
   /* Generate new weather and update simulation object. */
   double randval = Random();       /* Pseudo-random to determine weather. */
   Simulation *sim = ((reinterpret_cast<Simulation *> (this->simobj)));

   /* Determine the weather and set it in simulation object. */
   if(randval <= SimSettings::WEATHER_BAD_CHANCE) {
      sim->setWeather(
         WeatherType::WEATHER_BAD);
   } else {
      sim->setWeather(
         WeatherType::WEATHER_GOOD);
   }

   /* Schedule generator again in ~ 24 hours. */
   this->Activate(Time + Exponential(SimSettings::DAY_MINUTES));
}