/**
 * \file main_train_passgen.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Express trains passenger generator.
 */

#ifndef MAIN_TRAIN_PASSENGER_GENERATOR_HPP_INCLUDED
#define MAIN_TRAIN_PASSENGER_GENERATOR_HPP_INCLUDED

#include "passenger_generator.hpp"

class ExpressTrainPassengerGenerator : public PassengerGenerator {
   public:
   /**
    * \brief Constructs the object, initializing member variables to their
    *        initial value.
    */
   ExpressTrainPassengerGenerator();

   /**
    * \brief Generates passengers for the express.  These trains are
    * mostly internation trains and express trains. Data were taken from the
    * official train timetable for Nove Zamky city:
    * http://www.novezamky.sk/assets/File.ashx?id_org=700036&id_dokumenty=30188
    */
   void Behavior();

   /**
    * \brief Generates a random number and according to it returns delay, which
    *        may be 0, 5, 10, 15, 20, 25 or 30 minutes.
    */
   static int delay();

   private:
   int trainnum;

   /**
    * \brief Determines arrival of the next train according to the part of the
    *        day determined by trainnum.  If all trains for the given day were
    *        already scheduled and trainum == SimSettings::EXTRAIN_DAILY_NUM,
    *        first train for the next day is scheduled.
    */
   double determineNextTrain();

   /**
    * \brief Determines number of passengers according to the part of the day.
    *        Numbers are tailored especially for particular train.
    *        Not all Express/International trains are included.
    */
   int determinePassengersNum();
};

#endif