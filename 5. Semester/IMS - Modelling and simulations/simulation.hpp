/**
 * \file simulation.cpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Simulation contents declaration.
 */

#ifndef SIMULATION_HPP_INCLUDED
#define SIMULATION_HPP_INCLUDED

#include "express_train_passgen.hpp"
#include "day_weather_generator.hpp"
#include "regular_train_passgen.hpp"
#include "weather.hpp"
#include "time_manager.hpp"
#include <simlib.h>
#include <string>

class Simulation {
   public:
   /**
    * \brief Constructs objects and sets up private variables.
    */
   Simulation();

   /**
    * \brief Deallocates generators and other objects.
    */
   ~Simulation();

   /**
    * \brief Dump stats from all queues and statistics objects into the
    *        output file specified by setOutputFile() call.
    */
   void dumpStats();

   /**
    * \brief Initializes simulation by specifying start and end simulation
    *        times.  Poses as a wrapper for Simlib Init() function.
    * 
    * \param[in] Time to start the simulation in.
    * \param[in] Time to end the simulation.
    */
   void init(int start, int end);

   /**
    * \brief Sets output file to dump simulation results in. Poses as a wrapper
    *        for setOutput Simlib function.
    * 
    * \param[in] file Output file to write results in
    */
   void setOutputFile(std::string file);

   /**
    * \brief Starts the simulation process.  Poses as a wrapper for Run()
    *        function.
    */
   void start();

   /**
    * \brief Stops curret simulation being done by the instance.  Wrapper of 
    *        stop() Simlib function.
    */
   void stop();

   /**
    * \brief Sets the weather used in the simulation process.
    */
   void setWeather(WeatherType status);

   /**
    * \brief Retrievs currently set weather in the simulation model.
    */
   WeatherType weather();

   Queue taxi_queue;
   Store competition_pl1, competition_pl2, our_pl1, our_pl2;
   TimeManager time_manager;
   Stat passengers_want_taxi;
   Stat passengers_took_taxi;
   Stat our_passengers_city;
   Stat our_passengers_outskirts;
   Stat driven_minutes;
   Histogram queue_sizes;

   private:
   /** Generators used during the simulation process. */
   struct generators {
      RegularTrainPassengerGenerator *rtrain;   ///< Regular train passengers
      ExpressTrainPassengerGenerator *etrain;   ///< Express train passengers
      DayWeatherGenerator *weather;             ///< Weather generator
   } gener;

   /** Weather type for taxi chance taking adjust. */
   WeatherType model_weather;
};

#endif      /* SIMULATION_HPP_INCLUDED */