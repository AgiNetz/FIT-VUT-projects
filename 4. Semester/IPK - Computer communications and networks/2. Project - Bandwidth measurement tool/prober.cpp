/**
 * IPK Project 2: Application for measuring available bandwidth between 2 endpoints 
* Author: Tomas Danis
 * Login: xdanis05
 * File: prober.cpp
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <vector>
#include <cmath>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "api.hpp"

const long long unsigned int INITIAL_ESTIMATE = 1000000; //1Mbit

/**
 * Procedure fulfilling the role of "meter" in the bandwidth measurement process
 * Connects to the reflector, estimates the available bandwidth and floods the
 * reflector with data supposed to overload the network if the estimate is too much.
 * The reflector receives all these packets and sends back the received count. If some
 * packets were lost, the estimate is decreased, otherwise it is increased. This way,
 * the procedure estimates the available bandwidth through binary search.
 * Once the estimate is accurate enough, a new measurement begins with the initial
 * estimate equal to the measured bandwidth */
int prober(char *hostname, char *port, unsigned long probeSize, int measureTime)
{
   using namespace std;
   struct addrinfo hints;
   struct addrinfo *result;
   struct addrinfo *it;
   int success;
   int sender;

   /* Connecting to the reflector */

   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_DGRAM;

   if ((success = getaddrinfo(hostname, port, &hints, &result)) != 0) 
   {
      cerr << "Couldn't retrieve information about evaluator! Error: " << gai_strerror(success) << endl;
      return BM_GETADDR_ERR;
   }

   for(it = result; it != NULL; it = it->ai_next) 
   {
      if ((sender = socket(it->ai_family, it->ai_socktype, it->ai_protocol)) == -1) 
      {
         cerr << "Could not create socket! Trying another address.." << endl;
         continue;
      }

      if(connect(sender, it->ai_addr, it->ai_addrlen) < 0)
      {
         close(sender);
         cerr << "Could not connect to the reflector! Trying another address.." << endl;
         continue;
      }
      break;
   }

   if (it == NULL) 
   {
      cerr << "Could not connect through any socket!" << endl;
      return BM_BIND_ERR;
   }

   /* Measurement data preparation */

   long long unsigned int estimate = INITIAL_ESTIMATE;
   long long unsigned int estMax = 0;
   long long unsigned int estMin = 0;
   long long unsigned int toSend;
   int sentBytes;
   int recvBytes;
   char buf[1500];
   int sendSize;
   unsigned long int msgCount = 0;
   std::chrono::duration<double> measurementTime;
   double estimateMultiplier = 1.8;
   vector<long long unsigned int> measurements;
   vector<double> rtts;
   chrono::time_point<chrono::high_resolution_clock> globalStart = chrono::high_resolution_clock::now();
   chrono::time_point<chrono::high_resolution_clock> globalEnd;
   chrono::time_point<chrono::high_resolution_clock> lastSend;
   chrono::time_point<chrono::high_resolution_clock> lastRecv;

   /* Data flooding cycle */
   do
   {
      toSend = estimate / 24;
      if(probeSize * 10 > toSend)
      {
         cerr << "Bandwidth too low, adjusting probe size.." << endl;
         probeSize = toSend/10;
      }
      msgCount = 0;
      int sleepFreq = 45;
      long int sleepTime = 1000000*probeSize*sleepFreq/toSend;
      if(sleepTime > 100000)
      {
         sleepTime = 100000;
         sleepFreq = toSend/(10*probeSize);
      }

      /* Flooding */
      while(toSend != 0)
      {
         sendSize = (probeSize < toSend) ? probeSize : toSend;
         if ((sentBytes = sendto(sender, buf, sendSize, 0, it->ai_addr, it->ai_addrlen)) == -1) 
         {
            cerr << "Sending a message has failed!" << endl;
            return BM_SEND_ERR;
         }
         lastSend = chrono::high_resolution_clock::now();
         toSend -= sentBytes;
         msgCount++;
         if(msgCount % sleepFreq == 0)
            usleep(sleepTime);
      }

      /* Waiting for the informatio about the number of received packets from the reflector */
      fd_set monitor;
      struct timeval timeout;
      timeout.tv_sec = 4;
      timeout.tv_usec = 0;
      FD_ZERO(&monitor);
      FD_SET(sender, &monitor);

      select(sender+1, &monitor, NULL, NULL, &timeout);

      if(FD_ISSET(sender, &monitor))
      {
         if ((recvBytes = recvfrom(sender, buf, 1500, 0, it->ai_addr, &(it->ai_addrlen))) == -1) 
         {
            cerr << "Receiving a message has failed!" << endl;
            return BM_RECV_ERR;
         }
         lastRecv = chrono::high_resolution_clock::now();
         /* RTT calculation */
         chrono::duration<double, std::milli> rtt = lastRecv - lastSend;
         rtts.push_back(rtt.count());

         /* Estimate improvement */
         unsigned long recvCount = strtoul(buf, NULL, 10);

         if(msgCount == recvCount)
         {
            estMin = estimate;
            if(estMax == 0)
            {
               estimate *= estimateMultiplier;
            }
            else
            {
               estimate = (estMin + estMax)/2;
            }
         }
         else if(msgCount > recvCount)
         {
            estMax = estimate;
            if(estMin == 0)
            {
               estimate /= estimateMultiplier;
            }
            else
            {
               estimate = (estMin + estMax)/2;
            }
         }

         /* Accuracy adjustment */
         if(estimate < 24000)
         {
            estimate = 24000;
            cout << "Estimated bandwidth is less than 24Kbit. Unable to measure precisely." << endl;
         }
         
         /* Is the current estimate precise enough? If yes, begin new measurement */
         if(estMin*1.02 >= estMax && estMin != 0 && estMax != 0)
         {
            measurements.push_back(estimate);
            estMin = 0;
            estMax = 0;
            estimateMultiplier = 1.15;
         }
         else 
            continue;
      }

      globalEnd = chrono::high_resolution_clock::now();
      measurementTime = globalEnd - globalStart;
   }
   while(measurementTime.count() < measureTime);

   /* Output data calculation */
   double maxBW = 0;
   double minBW = measurements[0];
   double avgBW = 0;
   double dev = 0;

   /* Max, min, avg BW calculation */
   for(unsigned int i = 0; i < measurements.size(); i++)
   {
      measurements[i] /= 1000000;
      avgBW += measurements[i];
      if(measurements[i] > maxBW)
         maxBW = measurements[i];
      else if (measurements[i] < minBW)
         minBW = measurements[i];
   }

   avgBW /= measurements.size();

   /* Standard deviation calculation */
   for(unsigned int i = 0; i < measurements.size(); i ++)
   {
      dev += (measurements[i] - avgBW)*(measurements[i] - avgBW);
   }

   dev /= measurements.size();
   dev = sqrt(dev);

   /* Average RTT calculation */

   double avgRTT = 0;
   for(unsigned int i = 0; i < rtts.size(); i++)
   {
      rtts[i] -= 200; //Subtract the amount of time the reflector waits before sending a reply (200ms)
      avgRTT += rtts[i];
   }

   avgRTT /= rtts.size();

   cout << "Average bandwidth: " << avgBW << " Mbits/s" << endl;
   cout << "Maximum bandwidth: " << maxBW << " Mbits/s" << endl;
   cout << "Minimum bandwidth: " << minBW << " Mbits/s" << endl;
   cout << "Standard deviation: " << dev << " Mbits/s" << endl;
   cout << "Average RTT: " << avgRTT << " ms" << endl;
   
   freeaddrinfo(result);
   close(sender);

   return EXIT_SUCCESS;

}