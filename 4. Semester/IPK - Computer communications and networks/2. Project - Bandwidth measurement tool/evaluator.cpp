/**
 * IPK Project 2: Application for measuring available bandwidth between 2 endpoints 
* Author: Tomas Danis
 * Login: xdanis05
 * File: evaluator.cpp
 */

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <chrono>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/time.h>

#include "api.hpp"


/**
 * Procedure fulfilling the role of "reflector" in the bandwidth measurement process
 * Prepares for a connection from the meter and then waits to receive packets. When it
 * receives a stream of packet, it counts the number of received packets and sends this
 * information back to the meter. A stream ends when there are no packets received for 0,2s. */
int evaluator(char *port)
{
   using namespace std;
   struct addrinfo hints;
   struct addrinfo *result = NULL;
   struct addrinfo *it = NULL;
   int receiver;

   /* Prepare for connection from the meter */
   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_DGRAM;
   hints.ai_flags = AI_PASSIVE;

   int success;

   if ((success = getaddrinfo(NULL, port, &hints, &result)) != 0) 
   {
      cerr << "Couldn't retrieve information about local host! Error: " << gai_strerror(success) << endl;
      return BM_GETADDR_ERR;
   }

   for(it = result; it != NULL; it = it->ai_next) 
   {
      if ((receiver = socket(it->ai_family, it->ai_socktype, it->ai_protocol)) == -1) 
      {
         cerr << "Could not create socket! Trying another address.." << endl;
         continue;
      }

      int yes = 1;
      setsockopt(receiver, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

      if (bind(receiver, it->ai_addr, it->ai_addrlen) == -1) 
      {
         close(receiver);
         cerr << "Could not bind to the socket! Trying another address.." << endl;
         continue;
      }

      break;
   }

   if (it == NULL) 
   {
      cerr << "Could not bind to any address!" << endl;
      return BM_BIND_ERR;
   }

   freeaddrinfo(result);

   /* Data setup for receiving */

   struct sockaddr_storage prober_addr;
   socklen_t addr_len = sizeof(prober_addr);
   int recvBytes = 0;
   int sentBytes;
   char buf[1500];
   long int count = 0;
   fd_set monitor;
   struct timeval timeout;
   timeout.tv_sec = 0;
   timeout.tv_usec = 200000;
   FD_ZERO(&monitor);
   FD_SET(receiver, &monitor);
   while(true)
   {
      /* Counting number of received packets in a stream */
      FD_ZERO(&monitor);
      FD_SET(receiver, &monitor);
      timeout.tv_sec = 0;
      timeout.tv_usec = 200000;
      select(receiver+1, &monitor, NULL, NULL, &timeout);
      if(!FD_ISSET(receiver, &monitor))
      {
         if(count == 0)
            continue;

         /* A stream has ended */
         string count_str = to_string(count);
         if((sentBytes = sendto(receiver, count_str.c_str(), count_str.length() + 1, 0, (struct sockaddr *)&prober_addr, addr_len)) == -1)
         {
            cerr << "Sending a message has failed!" << endl;
            return BM_SEND_ERR;
         }
         count = 0;
         continue;
      }

      /* Receive a packet from the stream */

      if ((recvBytes = recvfrom(receiver, buf, 1500, 0, (struct sockaddr *)&prober_addr, &addr_len)) == -1) 
      {
         cerr << "Receiving a message has failed!" << endl;
         return BM_RECV_ERR;
      }
      count++;
   }

   close(receiver);
   return EXIT_SUCCESS;
}