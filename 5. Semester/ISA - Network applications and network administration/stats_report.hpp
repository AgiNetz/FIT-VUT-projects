/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: stats_report.hpp
 * Description: Module for reporting statistics from a map to various destinations
 */

#ifndef STATS_REPORT_HPP
#define STATS_REPORT_HPP

#include <map>
#include <string>

/* Prototypes */

/**
 * Prints the statistics to standard output
 *
 * @param stats Map containing the statistics to print
 */
void print_stats(std::map<std::string, int> stats);

/**
 * Sends the statitistics to a syslog server
 *
 * @param stats Map containing the statistics to report
 * @param server Hostname/address of the syslog server
 * @param hostname Hostname of this device. Used in syslog message for sender identification
 * @param app_name Name of the application sending the syslog messages
 * @param err Buffer used for error reporting
 *
 * @return 0 if everything worked properly, > 0 if there has been an error. In case of an error, err is filled with its description
 */ 
int report_to_syslog(std::map<std::string, int> stats, std::string server, std::string hostname, std::string app_name, std::string& err);

#endif