/**
 * \file shift_change.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Process responsible for changing system state during various parts of the day
 */

#ifndef SHIFT_CHANGE_HPP
#define SHIFT_CHANGE_HPP

#include <simlib.h>

class ShiftChange : public Process
{
public:
   ShiftChange(Store *store, int add_taxis);
   void Behavior();
private:
   Store *store;
   int add_taxis;
};

#endif