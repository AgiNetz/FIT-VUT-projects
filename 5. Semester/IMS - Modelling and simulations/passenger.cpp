/**
 * \file passenger.cpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Passenger process
 */

#include "passenger.hpp"
#include "sim_settings.hpp"
#include <cstdio>

class Timeout : public Event {
   Process *Id;
public:
   Timeout(Process *p, double dt): Id(p) {
      Activate(Time+dt); 
   }

   void Behavior() {
      for(auto it = SimSettings::sim->taxi_queue.begin(); it != SimSettings::sim->taxi_queue.end();)
      {
         auto tmp = ++it;
         it--;
         Process *item = reinterpret_cast<Process*>(SimSettings::sim->taxi_queue.Get(it));
         if(item == Id)
         {
            break;
         }
         SimSettings::sim->taxi_queue.PredIns(item, tmp);
         it = tmp;
      }
      Id->Cancel();
      Cancel();
   }
};

void Passenger::Behavior()
{
   double taxi_chance = SimSettings::TAKE_TAXI_GOOD_WEATHER_CHANCE;
   double queue_chance = SimSettings::QUEUE_WAIT_GOOD_WEATHER_CHANCE;
   double queue_wait_mean = SimSettings::QUEUE_WAIT_TIME_GOOD_WEATHER;

   Store *taxi_taken_from = nullptr;

   if(SimSettings::sim->weather() == WeatherType::WEATHER_BAD)
   {
      taxi_chance = SimSettings::TAKE_TAXI_BAD_WEATHER_CHANCE;
      queue_chance = SimSettings::QUEUE_WAIT_BAD_WEATHER_CHANCE;
      queue_wait_mean = SimSettings::QUEUE_WAIT_TIME_BAD_WEATHER;
   }

   /* Does not take taxi if true */
   if(Random() > taxi_chance)
   {
      //Leave system
      return;
   }

   SimSettings::sim->passengers_want_taxi(1);


   if(Random() <= SimSettings::TAXI_PARKING_LOT_1_CHANCE)
   {
      taxi_taken_from = TakeTaxi(&(SimSettings::sim->competition_pl1), &(SimSettings::sim->our_pl1), &(SimSettings::sim->competition_pl2), &(SimSettings::sim->our_pl2));
   }
   else
   {
      taxi_taken_from = TakeTaxi(&(SimSettings::sim->competition_pl2), &(SimSettings::sim->our_pl2), &(SimSettings::sim->competition_pl1), &(SimSettings::sim->our_pl1));
   }

   if(taxi_taken_from == nullptr)
   {
      if(Random() > queue_chance) return;
      Timeout *t = new Timeout(this, Exponential(queue_wait_mean));
      SimSettings::sim->taxi_queue.InsLast(this);
      SimSettings::sim->queue_sizes(SimSettings::sim->taxi_queue.Length());
      Passivate();
      delete t;
      taxi_taken_from = TakeTaxi(&(SimSettings::sim->competition_pl1), &(SimSettings::sim->our_pl1), &(SimSettings::sim->competition_pl2), &(SimSettings::sim->our_pl2));
      if(taxi_taken_from == nullptr) return;
   }

   SimSettings::sim->passengers_took_taxi(1);
   double ride_start_time = Time;
   if(Random() <= SimSettings::RIDE_CITY_CHANCE)
   {
      auto delay = SimSettings::sim->time_manager.GetCurrentCityRideDelay();
      Wait(Uniform(delay-2, delay+2));
      if(taxi_taken_from == SimSettings::sim->our_pl1 || taxi_taken_from == SimSettings::sim->our_pl2)
      {
         SimSettings::sim->our_passengers_city(1);
         double ride_end_time = Time;
         SimSettings::sim->driven_minutes(ride_end_time-ride_start_time);
      }
   }
   else
   {
      auto delay = SimSettings::sim->time_manager.GetCurrentOutskirtsRideDelay();
      Wait(Uniform(delay-2, delay+2));
      if(taxi_taken_from == SimSettings::sim->our_pl1 || taxi_taken_from == SimSettings::sim->our_pl2)
      {
         SimSettings::sim->our_passengers_outskirts(1);
         double ride_end_time = Time;
         SimSettings::sim->driven_minutes(ride_end_time-ride_start_time);
      }
   }

   taxi_taken_from->Leave(1);
   if(!(SimSettings::sim->taxi_queue.Empty()))
      SimSettings::sim->taxi_queue.GetFirst()->Activate();

   return;
}


Store *Passenger::TakeTaxi(Store *first_competition, Store *first_ours, Store *second_competition, Store *second_ours)
{
   Store *taken_from = nullptr;

   if((taken_from = this->TakeTaxiFromParkingLot(first_competition, first_ours))) { return taken_from; }
   return this->TakeTaxiFromParkingLot(second_competition, second_ours);
}

Store *Passenger::TakeTaxiFromParkingLot(Store *competition, Store* ours)
{
   double chance_of_our_taxi = 0.0;
   if(ours->Free())
      chance_of_our_taxi = ours->Free()/(competition->Free() + ours->Free());

   if(Random() > chance_of_our_taxi)
   {
      if(this->AttemptSeize(competition)) { return competition; }
      if(this->AttemptSeize(ours)) { return ours; }
   }
   else
   {
      if(this->AttemptSeize(ours)) { return ours; }
      if(this->AttemptSeize(competition)) { return competition; }
   }

   return nullptr;
}

bool Passenger::AttemptSeize(Store *store)
{
   if(store->Free())
   {
      Enter(*store, 1);
      return true;
   }
   return false;
}

