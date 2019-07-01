/**
 * \file shift_change.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Process responsible for changing system state during various parts of the day
 */

#include "shift_change.hpp"
#include <iostream>

ShiftChange::ShiftChange(Store *store, int add_taxis) : Process()
{
   this->store = store;
   this->add_taxis = add_taxis;
}

void ShiftChange::Behavior()
{
   if(add_taxis < 0)
   {
      Enter(*this->store, -this->add_taxis);
   }
   else
   {
      Leave(*this->store, this->add_taxis);
   }
}