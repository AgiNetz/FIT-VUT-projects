/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: headers.cpp
 * Description: Module for extracting application layer data from captured packets
 */

#include <pcap.h>
#include <arpa/inet.h>
#include <iostream>

#include "headers.hpp"
#include "message_reassembler.hpp"

/* Function definitions */

const u_char *prepare_dns_message(const u_char *packet)
{
   /* Data prep */
   static MessageReassembler ip_packets;
   static MessageReassembler tcp_segments;
   static MessageReassembler udp_segments;
   size_t transport_protocol = 0;
   size_t data_offset = sizeof(struct ethernet_header);
   int l4_size = 0;

   /* Parse L2 header */
   const struct ethernet_header *eth_head = reinterpret_cast<const struct ethernet_header*>(packet);
   uint16_t ethtype = ntohs(eth_head->ethtype);

   /* Parse L3 header */
   if(ethtype == ETHTYPE_IP)
   {
      /* Parse IP header */
      const struct iphdr *ip_head = reinterpret_cast<const struct iphdr*>(packet + data_offset);
      int ip_head_size = ip_head->ihl*4;
      data_offset += ip_head_size;

      /* Read flags and fragmentation data */
      uint16_t flags_offset = ntohs(ip_head->frag_off);
      uint16_t more_fragments = flags_offset & 0x2000;
      uint16_t offset = (flags_offset & 0x1FFF)*8;

      /* Reassemble IP fragments, if the message was fragmented */
      l4_size = ntohs(ip_head->tot_len) - ip_head_size;
      ip_packets.save_message_part(ip_head->id, offset, l4_size, packet + data_offset, !more_fragments);
      if(offset || more_fragments)
      {
         if(ip_packets.is_complete(ip_head->id))
            packet = ip_packets.get_message(ip_head->id);
         else
            return NULL;
      }
      else
      {
         packet = ip_packets.get_message(ip_head->id);
      }

      ip_packets.clear_message(ip_head->id);
      transport_protocol = ip_head->protocol;
   }
   else if(ethtype == ETHTYPE_IP6)
   {
      /* Parse IPv6 header */
      const struct ipv6_header *ip6_head = reinterpret_cast<const struct ipv6_header*>(packet + data_offset);
      data_offset += sizeof(struct ipv6_header);
      l4_size = ntohs(ip6_head->length);
      transport_protocol = ip6_head->next_header;
   }
   else
   {
      return NULL;
   }

   /* Parse L4 header */
   if(transport_protocol == NEXT_HEADER_TCP)
   {
      /* Parse TCP header */
      const struct tcphdr *tcp_head = reinterpret_cast<const struct tcphdr*>(packet);

      /* Filter nonDNS packets */
      if(ntohs(tcp_head->source) != 53) return NULL;

      /* Reassemble TCP segments, if the message was segmented */
      if(tcp_segments.get_complete_message_size(tcp_head->dest) == 0)
      {
         data_offset = tcp_head->doff*4 + 2;
         uint16_t dns_length = ntohs(*reinterpret_cast<const uint16_t *>(packet + data_offset - 2));
         tcp_segments.set_message_size(tcp_head->dest, dns_length);
         tcp_segments.save_message_part(tcp_head->dest, ntohl(tcp_head->seq) + 2, l4_size - data_offset, packet + data_offset, true);
      }
      else
      {
         data_offset = tcp_head->doff*4;
         tcp_segments.save_message_part(tcp_head->dest, ntohl(tcp_head->seq), l4_size - data_offset, packet + data_offset, true);
      }

      /* Free the buffer received from IP fragments reassembling */
      if(ethtype == ETHTYPE_IP)
      {
         free((void*)packet);
      }

      if(tcp_segments.is_complete(tcp_head->dest))
      {
         packet = tcp_segments.get_message(tcp_head->dest);
         tcp_segments.clear_message(tcp_head->dest);
         return packet;
      }
      else
      {
         return NULL;
      }
   }
   else if(transport_protocol == NEXT_HEADER_UDP)
   {
      /* Parse UDP header */
      const struct udp_header *udp_head = reinterpret_cast<const struct udp_header*>(packet);
      data_offset = sizeof(struct udp_header);

      /* Filter nonDNS packets */
      if(ntohs(udp_head->src_port) != 53) return NULL;

      /* Reassemble UDP message */
      /* This step is not actually necessary, but doing it copies the message to an internal buffer
         so it is done for return value consistency */
      udp_segments.save_message_part(udp_head->src_port, 0, l4_size - data_offset, packet + data_offset, true);
      packet = udp_segments.get_message(udp_head->src_port);
      udp_segments.clear_message(udp_head->src_port);
      return packet;
   }
   else
   {
      return NULL;
   }
}