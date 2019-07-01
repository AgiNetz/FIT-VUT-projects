/**
 * \file simulation.cpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Simulation contents definition.
 */

#include "day_weather_generator.hpp"
#include "simulation.hpp"
#include "sim_settings.hpp"
#include "weather.hpp"
#include "time_manager.hpp"
#include <ctime>
#include <simlib.h>
#include <iostream>


Simulation::Simulation() :
   taxi_queue("Waiting line for taxis"),
   competition_pl1("Competition taxis, parking lot 1", SimSettings::COMPETITION_PARKING_LOT_1_CAPACITY),
   competition_pl2("Competition taxis, parking lot 2", SimSettings::COMPETITION_PARKING_LOT_2_CAPACITY),
   our_pl1("Our taxis, parking lot 1", SimSettings::OUR_PARKING_LOT_1_CAPACITY),
   our_pl2("Our taxis, parking lot 2", SimSettings::OUR_PARKING_LOT_2_CAPACITY),
   time_manager(),
   passengers_want_taxi("Potential passengers"),
   passengers_took_taxi("Served passengers"),
   our_passengers_city("Passengers served by us (City)"),
   our_passengers_outskirts("Passengers served by us (Outskirts)"),
   driven_minutes("Minutes spent driving"),
   queue_sizes("Taxi Queue", 0, 5, 20),
   gener{new RegularTrainPassengerGenerator(),
         new ExpressTrainPassengerGenerator(),
         new DayWeatherGenerator(this)},
   model_weather(WeatherType::WEATHER_GOOD)
   {
      SimSettings::sim = this;
}

Simulation::~Simulation() {
   delete gener.rtrain;
   delete gener.etrain;
   delete gener.weather;
}

void Simulation::dumpStats() {
   this->our_pl1.Output();
   this->our_pl2.Output();
   this->competition_pl1.Output();
   this->competition_pl2.Output();
   this->passengers_want_taxi.Output();
   this->passengers_took_taxi.Output();
   this->our_passengers_city.Output();
   this->our_passengers_outskirts.Output();
   this->queue_sizes.Output();
}

void Simulation::init(int start, int end) {
   Init(start, end);
   RandomSeed(time(nullptr));
   time_manager.InitShifts();
   this->gener.rtrain->Activate();
   this->gener.etrain->Activate(
      SimSettings::express_trains[0].arrival +
      ExpressTrainPassengerGenerator::delay());
   this->gener.weather->Activate();
   time_manager.Activate();
}

void Simulation::setOutputFile(std::string file) {
   SetOutput(file.c_str());
}

void Simulation::start() {
   Run();
}

void Simulation::stop() {
   Stop();
}

void Simulation::setWeather(WeatherType status) {
   this->model_weather = status;
}

WeatherType Simulation::weather() {
   return this->model_weather;
}