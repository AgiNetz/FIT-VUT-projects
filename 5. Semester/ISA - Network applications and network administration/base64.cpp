/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: base64.cpp
 * Description: Module for working with base64 data and converting from/to base64 format
 */

#include <string>
#include <stdint.h>

/* Variables */

static std::string base64_enconding_table =
               "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
               "abcdefghijklmnopqrstuvwxyz"
               "0123456789+/";

/* Prototypes */

/**
 * Encodes 24bit data chunk into 4 base64 characters
 *
 * @param data_chunk Chunk of data to encode
 *
 * @return String containing 4 characters - the base64-encoded data_chunk
 */
std::string base64_encode_datachunk(uint32_t data_chunk);

/* Function definitions */

std::string base64_encode(const uint8_t *message, int message_len)
{
   std::string result;

   /* Split data into 24bit chunks and encode into 4 base64 characters */
   for(int i = 0; i < message_len/3; i+=3)
   {
      uint8_t first, second, third;
      first = message[i];
      second = message[i + 1];
      third = message[i + 2];

      uint32_t data_chunk = (first << 16) + (second << 8) + third; //24 bit data chunk

      result += base64_encode_datachunk(data_chunk);
   }

   /* Process remaining data - 8 or 16bit data chunks, pad with zero to 24bit */
   if(message_len % 3 == 1) //8 bit of data
   {
      uint32_t data_chunk = message[message_len - 1] << 16;
      result += base64_encode_datachunk(data_chunk);
      result[result.size() - 1] = '=';
      result[result.size() - 2] = '=';
   }
   else //16 bit of data
   {
      uint32_t data_chunk = (message[message_len - 1] << 16) + (message[message_len - 2] << 8);
      result += base64_encode_datachunk(data_chunk);
      result[result.size() - 1] = '=';
   }
   
   return result;
}

std::string base64_encode_datachunk(uint32_t data_chunk)
{
   std::string result;
   for(int j = 3; j >= 0; j--)
   {
      uint32_t mask = 0x0000003F << 6*j;
      uint8_t encoded_char = (data_chunk & mask) >> 6*j;
      result += base64_enconding_table[encoded_char];
   }
   return result;
}