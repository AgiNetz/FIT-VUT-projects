/**
 * \file time_manager.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Module for managing time of day events
 */
#ifndef TIME_MANAGER_HPP
#define TIME_MANAGER_HPP

#include <simlib.h>

enum class DayPart {
   EARLY_BIRD,
   MORNING_PEAK,
   DAYTIME_ATTENUATION,
   EVENING_PEAK,
   LATE_EVENING_ATTENUATION,
   NIGHT_STAGNATION
};

class TimeManager : public Event
{
public:
   TimeManager() : Event() {};
   ~TimeManager() {};

   DayPart GetPartOfDay();
   int GetStartOfDay();
   int GetCurrentCityRideDelay();
   int GetCurrentOutskirtsRideDelay();
   void InitShifts();
   void Behavior();

private:
};

#endif