/**
 * \file passenger.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Passenger process
 */

#ifndef PASSENGER_HPP
#define PASSENGER_HPP

#include <simlib.h>
#include "simulation.hpp"

class Passenger : public Process
{
public:
   void Behavior();

private:
   Store* TakeTaxi(Store *first_competition, Store *first_ours, Store *second_competition, Store *second_ours);
   Store* TakeTaxiFromParkingLot(Store *competition, Store* ours);
   bool AttemptSeize(Store *store);
};

#endif