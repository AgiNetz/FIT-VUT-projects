/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: headers.hpp
 * Description: Module for extracting application layer data from captured packets
 */

#ifndef HEADERS_HPP
#define HEADERS_HPP

#include <arpa/inet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

/* Constants */

const uint16_t ETHTYPE_IP = 0x0800;
const uint16_t ETHTYPE_IP6 = 0x86DD;

const uint8_t NEXT_HEADER_TCP = 0x06;
const uint8_t NEXT_HEADER_UDP = 0x11;

/* Structs */

/**
 * Structure representing 802.3 ethernet header
 */
struct ethernet_header 
{
   uint8_t dstmac[6];
   uint8_t srcmac[6];
   uint16_t ethtype;
};

/**
 * Structure representing IPv6 message header
 */
struct ipv6_header 
{
   unsigned int version : 4;
   unsigned int traffic_class : 8;
   unsigned int flow_label : 20;
   uint16_t length;
   uint8_t next_header;
   uint8_t hop_limit;
   struct in6_addr src_add;
   struct in6_addr dst_add;
};

/**
 * Structure representing TCP segment header
 */
struct tcp_header 
{
   uint16_t src_port;
   uint16_t dst_port;
   uint32_t seq_num;
   uint32_t ack_num;
   uint16_t data_offset : 4;
   uint16_t reserved : 3; 
   uint16_t flags : 9;
   uint16_t window_size;
   uint16_t checksum;
   uint16_t urgent;
};

/**
 * Structure representing UDP header
 */ 
struct udp_header
{
   uint16_t src_port;
   uint16_t dst_port;
   uint16_t length;
   uint16_t checksum;
};

/**
 * Structure representing a DNS message header
 */
struct dns_header
{
   uint16_t id;
# if __BYTE_ORDER == __BIG_ENDIAN
   uint16_t qr:1;
   uint16_t opcode:4;
   uint16_t aa:1;
   uint16_t tc:1;
   uint16_t rd:1;
   uint16_t ra:1;
   uint16_t zero:3;
   uint16_t rcode:4;
# elif __BYTE_ORDER == __LITTLE_ENDIAN
   uint16_t rd:1;
   uint16_t tc:1;
   uint16_t aa:1;
   uint16_t opcode:4;
   uint16_t qr:1;
   uint16_t rcode:4;
   uint16_t zero:3;
   uint16_t ra:1;
# endif
   uint16_t q_count; 
   uint16_t ans_count; 
   uint16_t auth_count; 
   uint16_t add_count; 
};

/**
 * Strips L2-L4 headers and extracts the DNS message from the packet
 *
 * In case the message is IP fragmented or TCP segmented, this function
 * returns NULL and saves the individual parts for later use. When all parts
 * of a fragmented/segmented message have been received, it reconstructs the message
 * and returns a pointer to the complete DNS message.
 *
 * @param packet Pointer to the captured packet
 *
 * @return Pointer to the extracted DNS message. The DNS message is copied into a
 *         newly allocated buffer and it is the responsibility of the caller to free
 *         it later.
 */
const u_char *prepare_dns_message(const u_char *packet);

#endif