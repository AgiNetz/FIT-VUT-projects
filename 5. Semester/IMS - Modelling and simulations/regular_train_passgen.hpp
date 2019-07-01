/**
 * \file regular_train_passgen.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Generator definitions and generator wrapper structure.
 */

#ifndef REGULAR_TRAIN_PASSGEN_HPP_INCLUDED
#define REGULAR_TRAIN_PASSGEN_HPP_INCLUDED

#include "passenger_generator.hpp"

class RegularTrainPassengerGenerator : public PassengerGenerator {
   public:
   /**
    * \brief Empty constructor.
    */
   RegularTrainPassengerGenerator();
   /**
    * \brief Generates passengers groups by simulating bursts representing
    *        arrival of the train.  Number of passengers is defined according
    *        to daily average and part of the day.
    */
   void Behavior();

   private:
   inline bool isInInterval(int val, int bottom_limit, int upper_limit);

   /**
    * \brief Determines number of passengers to be generated accroding to the
    *        current part of the day.
    */
   int determinePassengersNum();
};

#endif