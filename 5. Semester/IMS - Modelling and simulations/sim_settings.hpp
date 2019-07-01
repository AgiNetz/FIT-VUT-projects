/**
 * \file sim_settings.hpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Simulation process settings & definitions.
 */

#ifndef SIM_SETTINGS_HPP_INCLUDED
#define SIM_SETTINGS_HPP_INCLUDED

#include <vector>
#include <iostream>

class Simulation;

/** Defines numerous simulation settings and constants. */
namespace SimSettings {
   /** Parent simulation object */
   extern Simulation *sim;

   /** Number of minutes in day. */
   extern const int DAY_MINUTES;

   /** Default number of days to be simulated. */
   extern const int SIM_DAYS_DEFAULT;

   /* -----------------------   Weather generation   ----------------------- */
   extern const double WEATHER_BAD_CHANCE;
   extern const double WEATHER_GOOD_CHANCE;

   /* ----------------------   Passenger generation   ---------------------- */
   /* -------------------   General settings   ------------------- */
   /** Passengers daily average. */
   extern const int PASSENGERS_DAILY_AVG;

   /** Passenger normal distribution quotient. */
   extern const double PASSENGERS_NORMAL_DISTRIB_QUOTIENT;

   /** Makes train arrivals less predictable. */
   extern const int TRAIN_ARRIVAL_VARIATION;

   /* --------------------   Regular trains   -------------------- */
   /** Passenger generation interval during the day/morning. */
   extern const int PASSENGER_DAY_GEN_INTERVAL;

   /* Passenger generation interval during the night. */
   extern const int PASSENGER_NIGHT_GEN_INTERVAL;

   /** Passenger ratios to types of trains. */
   extern const double PASSENGER_RATIO_REG_TRAINS;       ///< Regular trains
   extern const double PASSENGER_RATIO_EXPRESS_TRAINS;   ///< Main trains

   /* Day phases threshold values in minutes elapsed since midnight. */
   extern const int DAY_EARLY_BIRD_START;                  ///< 4:00
   extern const int DAY_EARLY_BIRD_END;                    ///< 5:40
   extern const int DAY_MORNING_PEAK_START;                ///< 6:00
   extern const int DAY_MORNING_PEAK_END;                  ///< 9:10 
   extern const int DAY_DAYTIME_ATTENUATION_START;         ///< 10:00
   extern const int DAY_DAYTIME_ATTENUATION_END;           ///< 16:10
   extern const int DAY_EVENING_PEAK_START;                ///< 16:30
   extern const int DAY_EVENING_PEAK_END;                  ///< 19:10
   extern const int DAY_LATE_EVENENING_ATTENUATION_START;  ///< 19:30
   extern const int DAY_LATE_EVENENING_ATTENUATION_END;    ///< 21:10
   extern const int DAY_NIGHT_STAGNATION_START;            ///< 21:20
   extern const int DAY_NIGHT_STAGNATION_END;              ///< 0:40

   /**
   * Specifies ratios of passengers travelling in regular trains during
   * specified day phase.
   */
   extern const double DAY_RTRAIN_RATIO_EARLY_BIRD;
   extern const double DAY_RTRAIN_RATIO_MORNING_PEAK;
   extern const double DAY_RTRAIN_RATIO_DAYTIME_ATTENUATION;
   extern const double DAY_RTRAIN_RATIO_EVENING_PEAK;
   extern const double DAY_RTRAIN_RATIO_EVENING_ATTENUATION;
   extern const double DAY_RTRAIN_RATIO_NIGHT_STAGNATION;

   /** Number of N-minute blocks (PASSENGER_DAY/NIGHT_GEN_INTERVAL) in a
    *  particular day.
    */
   extern const int DAY_RTRAIN_BLK_NUM_EARLY_BIRD;
   extern const int DAY_RTRAIN_BLK_NUM_MORNING_PEAK;
   extern const int DAY_RTRAIN_BLK_NUM_DAYTIME_ATTENUATION;
   extern const int DAY_RTRAIN_BLK_NUM_EVENING_PEAK;
   extern const int DAY_RTRAIN_BLK_NUM_EVENING_ATTENUATION;
   extern const int DAY_RTRAIN_BLK_NUM_NIGHT_STAGNATION;

   /* --------------------   Express trains   -------------------- */
   struct ExpressTrainInfo {
      int arrival;         ///< Train arrival time in minutes after 00:00
      double ratio;        ///< Ratio of passengers getting of from the train
   };

   /* Delay tresholds. */
   extern const double TRAIN_DELAY_NONE_TRESHOLD;     ///< No delay
   extern const double TRAIN_DELAY_5_TRESHOLD;        ///< Delay 5 min
   extern const double TRAIN_DELAY_10_TRESHOLD;       ///< Delay 10 mins
   extern const double TRAIN_DELAY_15_TRESHOLD;       ///< Delay 15 mins
   extern const double TRAIN_DELAY_20_TRESHOLD;       ///< Delay 20 mins
   extern const double TRAIN_DELAY_25_TRESHOLD;       ///< Delay 25 mins
   extern const double TRAIN_DELAY_30_TRESHOLD;       ///< Delay 30 mins

   /** Number of express trains in a single day. */
   extern const int EXTRAIN_DAILY_NUM;

   /** Taxi taking chances constants. */
   extern const double TAKE_TAXI_GOOD_WEATHER_CHANCE;
   extern const double TAKE_TAXI_BAD_WEATHER_CHANCE;

   /** Taxi parking lot choice chances */
   extern const double TAXI_PARKING_LOT_1_CHANCE;

   /** Taxi parking lot capacities  */
   extern const int COMPETITION_PARKING_LOT_1_CAPACITY;
   extern const int COMPETITION_PARKING_LOT_2_CAPACITY;
   extern const int OUR_PARKING_LOT_1_CAPACITY;
   extern const int OUR_PARKING_LOT_2_CAPACITY;

   /** Number of taxis in the parking lots during parts of day*/
   extern const int COMPETITION_PARKING_LOT_1_TAXIS[];
   extern const int COMPETITION_PARKING_LOT_2_TAXIS[];
   extern int OUR_PARKING_LOT_1_TAXIS[];
   extern int OUR_PARKING_LOT_2_TAXIS[];

   /** Chances of waiting in Taxi queue */
   extern const double QUEUE_WAIT_BAD_WEATHER_CHANCE;
   extern const double QUEUE_WAIT_GOOD_WEATHER_CHANCE;
   extern const double QUEUE_WAIT_FOREVER_BAD_WEATHER_CHANCE;
   extern const double QUEUE_WAIT_FOREVER_GOOD_WEATHER_CHANCE;
   
   /** Mean times of waiting in taxi queue */
   extern const double QUEUE_WAIT_TIME_BAD_WEATHER;
   extern const double QUEUE_WAIT_TIME_GOOD_WEATHER;

   /** Ride destination chances */
   extern const double RIDE_CITY_CHANCE;

   /** Ride delays */
   extern const int RIDE_TIMES_CITY[];
   extern const int RIDE_TIMES_OUTSKIRTS[];

/**
 * Vector of EXTRAIN_IN_DAILY_NUM trains, which bring certain number of
 * passengers according to the part of the day and direction.
 * Data of train arrivals were taken from the:
 * http://www.novezamky.sk/assets/File.ashx?id_org=700036&id_dokumenty=30188
 * 
 * Given ratios are based mostly on observation and assumptions, since no
 * official statistics were available.
 */
   extern std::vector<ExpressTrainInfo> express_trains;
}

#endif
