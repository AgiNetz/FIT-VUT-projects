/**
 * \file passenger_generator.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Virtual class passenger generator declaration.
 */

#ifndef PASSENGER_GENERATOR_HPP_INCLUDED
#define PASSENGER_GENERATOR_HPP_INCLUDED

#include <simlib.h>

/**
 * \brief Abstract passenger generator class.
 */
class PassengerGenerator : public Event {
   public:
   PassengerGenerator() : Event() {}
   /**
    * \brief Generates number of passengers according to the part of the day
    * and other factors.
    */
   virtual void Behavior() = 0;
};

#endif      /* GENERATOR_HPP_INCLUDED */