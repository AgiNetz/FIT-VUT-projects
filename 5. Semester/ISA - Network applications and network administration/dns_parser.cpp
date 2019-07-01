/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: dns_parser.cpp
 * Description: Module for parsing DNS messages and extracting information from them
 */

#include <pcap.h>
#include <iostream>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iomanip>
#include <sstream>
#include <cstring>

#include "dns_parser.hpp"
#include "base64.hpp"

/* Prototypes */

/**
 * Parses a domain name as found in DNS records into a string containing the whole domain name
 *
 * This function supports DNS message compression as described in RFC 1035 and is able to parse it 
 * successfully.
 *
 * @param packet Pointer to the beginning of the whole DNS message. Used in domain name decompression
 * @param name_start Pointer to the beginning of the parsed domain name
 * @param name_length [out] Contains the amount of bytes the domain name takes in the memory
 *                    starting at the point where name_start points. This value can be used to
 *                    skip the domain name and read the next field in the DNS message
 * 
 * @return String containing the parsed domain name
 */
std::string parse_qname(const u_char *packet, const u_char *name_start, int *name_length);

/**
 * Gets the RR type string representation as specified in RFC 3597, Section 5
 *
 * @param type_names Map containing the type names
 * @param type RR type to get the representation of
 *
 * @return String containing the RR type representation
 */
std::string extract_type_name(std::map<int, std::string> type_names, uint16_t type);

/* Following functions each parse their respective Answer RR type and return a string
   containing its representation in accordance with their respective RFCs*/
std::string parse_a_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_aaaa_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_ns_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_mx_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_soa_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_ptr_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_cname_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_txt_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_srv_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_dnskey_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_rrsig_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_nsec_record(const u_char *packet, const u_char *record, uint16_t rdata_len);
std::string parse_ds_record(const u_char *packet, const u_char *record, uint16_t rdata_len);

/* Variables*/
static std::map<int, std::string> type_names{ //Map containing string representation of RR types
   {1, "A"},
   {2, "NS"},
   {5, "CNAME"},
   {6, "SOA"},
   {12, "PTR"},
   {15, "MX"},
   {16, "TXT"},
   {28, "AAAA"},
   {33, "SRV"},
   {43, "DS"},
   {46, "RRSIG"},
   {47, "NSEC"},
   {48, "DNSKEY"}}; 

std::vector<std::string> extract_answers(const u_char *packet)
{
   /* Callback map used for parsing individual RRs */
   static std::map<int, std::string(*)(const u_char*, const u_char*, uint16_t)> type_callbacks{
      {1, parse_a_record},
      {2, parse_ns_record},
      {5, parse_cname_record},
      {6, parse_soa_record},
      {12, parse_ptr_record},
      {15, parse_mx_record},
      {16, parse_txt_record},
      {28, parse_aaaa_record},
      {33, parse_srv_record},
      {43, parse_ds_record},
      {46, parse_rrsig_record},
      {47, parse_nsec_record},
      {48, parse_dnskey_record}}; 

   /* Parse DNS header */
   std::vector<std::string> answers;
   const struct dns_header *dns_head = reinterpret_cast<const struct dns_header*>(packet);
   
   /* Check whether this is a DNS response */
   if(ntohs(dns_head->qr) == 0) //Query
     return answers;

   /* Get to the DNS records in the message */
   const u_char *dns_records = packet + sizeof(struct dns_header);

   /* Skip queries */
   for(int i = 0; i < ntohs(dns_head->q_count); i++)
   {
      int name_length = 0;
      parse_qname(packet, dns_records, &name_length);
      dns_records += name_length + sizeof(struct query_format);
   }

   /* Parse answer RRs */
   for(int i = 0; i < ntohs(dns_head->ans_count); i++)
   {
      int name_length = 0;
      /* Parse domain name of the RR */
      std::string query_name = parse_qname(packet, dns_records, &name_length);
      query_name.pop_back();

      /* Parse RR header */
      const struct answer_format *answer_record = reinterpret_cast<const struct answer_format*>(dns_records + name_length);

      /* Check if this is a recognized RR type */
      std::string type_string = type_names[ntohs(answer_record->type)];
      if(!type_string.empty())
      {
         /* Parse RR RDATA */
         std::string rdata = type_callbacks[htons(answer_record->type)](packet, dns_records+name_length+sizeof(struct answer_format), ntohs(answer_record->length));
         std::string result = query_name + " " + type_string + " " + rdata;
         answers.push_back(result);
      }

      /* Skip to the next RR */
      dns_records += name_length + sizeof(struct answer_format) + ntohs(answer_record->length);
   }

   return answers;
}

std::string parse_qname(const u_char *packet, const u_char *name_start, int *name_length)
{
   *name_length = 0;
   uint8_t length_octet = *(reinterpret_cast<const uint8_t*>(name_start));

   /* Recursion end condition, found null octet terminating the domain name */
   if(length_octet == 0)
   {
      *name_length = 1;
      return "";
   }

   std::string name;

   /* Identify the next content - pointer or a label */
   uint8_t content_identifier = length_octet >> 6;
   if(content_identifier == 0x03)
   {
      /* Value is a pointer */
      /* Read the pointer and continue reading from there */
      uint16_t offset = ntohs(*(reinterpret_cast<const uint16_t*>(name_start)));
      offset &= 0x3FFF;
      name = parse_qname(packet, reinterpret_cast<const u_char*>(packet + offset), name_length);
      *name_length = 2;
   }
   else
   {
      /* Value is a label */
      name_start++;
      
      /* Read the label */
      name.assign(reinterpret_cast<const char *>(name_start), length_octet);
      name += '.';
      *name_length = length_octet + 1;

      /* Continue reading the domain name */
      int next_name_length = 0;
      name += parse_qname(packet, reinterpret_cast<const u_char*>(name_start + length_octet), &next_name_length);
      *name_length += next_name_length;
   }

   return name;
}

std::string extract_type_name(std::map<int, std::string> types, uint16_t type)
{
   std::string result = types[type];
   if(result.empty())
   {
      std::stringstream ss;
      ss << "TYPE" << type;
      return ss.str();
   }
   return result;
}

std::string parse_a_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)packet;
   (void)rdata_len;
   const struct a_rdata *rdata = reinterpret_cast<const struct a_rdata*>(record);
   struct in_addr addr;
   addr.s_addr = rdata->address;
   return inet_ntoa(addr);
}

std::string parse_ns_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)rdata_len;
   int len;
   std::string tmp = parse_qname(packet, record, &len);
   tmp.pop_back();
   return tmp;
}

std::string parse_cname_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)rdata_len;
   int len;
   std::string tmp = parse_qname(packet, record, &len);
   tmp.pop_back();
   return tmp;
}

std::string parse_aaaa_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)rdata_len;
   (void)packet;
   char result[INET6_ADDRSTRLEN];
   struct in6_addr addr;
   memcpy(addr.s6_addr, record, 16);
   std::string str = inet_ntop(AF_INET6, &addr, result, INET6_ADDRSTRLEN);
   return str;
}

std::string parse_mx_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)rdata_len;
   const struct mx_rdata *rdata = reinterpret_cast<const struct mx_rdata*>(record);
   std::stringstream ss;
   ss << ntohs(rdata->preference) << " ";
   int len;
   std::string tmp = parse_qname(packet, record+sizeof(struct mx_rdata), &len);
   tmp.pop_back();
   ss << tmp;
   return ss.str();
}

std::string parse_soa_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)rdata_len;
   std::stringstream ss;
   int len;
   std::string tmp = parse_qname(packet, record, &len);
   tmp.pop_back();
   ss << tmp << " ";
   int len2;
   tmp = parse_qname(packet, record + len, &len2);
   tmp.pop_back();
   ss << tmp << " ";
   const struct soa_rdata *rdata = reinterpret_cast<const struct soa_rdata*>(record + len + len2);
   ss << ntohl(rdata->serial) << " ";
   ss << ntohl(rdata->refresh) << " ";
   ss << ntohl(rdata->retry) << " ";
   ss << ntohl(rdata->expire) << " ";
   ss << ntohl(rdata->minimum);
   return ss.str();
}

std::string parse_ptr_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)rdata_len;
   int len;
   std::string tmp = parse_qname(packet, record, &len);
   tmp.pop_back();
   return tmp;
}

std::string parse_txt_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)rdata_len;
   int len;
   std::string tmp = parse_qname(packet, record, &len);
   tmp.pop_back();
   return tmp;
}

std::string parse_srv_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)rdata_len;
   const struct srv_rdata *rdata = reinterpret_cast<const struct srv_rdata*>(record);
   std::stringstream ss;
   ss << ntohs(rdata->priority) << " ";
   ss << ntohs(rdata->weight) << " ";
   ss << ntohs(rdata->port) << " ";
   int len;
   std::string tmp = parse_qname(packet, record+sizeof(struct srv_rdata), &len);
   tmp.pop_back();
   ss << tmp;
   return ss.str();
}

std::string parse_dnskey_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)packet;
   const struct dnskey_rdata *rdata = reinterpret_cast<const struct dnskey_rdata*>(record);
   std::stringstream ss;
   ss << ntohs(rdata->flags) << " ";
   ss << static_cast<uint16_t>(rdata->protocol) << " ";
   ss << static_cast<uint16_t>(rdata->algorithm) << " ";
   ss << base64_encode(record + sizeof(struct dnskey_rdata), rdata_len - sizeof(struct dnskey_rdata));
   return ss.str();
}

std::string parse_rrsig_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   const struct rrsig_rdata *rdata = reinterpret_cast<const struct rrsig_rdata*>(record);
   std::stringstream ss;
   ss << extract_type_name(type_names, ntohs(rdata->type_covered)) << " ";
   ss << static_cast<uint16_t>(rdata->algorithm) << " ";
   ss << static_cast<uint16_t>(rdata->labels) << " ";
   ss << ntohl(rdata->original_ttl) << " ";
   ss << ntohl(rdata->signature_expiration) << " ";
   ss << ntohl(rdata->signature_inception) << " ";
   ss << ntohs(rdata->key_tag) << " ";
   int len;
   std::string tmp = parse_qname(packet, record+sizeof(struct rrsig_rdata), &len);
   tmp.pop_back();
   ss << tmp << " ";
   ss << base64_encode(record + sizeof(struct rrsig_rdata) + len, rdata_len - sizeof(struct rrsig_rdata) - len);
   return ss.str();
}

std::string parse_nsec_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   std::stringstream ss;
   int len;
   std::string tmp = parse_qname(packet, record, &len);
   tmp.pop_back();
   ss << tmp;

   /* Read the type bitmap fields */
   const u_char *message_end = record + rdata_len;
   const u_char *reading_head = record + len;
   while(reading_head != message_end)
   {
      uint8_t window = reading_head[0];
      uint8_t bitmap_length = reading_head[1];
      for(int i = 0; i < bitmap_length; i++)
      {
         uint8_t octet = reading_head[2 + i];
         for(int j = 7; j >= 0; j--)
         {
            uint8_t mask = 0x01 << j;
            uint8_t bit = octet & mask;
            if(bit)
            {
               uint16_t bit_val = (window << 8) + 8*i + (8 - j) - 1;
               ss << " " << extract_type_name(type_names, bit_val);
            } 
         }
      }
      reading_head += 2 + bitmap_length;
   }
   return ss.str();
}

std::string parse_ds_record(const u_char *packet, const u_char *record, uint16_t rdata_len)
{
   (void)packet;
   const struct ds_rdata *rdata = reinterpret_cast<const struct ds_rdata*>(record);
   std::stringstream ss;
   ss << ntohs(rdata->key_tag) << " ";
   ss << static_cast<uint16_t>(rdata->algorithm) << " ";
   ss << static_cast<uint16_t>(rdata->digest_type) << " ";

   const u_char *digest = packet + sizeof(struct ds_rdata);
   for(unsigned int i = 0; i < rdata_len - sizeof(struct ds_rdata); i++)
   {
      ss << std::hex << static_cast<uint16_t>(digest[i]);
   }
   return ss.str();
}