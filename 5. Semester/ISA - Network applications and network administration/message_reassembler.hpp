/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: message_reassembler.hpp
 * Description: Module for reassembling multiple part messages which may arrive out of order
 */

#ifndef MESSAGE_REASSEMBLER_HPP
#define MESSAGE_REASSEMBLER_HPP

#include <map>
#include <pcap.h>

/* Structs */

/**
 * Structure representing a single part of a multi-part message
 */
struct message_part
{
   int part_size;    /* Size of this part in bytes */
   u_char *message;  /* Pointer to the message part */
};

/**
 * Structure containing metadata about saved messages
 */
struct message_mtdata
{
   int message_size;          /* Expected size of the completed message */
   bool last_part_received;   /* Flag indicating whether last part of a message has been received */
};

/* Classes */

/**
 * @class Class for reassembling multiple part messages which may arrive out of order
 */ 
class MessageReassembler
{
public:
   MessageReassembler();
   ~MessageReassembler();

   /**
    * Saves a part of a message for future reconstruction
    *
    * @param message_id Identification of the message to which this part belongs to
    * @param part_id Identification of this part of the message. This value should be the offset
    *                of this part from the message beginning in bytes
    * @param part_size Size of this message part
    * @param message message Pointer to the data of this part
    * @param last Flag indicating whether this part is the last part of the complete message
    */
   void save_message_part(int message_id, int part_id, int part_size, const u_char *message, bool last);

   /**
    * Sets the expected size of the complete message. This value is used when checking for message completeness
    *
    * @param message_id Identification of the message whose size we are setting
    * @param size Expected size of the complete message
    */
   void set_message_size(int message_id, int size);

   /**
    * Gets the expected size of the complete message which has been previously set by set_message_size
    *
    * @param Identification of the message in question
    * 
    * @return Expected size of the complete message
    */
   int get_complete_message_size(int message_id);

   /**
    * Checks whether a message has all parts saved and is ready for reconstruction
    * 
    * If the expected message size hs been set by set_message_size, then the combined
    * size of all message parts must be equal to this size. Apart from that, this method checks
    * if the message has received its last part and whether the parts are contiguous e.g
    * part ID + part size = next part ID.
    *
    * @param message_id Idenfitication of the message to check
    *
    * @return Flag indicating whether the message is complete and ready to be reconstructed using get_message
    */
   bool is_complete(int message_id);

   /**
    * Gets the complete message reconstructed from saved parts
    *
    * @param message_id Identification of the message to reconstruct
    *
    * @return Pointer to a newly allocated buffer containing the reconstructed message.
    *         The size of this buffer is equal to the combined size of all message parts.
    *         It is the responsibility of the caller to free this buffer later on.
    */
   const u_char *get_message(int message_id);

   /**
    * Clears all saved data about the given message - all its parts, its size are deleted.
    *
    * After the call of this function, a new message under the same message ID can be saved
    *
    * @param message_id Identification of the message to clear
    */
   void clear_message(int message_id);

private:
   std::map<int, std::map<int, struct message_part>> messages; //Map containing all message parts of all messages
   std::map<int, struct message_mtdata> message_metadata; //Map containing metadata for all saved messages
};

#endif