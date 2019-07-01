/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: message_reassembler.cpp
 * Description: Module for reassembling multiple part messages which may arrive out of order
 */

#include <cstring>
#include <iostream>

#include "message_reassembler.hpp"

/* Function definitions */

MessageReassembler::MessageReassembler() {}
MessageReassembler::~MessageReassembler() 
{
   for(auto& message : messages)
   {
      clear_message(message.first);
   }
}

void MessageReassembler::save_message_part(int message_id, int part_id, int part_size, const u_char *message, bool last)
{
   struct message_part part;
   
   /* Copy the message to internal buffer */
   part.message = reinterpret_cast<u_char *>(malloc(part_size));
   if(part.message == NULL)
      return;
   memcpy(part.message, message, part_size);
   
   /* Save the message */
   part.part_size = part_size;
   messages[message_id][part_id] = part;

   /* Initialize message metadata */
   if(!message_metadata.count(message_id))
   {
      message_metadata[message_id].message_size = 0;
      message_metadata[message_id].last_part_received = 0;
   }
   if(last)
      message_metadata[message_id].last_part_received = true;
}

void MessageReassembler::set_message_size(int message_id, int size)
{
   this->message_metadata[message_id].message_size = size;
}

int MessageReassembler::get_complete_message_size(int message_id)
{
   if(message_metadata.count(message_id))
      return this->message_metadata[message_id].message_size;
   return 0;
}

bool MessageReassembler::is_complete(int message_id)
{
   int previous_end = messages[message_id].begin()->first; //No 0th message, initialize to initial value
   int size = this->get_complete_message_size(message_id);

   /* Check whether the message parts are contiguous */
   for(auto &part : messages[message_id])
   {
      if(part.first != previous_end)
      {
         return false;
      }
      previous_end = part.first + part.second.part_size;
      size -= part.second.part_size; //Count the size of the message
   }

   /* Check whether the message is correct size and if the final part has been received */
   if(size > 0 || !message_metadata[message_id].last_part_received)
   {
      return false;
   }

   /* All checks passed, message is complete */
   return true;
}

const u_char *MessageReassembler::get_message(int message_id)
{
   /* Get message parts size */
   int size = 0;
   for(auto &part : messages[message_id])
   {
      size += part.second.part_size;
   }

   /* Allocate complete message buffer */
   u_char *message = reinterpret_cast<u_char *>(malloc(size));
   if(message == NULL)
      return NULL;

   /* Concatenate the message part to form the complete message */
   int copied_bytes = 0;
   for(auto &part : messages[message_id])
   {
      memcpy(message + copied_bytes, part.second.message, part.second.part_size);
      copied_bytes += part.second.part_size;
   }

   return message;
}

void MessageReassembler::clear_message(int message_id)
{
   /* Delete all message parts and clear message metadata */
   for(auto &part : messages[message_id])
   {
      free(part.second.message);
   }
   messages.erase(message_id);
   message_metadata.erase(message_id);
}