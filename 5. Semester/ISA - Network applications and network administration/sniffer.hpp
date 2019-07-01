/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: sniffer.hpp
 * Description: Module for capturing packets on an interface or reading them from a .pcap file
 */

#ifndef SNIFFER_HPP
#define SNIFFER_HPP

#include <pcap.h>

/* Constants */
const int BREAK_SIGUSR1 = 1; //Value for break_flag if pcap_loop was broken by SIGUSR1 signal
const int BREAK_SIGALRM = 2; //Value for break_flag if pcap_loop was broken by SIGALRM signal

/* Prototypes */

/**
 * Analyzes DNS traffic coming on an interface or read from a .pcap file and reports statistics about it
 *
 * @param source Source of the traffic - either a filename or a interface name
 * @param live Flag indicating whether the traffic will be read from a file (false) or a interface (true)
 * @param logging_server Hostname/address of a syslog server the statistics will be reported to. "" if
 *                       you do not want to report to syslog server.
 * @param reporting_period Period in seconds specifying how often will the statistics be reported to the syslog server
 *
 * @return Status value indicating success of the operation. 0 if no error occured, != 0 otherwise.
 */
int analyze_dns_traffic(std::string source, bool live, std::string logging_server, unsigned int reporting_period);

/* Variables */
extern pcap_t *handle;
extern int break_flag;

#endif