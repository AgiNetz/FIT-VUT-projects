/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: dns_parser.hpp
 * Description: Module for parsing DNS messages and extracting information from them
 */

#ifndef DNS_PARSER_HPP
#define DNS_PARSER_HPP

#include <vector>
#include <string>

#include "headers.hpp"

/* Structures */

/**
 * Structure representing the format of a Query RR in DNS messages 
 */
struct query_format
{
   //Name - variable length
   uint16_t type;
   uint16_t qclass;
};

/**
 * Structure representing the format of an Answer RR in DNS messages
 */
struct answer_format
{
   //Name - variable length
   uint16_t type;
   uint16_t qclass;
   uint32_t ttl;
   uint16_t length;
   //R data
} __attribute__ ((packed));

/* Following structures represent RDATA portion of the DNS message
   of various DNS messages types. */

struct a_rdata
{
   uint32_t address;
};

struct aaaa_rdata
{
   uint8_t address[16];
};

struct cname_rdata
{
   u_char *cname;
};

struct mx_rdata
{
   uint16_t preference;
   //u_char *domain_name;
} __attribute__ ((packed));

struct ns_rdata
{
   u_char *domain_name;
}; 

struct ptr_rdata
{
   u_char *domain_name;
};

struct soa_rdata
{
   //u_char *mname;
   //u_char *rname;
   uint32_t serial;
   uint32_t refresh;
   uint32_t retry;
   uint32_t expire;
   uint32_t minimum;
} __attribute__ ((packed));

struct txt_rdata
{
   u_char *txt_data;
};

struct srv_rdata
{
   uint16_t priority;
   uint16_t weight;
   uint16_t port;
   //u_char *target;
} __attribute__ ((packed));

struct dnskey_rdata
{
   uint16_t flags;
   uint8_t protocol;
   uint8_t algorithm;
   //u_char *base64_key;
} __attribute__ ((packed));

struct rrsig_rdata
{
   uint16_t type_covered;
   uint8_t algorithm;
   uint8_t labels;
   uint32_t original_ttl;
   uint32_t signature_expiration;
   uint32_t signature_inception;
   uint16_t key_tag;
   //u_char *signer_name;
   //u_char *base64_key;
} __attribute__ ((packed)); 

struct nsec_rdata
{
   //u_char *next_domain_name;
   //u_char *type_bit_maps;
} __attribute__ ((packed));

struct ds_rdata
{
   uint16_t key_tag;
   uint8_t algorithm;
   uint8_t digest_type;
   //u_char *digest;
} __attribute__ ((packed));

/* Prototypes */

/**
 * Extracts all Answer RRs from the given DNS message.
 *
 * @param packet Pointer pointing to the beginning of the DNS message
 *
 * @return vector of strings, each string representing a single Answer RR. The strings
 *         are appropriately formatted according to their respective RFCs
 */
std::vector<std::string> extract_answers(const u_char *packet);

#endif