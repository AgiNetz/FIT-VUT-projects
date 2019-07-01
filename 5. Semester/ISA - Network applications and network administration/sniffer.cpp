/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: sniffer.cpp
 * Description: Module for capturing packets on an interface or reading them from a .pcap file
 */

#include <pcap.h>
#include <iostream>
#include <map>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include "sniffer.hpp"
#include "dns-export.hpp"
#include "headers.hpp"
#include "dns_parser.hpp"
#include "stats_report.hpp"

/* Prototypes */

/**
 * Opens a packet capture handle for capturing incoming traffic
 *
 * @param source Source of the traffic. Either a interface name or a filename.
 * @param live Indicates where to read the traffic from - interface (true) or a file (false)
 * @param err Buffer used for error reporting
 *
 * @return Newly opened, activated handle if no error occured, NULL otherwise
 */
pcap_t* open_packet_capture_handle(std::string source, bool live, char *err);

/**
 * Sets filters to only capture DNS traffic on a given packet capture handle
 *
 * @param handle Handle on which to filter
 */
void set_dns_filters(pcap_t *handle);

/**
 * Processes a single captured packet. Used as a callback function for pcap_loop
 *
 * Collects statistics about received DNS packets.
 *
 * @param args Arguments passed to this function in the pcap_loop call
 * @param header Contains information about the captured packet
 * @param packet Pointer to the captured packet data
 */
void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

/**
 * Gets the IP address assigned to an interface
 *
 * @param ifname Interface name of which IP address to retrieve
 *
 * @return String containing the IP address address in dotted decimal format
 */
std::string get_if_address(std::string ifname);

/**
 * Gets a name of a single non-loopback interface on the current device
 * 
 * @return Name of a non-loopback interface on the current device. "" if the device has no non-loopback interfaces
 */
std::string get_device_if();

/* Variables */
pcap_t* handle;
int break_flag = 0;

void process_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
   (void)header;
 
   /* Map for collecting statistics */
   std::map<std::string, int> *message_counts = reinterpret_cast<std::map<std::string, int>*>(args);

   /* Extract the DNS message from the packet */
   const u_char *data = prepare_dns_message(packet);
   if(data == NULL)
      return;

   /* Extract answers from the DNS message */
   std::vector<std::string> answers = extract_answers(data);

   /* Count the extracted answers into the statistics */
   for(unsigned int i = 0; i < answers.size(); i++)
   {
      (*message_counts)[answers[i]]++;
   }

   /* Free the message allocated by prepare_dns_message*/
   free((void*)data);
   return;
}

int analyze_dns_traffic(std::string source, bool live, std::string logging_server, unsigned int reporting_period)
{
   /* Setup an alarm to report statistics to the syslog server */
   alarm(reporting_period);

   /* Data preparation */
   std::map<std::string, int> message_counts;
   char err[PCAP_ERRBUF_SIZE];

   /* Open a packet capture handle */
   handle = open_packet_capture_handle(source, live, err);
   if(handle == NULL)
   {
      std::cerr << err << std::endl;
      return EXIT_PCAP_HANDLE_ERR;
   }

   /* Set filters - not used as the filters were filtering IP fragments - undesirable */
   //set_dns_filters(handle);

   /* Check datalink protocol support */
   if(pcap_datalink(handle) != DLT_EN10MB)
   {
      std::cerr << "Given data link layer protocol is not supported!" << std::endl;
      return EXIT_UNSUPPORTED_DATA_LINK_ERR;
   }

   int retval = -1;
   while(retval)
   {
      /* Capture packets indefinitely */
      retval = pcap_loop(handle, 0, process_packet, reinterpret_cast<u_char*>(&message_counts));

      /* Packet capture ended, examine reason */
      if(retval == PCAP_ERROR_BREAK) //Broken using pcap_breakloop
      {
         /* Check break source */
         if(break_flag == BREAK_SIGUSR1) //Broken by SIGUSR1 handler
         {
            print_stats(message_counts);
         }
         else if(break_flag == BREAK_SIGALRM) //Broken by SIGALRM handler
         {
            /* Report to syslog */
            std::string err;
            if(!logging_server.empty())
            {
               if(report_to_syslog(message_counts, logging_server, get_if_address(source), "dns-export", err) < 0)
               {
                  std::cerr << err << std::endl;
               }
            }

            /* Setup the alarm once again */
            alarm(reporting_period);
         }
      }
      else if(retval == 0 && !live) //Packets were read from a file and it has been completely processed
      {
         /* File processing finished, report statistics to syslog server */
         std::string err;
         if(!logging_server.empty())
         {
            /* Get device identification for syslog message */
            std::string device_if = get_device_if();
            std::string hostname;
            if(device_if.empty())
            {
               hostname = "UNKNOWN HOSTNAME";
            }
            else
            {
               hostname = get_if_address(device_if);
            }

            /* Report to syslog */
            if(report_to_syslog(message_counts, logging_server, hostname, "dns-export", err) < 0)
            {
               std::cerr << err << std::endl;
            }
         }
      }
   }

   return 0;
}

pcap_t* open_packet_capture_handle(std::string source, bool live, char *err)
{
   pcap_t *handle = NULL;
   /* Check handle type */
   if(live) //Online capture
   {
      handle = pcap_create(source.c_str(), err);
      if(handle == NULL) return handle;
      pcap_set_promisc(handle, 1);
      pcap_set_immediate_mode(handle, 1);
      if(pcap_activate(handle))
      {
         err = strcpy(err,"Could not activate live capture handle! Try running the application with root privileges.");
         return NULL;
      }
   }
   else //Offline capture
   {
      handle = pcap_open_offline(source.c_str(), err);
   }

   return handle;
}

void set_dns_filters(pcap_t *handle)
{
   char filter[] = "src port 53";
   struct bpf_program fp;
   pcap_compile(handle, &fp, filter, 0, PCAP_NETMASK_UNKNOWN);
   pcap_setfilter(handle, &fp);
   pcap_freecode(&fp);
}

std::string get_if_address(std::string ifname)
{
   int req_fd;
   struct ifreq ifr;

   req_fd = socket(AF_INET, SOCK_DGRAM, 0);
   ifr.ifr_addr.sa_family = AF_INET;
   strncpy(ifr.ifr_name, ifname.c_str(), IFNAMSIZ-1);

   ioctl(req_fd, SIOCGIFADDR, &ifr);

   close(req_fd);

   return inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr);
}

std::string get_device_if()
{
   struct ifaddrs *addrs, *tmp;

   getifaddrs(&addrs);
   tmp = addrs;

   while (tmp)
   {
      if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET && strncmp("lo", tmp->ifa_name, 2) != 0)
      {
         freeifaddrs(addrs);
         return tmp->ifa_name;
      }
      tmp = tmp->ifa_next;
   }

   freeifaddrs(addrs);
   return "";
}