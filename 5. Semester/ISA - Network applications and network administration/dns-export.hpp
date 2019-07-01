/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: dns-export.hpp
 * Description: Main program module. The program analyses incoming DNS traffic on an interface or traffic
 *              captured from a file and collects statistics about the DNS Answer RRs found in the DNS
 *              responses. These statistics are reported either to a syslog server periodically, or printed
 *              out to the standard output on demand.
 */

#ifndef DNS_EXPORT_HPP
#define DNS_EXPORT_HPP

/* Constants */
const int EXIT_ARG_ERR = 10;
const int EXIT_PCAP_HANDLE_ERR = 11;
const int EXIT_PCAP_DEVICE_ERR = 12;
const int EXIT_UNSUPPORTED_DATA_LINK_ERR = 13;

#endif