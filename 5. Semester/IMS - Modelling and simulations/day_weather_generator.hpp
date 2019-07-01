/**
 * \file day_weather_generator.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 08.12.2018
 * \brief Generator for generating variable weather declaration.
 */

#ifndef DAY_WEATHER_GENERATOR_HPP_INCLUDED
#define DAY_WEATHER_GENERATOR_HPP_INCLUDED

#include <simlib.h>

class DayWeatherGenerator : public Event {
   public:
   /**
    * Constructs DayWeatherGenerator object and binds it to a particular
    * Simulation object to update day status in.
    */
   DayWeatherGenerator(void *simulation_obj);

   /**
    * \brief Generates new weather every Exp(24h) or SimSettings::DAY_MINUTES
    *        respectively.  Weather model is generated according to statistics
    *        on meteoblue.com website:
    * https://www.meteoblue.com/sk/počasie/predpoveď/modelclimate/nové-zámky_
    * slovenská-republika_3058472
    * 
    * For simplification, "bad weather" is defines as a day when its either
    * freezing or raining more than 2mm.  This status lasts for whole day.
    * Type of a day is then calculated as a percentual chance to particular day
    * to all days in a year.
    */
   void Behavior();

   private:
   /** Pointer to simulation instance to update its weather status. */
   void *simobj;
};

#endif      /* DAY_WEATHER_GENERATOR_HPP_INCLUDED */