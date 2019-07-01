/**
 * IPK Project 2: Application for measuring available bandwidth between 2 endpoints 
* Author: Tomas Danis
 * Login: xdanis05
 * File: ipk-mtrip.cpp
 */

#include <iostream>
#include <cstdlib>
#include <string>
#include <cstring>
#include <unistd.h>
#include <ctype.h>

#include "prober.hpp"
#include "evaluator.hpp"
#include "api.hpp"

using namespace std;

const int P_FLAG = 0;
const int H_FLAG = 1;
const int S_FLAG = 2;
const int T_FLAG = 3;

const int MIN_PROBE_SIZE = 100;
const int MAX_PROBE_SIZE = 1500;
const int MIN_MEASUREMENT_TIME = 20;

const string USAGE = "Usage: ./ipk-mtrip [meter|reflect] -h <host> -p <port> -s <size> -t <time>";
const string REFLECT_USAGE = "Usage: ./ipk-mtrip reflect -p <port>";
const string METER_USAGE = "Usage: ./ipk-mtrip meter -h <host> -p <port> -s <size> -t <time>";

bool parseArgs(int argc, char *argv[], bool flags[], char *argVals[]);

int main(int argc, char *argv[])
{
   bool flags[4] { false, false, false, false }; //Flags array
   char *argVals[4] { NULL, NULL, NULL, NULL} ;
   if(!parseArgs(argc, argv, flags, argVals))
      return BM_ARG_ERR;

   if(strcmp(argv[1], "reflect") == 0)
   {
      if(argc != 4 || !flags[P_FLAG])
      {
         cerr << "Wrong arguments for the reflector!" << endl;
         cerr << REFLECT_USAGE << endl;
         return BM_ARG_ERR;
      }

      return evaluator(argVals[P_FLAG]);
   }
   else if(strcmp(argv[1], "meter") == 0)
   {
      if(!flags[H_FLAG])
      {
         cerr << "Host address was not specified!" << endl;
         cerr << METER_USAGE << endl;
         return BM_ARG_ERR;
      }
      else if(!flags[P_FLAG])
      {
         cerr << "Port to connect to was not specified!" << endl;
         cerr << METER_USAGE << endl;
         return BM_ARG_ERR;
      }
      else if(!flags[S_FLAG])
      {
         cerr << "Size of the probe packet was not specified!" << endl;
         cerr << METER_USAGE << endl;
         return BM_ARG_ERR;
      }
      else if(!flags[T_FLAG])
      {
         cerr << "Time to measure was not specified!" << endl;
         cerr << METER_USAGE << endl;
         return BM_ARG_ERR;         
      }
      else if(argc != 10)
      {
         cerr << "Unknown arguments found!" << endl;
         cerr << METER_USAGE << endl;
         return BM_ARG_ERR;
      }

      char *err = NULL;
      unsigned long probeSize = strtoul(argVals[S_FLAG], &err, 10);
      if(err != NULL && *err != 0)
      {
         cerr << "Specified probe size is not a positive number!" << endl;
         return BM_ARG_ERR;
      }

      if(probeSize > MAX_PROBE_SIZE)
      {
         cerr << "Maximum probe size is " << MAX_PROBE_SIZE << "! Truncating.." << endl;
         probeSize = MAX_PROBE_SIZE;
      }
      else if(probeSize < MIN_PROBE_SIZE)
      {
         cerr << "Minimum probe size is " << MIN_PROBE_SIZE << "! Upscaling.." << endl;
         probeSize = MIN_PROBE_SIZE;
      }

      err = NULL;
      unsigned long measureTime = strtoul(argVals[T_FLAG], &err, 10);
      if(err != NULL && *err != 0)
      {
         cerr << "Specified measure time is not a positive number!" << endl;
         return BM_ARG_ERR;
      }

      if(measureTime < MIN_MEASUREMENT_TIME)
      {
         cerr << "Measure time is less than " << MIN_MEASUREMENT_TIME << " seconds! Results might not be accurate." << endl;  
      }

      return prober(argVals[H_FLAG], argVals[P_FLAG], probeSize, measureTime);
   }
   else
   {
      cerr << "First argument must be either \"reflect\" or \"meter\"!" << endl;
      return BM_ARG_ERR;
   }
}

/**
 * Procedure for parsing arguments.
 * Returns if the parsing was successful.
 */
bool parseArgs(int argc, char *argv[], bool flags[], char *argVals[])
{
   optind = 2; //Start parsing from the second argument
   opterr = 0; //Silent

   int arg = 0;
   while((arg = getopt(argc, argv, "h:p:s:t:")) != -1)
   {
      switch(arg)
      {
         case 'h':
            if(flags[H_FLAG])
            {
               cerr << "Duplicate flag -h!" << endl;
               cerr << USAGE << endl;
               return false;
            }

            flags[H_FLAG] = true;
            argVals[H_FLAG] = optarg;
            break;
         case 'p':
            if(flags[P_FLAG])
            {
               cerr << "Duplicate flag -p!" << endl;
               cerr << USAGE << endl;
               return false;
            }

            flags[P_FLAG] = true;
            argVals[P_FLAG] = optarg;
            break;
         case 's':
            if(flags[S_FLAG])
            {
               cerr << "Duplicate flag -s!" << endl;
               cerr << USAGE << endl;
               return false;
            }

            flags[S_FLAG] = true;
            argVals[S_FLAG] = optarg;
            break;
         case 't':
            if(flags[T_FLAG])
            {
               cerr << "Duplicate flag -t!" << endl;
               cerr << USAGE << endl;
               return false;
            }

            flags[T_FLAG] = true;
            argVals[T_FLAG] = optarg;
            break;
         case '?':
            switch(optopt)
            {
               case 'h':
                  cerr << "Host address was not specified!" << endl;
                  cerr << USAGE << endl;
                  return false;
                  break;
               case 'p':
                  cerr << "Port to connect to was not specified!" << endl;
                  cerr << USAGE << endl;
                  return false;
                  break;
               case 's':
                  cerr << "Size of the probe packet was not specified!" << endl;
                  cerr << USAGE << endl;
                  return false;
                  break;
               case 't':
                  cerr << "Time to measure was not specified!" << endl;
                  cerr << USAGE << endl;
                  return false;
                  break;
               default:
                  cerr << "Unknown option!" << endl;
                  cerr << USAGE << endl;
                  return false;
                  break;
            }
            break;
         default:
            cerr << USAGE << endl;
            return false;
            break;
      }
   }
   return true;
}