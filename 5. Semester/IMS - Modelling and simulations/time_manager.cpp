/**
 * \file time_manager.cpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Module for managing time of day events
 */

#include "time_manager.hpp"
#include "sim_settings.hpp"
#include "shift_change.hpp"
#include "simulation.hpp"
#include <iostream>

DayPart TimeManager::GetPartOfDay()
{
   int timeofday = Time - GetStartOfDay();
   if(timeofday < SimSettings::DAY_EARLY_BIRD_START)
      return DayPart::NIGHT_STAGNATION;
   else if(timeofday < SimSettings::DAY_MORNING_PEAK_START)
      return DayPart::EARLY_BIRD;
   else if(timeofday < SimSettings::DAY_DAYTIME_ATTENUATION_START)
      return DayPart::MORNING_PEAK;
   else if(timeofday < SimSettings::DAY_EVENING_PEAK_START)
      return DayPart::DAYTIME_ATTENUATION;
   else if(timeofday < SimSettings::DAY_LATE_EVENENING_ATTENUATION_START)
      return DayPart::EVENING_PEAK;
   else if(timeofday < SimSettings::DAY_NIGHT_STAGNATION_START)
      return DayPart::LATE_EVENING_ATTENUATION;
   else
      return DayPart::NIGHT_STAGNATION;
}

int TimeManager::GetStartOfDay()
{
   int days = Time/SimSettings::DAY_MINUTES;
   return days*SimSettings::DAY_MINUTES;
}

int TimeManager::GetCurrentCityRideDelay()
{
   return SimSettings::RIDE_TIMES_CITY[static_cast<int>(GetPartOfDay())];
}

int TimeManager::GetCurrentOutskirtsRideDelay()
{
   return SimSettings::RIDE_TIMES_OUTSKIRTS[static_cast<int>(GetPartOfDay())];
}

void TimeManager::InitShifts()
{
   int unused_capacity = 0;

   unused_capacity = SimSettings::COMPETITION_PARKING_LOT_1_CAPACITY - SimSettings::COMPETITION_PARKING_LOT_1_TAXIS[5];
   (new ShiftChange(SimSettings::sim->competition_pl1, -unused_capacity))->Activate();

   unused_capacity = SimSettings::COMPETITION_PARKING_LOT_2_CAPACITY - SimSettings::COMPETITION_PARKING_LOT_2_TAXIS[5];
   (new ShiftChange(SimSettings::sim->competition_pl2, -unused_capacity))->Activate();

   unused_capacity = SimSettings::OUR_PARKING_LOT_1_CAPACITY - SimSettings::OUR_PARKING_LOT_1_TAXIS[5];
   (new ShiftChange(SimSettings::sim->our_pl1, -unused_capacity))->Activate();

   unused_capacity = SimSettings::OUR_PARKING_LOT_2_CAPACITY - SimSettings::OUR_PARKING_LOT_2_TAXIS[5];
   (new ShiftChange(SimSettings::sim->our_pl2, -unused_capacity))->Activate();
}

void TimeManager::Behavior()
{
   int shift_change_times[] = {
      SimSettings::DAY_EARLY_BIRD_START, 
      SimSettings::DAY_MORNING_PEAK_START,
      SimSettings::DAY_DAYTIME_ATTENUATION_START,
      SimSettings::DAY_EVENING_PEAK_START,
      SimSettings::DAY_LATE_EVENENING_ATTENUATION_START,
      SimSettings::DAY_NIGHT_STAGNATION_START
   };

   (new ShiftChange(SimSettings::sim->competition_pl1, 
      SimSettings::COMPETITION_PARKING_LOT_1_TAXIS[0] - SimSettings::COMPETITION_PARKING_LOT_1_TAXIS[5]))->Activate(Time + shift_change_times[0]);
   (new ShiftChange(SimSettings::sim->competition_pl2, 
      SimSettings::COMPETITION_PARKING_LOT_2_TAXIS[0] - SimSettings::COMPETITION_PARKING_LOT_2_TAXIS[5]))->Activate(Time + shift_change_times[0]);
   (new ShiftChange(SimSettings::sim->our_pl1, 
      SimSettings::OUR_PARKING_LOT_1_TAXIS[0] - SimSettings::OUR_PARKING_LOT_1_TAXIS[5]))->Activate(Time + shift_change_times[0]);
   (new ShiftChange(SimSettings::sim->our_pl2, 
      SimSettings::OUR_PARKING_LOT_2_TAXIS[0] - SimSettings::OUR_PARKING_LOT_2_TAXIS[5]))->Activate(Time + shift_change_times[0]);

   for(unsigned int i = 1; i < 6; i++)
   {
      (new ShiftChange(SimSettings::sim->competition_pl1, 
         SimSettings::COMPETITION_PARKING_LOT_1_TAXIS[i] - SimSettings::COMPETITION_PARKING_LOT_1_TAXIS[i-1]))->Activate(Time + shift_change_times[i]);
      (new ShiftChange(SimSettings::sim->competition_pl2, 
         SimSettings::COMPETITION_PARKING_LOT_2_TAXIS[i] - SimSettings::COMPETITION_PARKING_LOT_2_TAXIS[i-1]))->Activate(Time + shift_change_times[i]);
      (new ShiftChange(SimSettings::sim->our_pl1, 
         SimSettings::OUR_PARKING_LOT_1_TAXIS[i] - SimSettings::OUR_PARKING_LOT_1_TAXIS[i-1]))->Activate(Time + shift_change_times[i]);
      (new ShiftChange(SimSettings::sim->our_pl2, 
         SimSettings::OUR_PARKING_LOT_2_TAXIS[i] - SimSettings::OUR_PARKING_LOT_2_TAXIS[i-1]))->Activate(Time + shift_change_times[i]);
   }

   this->Activate(Time + SimSettings::DAY_MINUTES);
}

