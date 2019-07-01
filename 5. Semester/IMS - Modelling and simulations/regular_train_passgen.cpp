/**
 * \file regular_train_passgen.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Generator definitions and generator wrapper structure.
 */

#include "regular_train_passgen.hpp"
#include "sim_settings.hpp"
#include "passenger.hpp"
#include <simlib.h>


RegularTrainPassengerGenerator::RegularTrainPassengerGenerator() {

}

void RegularTrainPassengerGenerator::Behavior() {
   int passengers = this->determinePassengersNum();

   for(int i = 0; i < passengers; i++) {
      (new Passenger)->Activate();
   }

   /* Reschedule generator after ~ 10 simulation minutes. */
   this->Activate(Time + Exponential(SimSettings::PASSENGER_DAY_GEN_INTERVAL));
}

/* --------------------------    PRIVATE METHODS   ------------------------- */
bool RegularTrainPassengerGenerator::isInInterval(
   int val, int bottom_limit, int upper_limit) {
   if (val >= bottom_limit && val <= upper_limit) {
      return true;
   } else {
      return false;
   }
}

int RegularTrainPassengerGenerator::determinePassengersNum() {
   using namespace SimSettings;

   /* Counts how many times was the function subsequently called during the
    * "night" to deremine when to return some people.
    */
   static int night_calls(0);
   bool in_night(false);      /* True if night generation was used. */
   /* Value to be returned. */
   int people_num(0);
   /* Compute number of passed minutes in the current day. */
   int mins_passed(static_cast<const int> (Time) % DAY_MINUTES);
   /* Compute passengers average transported by regular trains. */
   int rtrains_passengers(PASSENGERS_DAILY_AVG * PASSENGER_RATIO_REG_TRAINS);

   /* Determine day interval. */
   if (isInInterval(mins_passed, DAY_EARLY_BIRD_START, DAY_EARLY_BIRD_END)) {
      people_num = rtrains_passengers * DAY_RTRAIN_RATIO_EARLY_BIRD /
                   DAY_RTRAIN_BLK_NUM_EARLY_BIRD;
   } else if (isInInterval(mins_passed, DAY_MORNING_PEAK_START,
                          DAY_MORNING_PEAK_END)) {
      people_num = rtrains_passengers * DAY_RTRAIN_RATIO_MORNING_PEAK /
                   DAY_RTRAIN_BLK_NUM_MORNING_PEAK;
   } else if (isInInterval(mins_passed, DAY_DAYTIME_ATTENUATION_START,
                           DAY_DAYTIME_ATTENUATION_END)) {
      people_num = rtrains_passengers * DAY_RTRAIN_RATIO_DAYTIME_ATTENUATION /
                   DAY_RTRAIN_BLK_NUM_DAYTIME_ATTENUATION;
   } else if (isInInterval(mins_passed, DAY_EVENING_PEAK_START,
                           DAY_EVENING_PEAK_END)) {
      people_num = rtrains_passengers * DAY_RTRAIN_RATIO_EVENING_PEAK /
                   DAY_RTRAIN_BLK_NUM_EVENING_PEAK;
   } else if (isInInterval(mins_passed, DAY_LATE_EVENENING_ATTENUATION_START,
                          DAY_LATE_EVENENING_ATTENUATION_END)) {
      people_num = rtrains_passengers * DAY_RTRAIN_RATIO_EVENING_ATTENUATION /
                   DAY_RTRAIN_BLK_NUM_EVENING_ATTENUATION;
   } else if (mins_passed > DAY_NIGHT_STAGNATION_START ||
              mins_passed < DAY_NIGHT_STAGNATION_END) {
      /* Generator is woken only every 30-minutes in this day part. */
      in_night = true;
      night_calls++;

      /* 3 subsequent night calls mean 30-day interval. */
      if (night_calls == PASSENGER_NIGHT_GEN_INTERVAL /
                         PASSENGER_DAY_GEN_INTERVAL) {
         people_num = rtrains_passengers * DAY_RTRAIN_RATIO_NIGHT_STAGNATION /
                      DAY_RTRAIN_BLK_NUM_NIGHT_STAGNATION;
         night_calls = 0;
      }
   }

   if (!in_night) {
      night_calls = 0;
   }

   return Normal(people_num, people_num *
                 SimSettings::PASSENGERS_NORMAL_DISTRIB_QUOTIENT);
}
