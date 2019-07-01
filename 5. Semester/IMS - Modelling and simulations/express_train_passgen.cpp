/**
 * \file main_train_passgen.cpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Main (popular) trains passenger generator.
 */

#include "express_train_passgen.hpp"
#include "sim_settings.hpp"
#include "passenger.hpp"
#include <simlib.h>

ExpressTrainPassengerGenerator::ExpressTrainPassengerGenerator() :
   trainnum(0) {

}

void ExpressTrainPassengerGenerator::Behavior() {
   int passengers = this->determinePassengersNum();

   for(int i = 0; i < passengers; i++) {
      (new Passenger)->Activate();
   }

   /* Update number of the train. */
   this->trainnum++;

   /* Schedule next generation event. */
   this->Activate(this->determineNextTrain());

   if(this->trainnum == SimSettings::EXTRAIN_DAILY_NUM) {
       this->trainnum = 0;
   }
}

int ExpressTrainPassengerGenerator::delay() {
   int delay;                    /* Delay to be returned. */
   double randval = Random();    /* Random to determine delay. */

   if (randval < SimSettings::TRAIN_DELAY_NONE_TRESHOLD) {
         delay = 0;
   } else if (randval < SimSettings::TRAIN_DELAY_5_TRESHOLD) {
      delay = 5;
   } else if (randval < SimSettings::TRAIN_DELAY_10_TRESHOLD) {
      delay = 10;
   } else if (randval < SimSettings::TRAIN_DELAY_15_TRESHOLD) {
      delay = 15;
   } else if (randval <= SimSettings::TRAIN_DELAY_20_TRESHOLD) {
      delay = 20;
   } else if (randval <= SimSettings::TRAIN_DELAY_25_TRESHOLD) {
      delay = 25;
   } else {
      delay = 30;
   }

   return delay;
}

/* --------------------------    PRIVATE METHODS   ------------------------- */
double ExpressTrainPassengerGenerator::determineNextTrain() {
   double next_train;         /* Arrival of the next train. */
   int day_start_time = (static_cast <int> (Time / SimSettings::DAY_MINUTES)) *
                        SimSettings::DAY_MINUTES;
   int train_info_idx(this->trainnum); /* Index where train data is stored. */

   if(this->trainnum == SimSettings::EXTRAIN_DAILY_NUM) {
      train_info_idx = 0;
      day_start_time += SimSettings::DAY_MINUTES;
   }

   next_train = day_start_time +
      SimSettings::express_trains[train_info_idx].arrival + this->delay();

   /* Correction if scheduled arrival would be before current sim. time. */
   return (next_train > Time) ? next_train : Time + Exponential(5);
}

int ExpressTrainPassengerGenerator::determinePassengersNum() {
   double extrain_passengers(
      SimSettings::PASSENGERS_DAILY_AVG *
      SimSettings::PASSENGER_RATIO_EXPRESS_TRAINS *
      SimSettings::express_trains[this->trainnum].ratio);

   return Normal(extrain_passengers, extrain_passengers *
                 SimSettings::PASSENGERS_NORMAL_DISTRIB_QUOTIENT);
}
