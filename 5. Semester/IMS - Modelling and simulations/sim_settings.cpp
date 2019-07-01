/**
 * \file sim_settings.cpp
 * \author Patrik Goldschmidt, xgolds00 AT stud.fit.vutbr.cz
 * \author Tomas Danis, xdanis05 AT stud.fit.vutbr.cz
 * \date 06.12.2018
 * \brief Simulation process settings & definitions.
 */

#include "sim_settings.hpp"
#include "simulation.hpp"
#include <math.h>
#include <vector>

/* --------------------------   Parent object      ------------------------- */
Simulation *SimSettings::sim = nullptr;

/* --------------------------   General settings   ------------------------- */
const int SimSettings::DAY_MINUTES(1440);
const int SimSettings::SIM_DAYS_DEFAULT(365);

/* -------------------------   Weather generation   ------------------------ */
const double SimSettings::WEATHER_BAD_CHANCE(140/365.0);
const double SimSettings::WEATHER_GOOD_CHANCE(
   1 - SimSettings::WEATHER_BAD_CHANCE);

/* ------------------------   Passenger generation   ----------------------- */
/* ---------------------------   Regular trains   -------------------------- */
/* Average passengers per day. */
const int SimSettings::PASSENGERS_DAILY_AVG(3900);

/* Passengers normal distribution ratio. */
const double SimSettings::PASSENGERS_NORMAL_DISTRIB_QUOTIENT(0.2);

/* Makes train arrivals less predictable. */
const int SimSettings::TRAIN_ARRIVAL_VARIATION(7);

/* Passengers ratio related to types of trains. */
const double SimSettings::PASSENGER_RATIO_REG_TRAINS(0.45);
const double SimSettings::PASSENGER_RATIO_EXPRESS_TRAINS(
   1 - PASSENGER_RATIO_REG_TRAINS);

/* Passenger generator interval in minutes. */
const int SimSettings::PASSENGER_DAY_GEN_INTERVAL(10);
const int SimSettings::PASSENGER_NIGHT_GEN_INTERVAL(30);

/* Day phases threshold values in minutes elapsed since 00:00. */
const int SimSettings::DAY_EARLY_BIRD_START(240);
const int SimSettings::DAY_EARLY_BIRD_END(340);
const int SimSettings::DAY_MORNING_PEAK_START(360);
const int SimSettings::DAY_MORNING_PEAK_END(550);
const int SimSettings::DAY_DAYTIME_ATTENUATION_START(600);
const int SimSettings::DAY_DAYTIME_ATTENUATION_END(970);
const int SimSettings::DAY_EVENING_PEAK_START(990);
const int SimSettings::DAY_EVENING_PEAK_END(1150);
const int SimSettings::DAY_LATE_EVENENING_ATTENUATION_START(1170);
const int SimSettings::DAY_LATE_EVENENING_ATTENUATION_END(1270);
const int SimSettings::DAY_NIGHT_STAGNATION_START(1280);
const int SimSettings::DAY_NIGHT_STAGNATION_END(40);

/*
 * Defines ratios of passengers travelling in regular trains during
 * specified day phase.
 */
const double SimSettings::DAY_RTRAIN_RATIO_EARLY_BIRD(0.07);
const double SimSettings::DAY_RTRAIN_RATIO_MORNING_PEAK(0.16);
const double SimSettings::DAY_RTRAIN_RATIO_DAYTIME_ATTENUATION(0.13);
const double SimSettings::DAY_RTRAIN_RATIO_EVENING_PEAK(0.48);
const double SimSettings::DAY_RTRAIN_RATIO_EVENING_ATTENUATION(0.10);
const double SimSettings::DAY_RTRAIN_RATIO_NIGHT_STAGNATION(0.06);


/* Number of N-minute blocks used in particular day stages. */
const int SimSettings::DAY_RTRAIN_BLK_NUM_EARLY_BIRD(
   round((SimSettings::DAY_EARLY_BIRD_END -
   SimSettings::DAY_EARLY_BIRD_START) /
   SimSettings::PASSENGER_DAY_GEN_INTERVAL)
);
const int SimSettings::DAY_RTRAIN_BLK_NUM_MORNING_PEAK(
   round((SimSettings::DAY_MORNING_PEAK_END -
   SimSettings::DAY_MORNING_PEAK_START) /
   SimSettings::PASSENGER_DAY_GEN_INTERVAL)
);
const int SimSettings::DAY_RTRAIN_BLK_NUM_DAYTIME_ATTENUATION(
   round((SimSettings::DAY_DAYTIME_ATTENUATION_END -
   SimSettings::DAY_DAYTIME_ATTENUATION_START) /
   SimSettings::PASSENGER_DAY_GEN_INTERVAL)
);
const int SimSettings::DAY_RTRAIN_BLK_NUM_EVENING_PEAK(
   round((SimSettings::DAY_EVENING_PEAK_END -
   SimSettings::DAY_EVENING_PEAK_START) /
   SimSettings::PASSENGER_DAY_GEN_INTERVAL)
);
const int SimSettings::DAY_RTRAIN_BLK_NUM_EVENING_ATTENUATION(
   round((SimSettings::DAY_LATE_EVENENING_ATTENUATION_END -
   SimSettings::DAY_LATE_EVENENING_ATTENUATION_START) /
   SimSettings::PASSENGER_DAY_GEN_INTERVAL)
);
const int SimSettings::DAY_RTRAIN_BLK_NUM_NIGHT_STAGNATION(
   round((SimSettings::DAY_MINUTES - SimSettings::DAY_NIGHT_STAGNATION_START +
   SimSettings::DAY_NIGHT_STAGNATION_END) /
   SimSettings::PASSENGER_NIGHT_GEN_INTERVAL)
);

/* ---------------------------   Express trains   -------------------------- */
const int SimSettings::EXTRAIN_DAILY_NUM(25);

/* Delay tresholds. */
const double SimSettings::TRAIN_DELAY_NONE_TRESHOLD(0.5);    ///< No delay
const double SimSettings::TRAIN_DELAY_5_TRESHOLD(0.75);      ///< Delay 5 min
const double SimSettings::TRAIN_DELAY_10_TRESHOLD(0.875);    ///< Delay 10 mins
const double SimSettings::TRAIN_DELAY_15_TRESHOLD(0.9375);   ///< Delay 15 mins
const double SimSettings::TRAIN_DELAY_20_TRESHOLD(0.96875);  ///< Delay 20 mins
const double SimSettings::TRAIN_DELAY_25_TRESHOLD(0.987375); ///< Delay 25 mins
const double SimSettings::TRAIN_DELAY_30_TRESHOLD(1);        ///< Delay 30 mins

/** Taxi taking chances constants. */
const double SimSettings::TAKE_TAXI_GOOD_WEATHER_CHANCE(0.07);
const double SimSettings::TAKE_TAXI_BAD_WEATHER_CHANCE(0.13);

/** Taxi parking lot choice chances */
const double SimSettings::TAXI_PARKING_LOT_1_CHANCE(0.7);

/** Taxi parking lot capacities  */
const int SimSettings::COMPETITION_PARKING_LOT_1_CAPACITY = 8;
const int SimSettings::COMPETITION_PARKING_LOT_2_CAPACITY = 3;
const int SimSettings::OUR_PARKING_LOT_1_CAPACITY = 3;
const int SimSettings::OUR_PARKING_LOT_2_CAPACITY = 2;

/** Number of taxis in the parking lots during parts of day*/
const int SimSettings::COMPETITION_PARKING_LOT_1_TAXIS[] = {3,7,2,8,4,1};
const int SimSettings::COMPETITION_PARKING_LOT_2_TAXIS[] = {1,2,1,3,1,0};
int SimSettings::OUR_PARKING_LOT_1_TAXIS[] = {1,1,1,1,1,1};
int SimSettings::OUR_PARKING_LOT_2_TAXIS[] = {0,0,0,0,0,0};

/** Chances of waiting in Taxi queue */
const double SimSettings::QUEUE_WAIT_BAD_WEATHER_CHANCE(0.94);
const double SimSettings::QUEUE_WAIT_GOOD_WEATHER_CHANCE(0.72);

/** Mean times of waiting in taxi queue */
const double SimSettings::QUEUE_WAIT_TIME_BAD_WEATHER(35);
const double SimSettings::QUEUE_WAIT_TIME_GOOD_WEATHER(14);

/** Ride destination chances */
const double SimSettings::RIDE_CITY_CHANCE(0.92);
/* Else destination - outskirts */

const int SimSettings::RIDE_TIMES_CITY[] = {11, 17, 11, 17, 11, 11};
const int SimSettings::RIDE_TIMES_OUTSKIRTS[] = {15, 21, 15, 21, 15, 15};

/**
 * Vector of EXTRAIN_IN_DAILY_NUM trains, which bring certain number of
 * passengers according to the part of the day and direction.
 * Data of train arrivals were taken from the:
 * http://www.novezamky.sk/assets/File.ashx?id_org=700036&id_dokumenty=30188
 * 
 * Given ratios are based mostly on observation and assumptions, since no
 * official statistics were available.
 */
std::vector<SimSettings::ExpressTrainInfo> SimSettings::express_trains {
   {39,   0.03},     /* R 801    00:39  BA HL.ST. 23:30 - GA 00:08       */  
   {240,  0.01},     /* R 800    04:00  Prešov 21:12 - Zvolen 1:59       */
   {404,  0.03},     /* EN 477   06:44  Breclav 4:59 - BA HL.ST. 5:53    */
   {428,  0.05},     /* EC 280   07:08  Budapest N 5:40 - Štúrovo 6:43   */
   {529,  0.03},     /* EC 271   08:49  Brno 6:21 - Bratisl.hl. 7:57     */
   {548,  0.03},     /* EC 172   09:08  Budapest N 7:40 - Štúrovo 8:43   */
   {608,  0.01},     /* EC 130   10:08  Budapest N 8:40 - Štúrovo 9:43   */
   {649,  0.01},     /* EC 273   10:49  Praha HL. 5:50 - BA HL.ST.9:57   */
   {668,  0.02},     /* EC 278   11:08  Budapest N 9:40 - Štúrovo 10:43  */
   {769,  0.02},     /* EC 275   12:49  Praha HL. 7:50 - BA HL.ST.11:57  */
   {788,  0.02},     /* EC 276   13:08  Budapest N 11:40 - Štúrovo 12:43 */
   {889,  0.03},     /* EC 277   14:49  Praha HL. 9:50 - BA HL.ST.13:57  */
   {908,  0.03},     /* EC 274   15:08  Budapest N 13:40 -Štúrovo 14:43  */
   {959,  0.05},     /* RR 871   15:59  BA HL.ST 14:57 - GA 15:22        */
   {1009, 0.06},     /* EC 279   16:49  Praha HL. 11:50 - BA HL.ST.13:57 */
   {1028, 0.05},     /* EC 272   17:08  Budapest N 15:40 -Štúrovo 16:43  */
   {1062, 0.09},     /* REX 1875 17:42  BA HL.ST 16:33 - GA 17:14        */
   {1067, 0.08},     /* EC 131   17:47  Warszawa W . 8:49 - BA HL.16:55  */
   {1122, 0.06},     /* REX 1877 18:42  BA HL.ST 17:33 - GA 18:14        */
   {1129, 0.06},     /* EC 173   18:49  Hamburg 6:36 - BA HL.ST. 17:57   */
   {1148, 0.05},     /* EC 270   19:08  Budapest N 17:40 -Štúrovo 18:43  */
   {1182, 0.06},     /* REC 1879 19:42  BA HL.ST 18:33 - GA 19:14        */
   {1249, 0.05},     /* EC 281   20:49  Praha HL. 15:50 - BA HL.ST.19:57 */
   {1268, 0.03},     /* EN 476   21:07  Budapest N. 19:25 -Štúrovo 20:40 */
   {1374, 0.04}      /* RR 873   22:54  BA HL.ST 21:53 - GA 22:26        */
};
