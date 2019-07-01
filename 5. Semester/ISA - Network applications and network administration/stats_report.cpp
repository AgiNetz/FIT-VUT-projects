/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: stats_report.cpp
 * Description: Module for reporting statistics from a map to various destinations
 */

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/time.h>
#include <iomanip>

#include "stats_report.hpp"

/* Constants */
const char *SYSLOG_PORT = "514";

/* Prototypes */

/**
 * Sets up a UDP connection to a UDP server
 *
 * @param Hostname/address of the server to connect to
 * @param port UDP port on which to contact the server
 * @param err Buffer used for error reporting
 * @param server_info [out] Structure containing information about the server we have connected to
 *
 * @return File descriptor of the opened socket through which the communication will take place
 */
int connect_to_udp_server(std::string server, std::string port, std::string& err, struct addrinfo *server_info);

/* Function definitions */

void print_stats(std::map<std::string, int> stats)
{
   for(auto& key_value : stats)
   {
      std::cout << key_value.first << " " << key_value.second << std::endl;
   }
}

int report_to_syslog(std::map<std::string, int> stats, std::string server, std::string hostname, std::string app_name, std::string& err)
{
   /* Set up a connection to the UDP server */
   struct addrinfo host;
   struct sockaddr in;
   host.ai_addr = &in;
   int fd = connect_to_udp_server(server, SYSLOG_PORT, err, &host);
   if(fd < 0)
      return fd;

   /* Prepare the syslog message header */
   std::stringstream ss;
   ss << "<134>1 "; //Priority + version
   struct timeval current_dt;
   gettimeofday(&current_dt, 0);
   struct tm *now = gmtime(&current_dt.tv_sec);
   ss << now->tm_year + 1900 << "-" << now->tm_mon + 1 << "-" << now->tm_mday << "T"; //Date
   ss << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec << ".";
   ss << std::setw(3) << current_dt.tv_usec/1000 << "Z "; //Time
   ss << hostname << " "; //Hostname
   ss << app_name << " - - - "; //Fill in remaining parameters with empty values
   std::string header = ss.str();

   /* Send the statistics in syslog messages */
   for(auto& key_value : stats)
   {
      std::stringstream message_ss;
      message_ss << header;
      message_ss << key_value.first << " " << key_value.second;
      std::string message_string = message_ss.str();
      sendto(fd, message_string.c_str(), strlen(message_string.c_str()), 0, host.ai_addr, host.ai_addrlen);
   }

   close(fd);

   return 0;
}

int connect_to_udp_server(std::string server, std::string port, std::string& err, struct addrinfo *server_info)
{
   int status;
   struct addrinfo hints;
   struct addrinfo *servinfo;
   struct addrinfo *host;
   int fd;

   memset(&hints, 0, sizeof hints);
   hints.ai_family = AF_UNSPEC;
   hints.ai_socktype = SOCK_DGRAM;

   /* Get information about the server */
   if((status = getaddrinfo(server.c_str(), port.c_str(), &hints, &servinfo)) != 0)
   {
      err = gai_strerror(status);
      return -1;
   }

   /* Connect through one of the returned addresses */
   for(host = servinfo; host != NULL; host = host->ai_next)
   {
      if ((fd = socket(host->ai_family, host->ai_socktype, host->ai_protocol)) == -1) 
      {
         continue;
      }

      if (connect(fd, host->ai_addr, host->ai_addrlen) == -1) 
      {
         close(fd);
         continue;
      }

      break;
   }

   if(host == NULL)
   {
      err = "Failed to connect to the server";
      return -1;
   }

   /* Fill in information about the server for communication */
   memcpy(server_info->ai_addr,host->ai_addr, sizeof(struct sockaddr));
   server_info->ai_addrlen = host->ai_addrlen;

   freeaddrinfo(servinfo);

   return fd;
}