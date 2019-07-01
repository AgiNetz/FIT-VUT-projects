/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: dns-export.cpp
 * Description: Main program module. The program analyses incoming DNS traffic on an interface or traffic
 *              captured from a file and collects statistics about the DNS Answer RRs found in the DNS
 *              responses. These statistics are reported either to a syslog server periodically, or printed
 *              out to the standard output on demand.
 */

#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include <csignal>

#include "dns-export.hpp"
#include "sniffer.hpp"

/* Prototypes */

/**
 * Parses arguments of dns-export and saved them into a map
 *
 * The resulting map contains key-value pairs where key is a single char - the short
 * form of the argument in question. The value is a bool, but its value has no meaning. The presence of
 * the key itself indicates presence of the argument. If invalid arguments were found, record with key of
 * '?' can be found in the map.
 *
 * @param argc Number of arguments, passed from main
 * @param argv Argument array, passed from main
 * @param result Map to which to save the arguments
 * @param arg_vals Argument values map for the arguments which take a value
 */
void parse_args(int argc, char *argv[], std::unordered_map<char, bool>& result, std::unordered_map<char, std::string>& arg_vals);

/**
 * Checks the validity of dns-export arguments 
 * 
 * @param args Map containing the parsed arguments, output of parse_args
 * @param arg_vals Map containing argument values, output of parse_args
 * @param err Buffer used for error reporting
 *
 * @return Flag indicating whether the argument checks passed succesfully.
 */
bool check_args(std::unordered_map<char, bool>& args, std::unordered_map<char, std::string>& arg_vals, std::string& err);

/**
 * Signal handler for SIGUSR1 signal
 */
void usr1_handler(int signum);

/**
 * Signal handler for SIGALRM signal
 */
void alarm_handler(int signum);

/* Function definitions */
void usr1_handler(int signum)
{
   (void)signum;
   pcap_breakloop(handle);
   break_flag = BREAK_SIGUSR1;
}

void alarm_handler(int signum)
{
   (void)signum;
   pcap_breakloop(handle);
   break_flag = BREAK_SIGALRM;
}

int main(int argc, char *argv[])
{
   /* Data structures for argument parsing */
   std::unordered_map<char, bool> args;
   std::unordered_map<char, std::string> arg_vals;
   std::string err;
   
   /* Parse and check arguments */
   parse_args(argc, argv, args, arg_vals);
   if(!check_args(args, arg_vals, err))
   {
      std::cerr << err << std::endl;
      return EXIT_ARG_ERR;
   }

   /* Prepare analyzer arguments */
   std::string source;
   bool live;
   std::string logging_server = "";
   unsigned int report_period = 60;

   if(args.count('r'))
   {
      source = arg_vals['r'];
      live = false;
   }
   else
   {
      source = arg_vals['i'];
      live = true;
   }

   if(args.count('s'))
   {
      logging_server = arg_vals['s'];
   }

   if(args.count('t'))
   {
      char *conv_err = NULL;
      report_period = strtoul(arg_vals['t'].c_str(), &conv_err, 10);
      if(*conv_err != 0)
      {
         std::cerr << "Given reporting period value is not a number!" << std::endl;
         return EXIT_ARG_ERR;
      }
   }

   /* Configure signal handling and alarms*/
   signal(SIGUSR1, usr1_handler);
   signal(SIGALRM, alarm_handler);

   return analyze_dns_traffic(source, live, logging_server, report_period);
}

void parse_args(int argc, char *argv[], std::unordered_map<char, bool>& result, std::unordered_map<char, std::string>& arg_vals)
{
   opterr = 0; //Silent

   int arg = 0;
   while((arg = getopt(argc, argv, "r:i:s:t:")) != -1)
   {
      switch(arg)
      {
         case 'r':
         case 'i':
         case 's':
         case 't':
            result.insert({{arg, true}});
            arg_vals.insert({{arg, optarg}});
            break;
         case '?':
            switch(optopt)
            {
               case 'r':
               case 'i':
               case 's':
               case 't':
                  arg_vals.insert({{optopt, ""}});
                  break;
               default:
                  result.insert({{'?', true}});
                  break;
            }
            break;
         default:
            result.insert({{'?', true}});
            break;
      }
   } 
}

bool check_args(std::unordered_map<char, bool>& args, std::unordered_map<char, std::string>& arg_vals, std::string& err)
{
   //.count - Counts the number of occurences of the given key in the map. Effectively 0 or 1, serves as key_exists() function
   if(arg_vals.count('r') && arg_vals['r'] == "")
   {
      err = "Missing .pcap file to process with argument -r!";
   }
   else if(arg_vals.count('i') && arg_vals['i'] == "")
   {
      err = "Missing interface to listen on with argument -i!";
   }
   else if(arg_vals.count('s') && arg_vals['s'] == "")
   {
      err = "Missing Syslog server address with argument -s!";
   }
   else if(arg_vals.count('t') && arg_vals['t'] == "")
   {
      err = "Missing report period with argument -t!";
   }
   else if(args.count('r') == 0 && args.count('i') == 0)
   {
      err = "Either a interface to listen on or a .pcap file to process must be specified!";
   }
   else if(args.count('r') && args.count('i'))
   {
      err = "Cannot capture in both online and offline mode!";
   }
   else if(args.count('?'))
   {
      err = "Unknown argument specified!";
   }
   else
   {
      //Everything ok
      return true;
   }

   return false;
}