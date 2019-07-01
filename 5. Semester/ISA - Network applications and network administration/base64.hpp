/**
 * Author: Tomas Danis
 * Login: xdanis05
 * Module: base64.hpp
 * Description: Module for working with base64 data and converting from/to base64 format
 */

#ifndef BASE_64_HPP
#define BASE_64_HPP

/* Prototypes */

/**
 * Encodes binary data as Base64 string
 *
 * @param message The data to be encoded
 * @param message_len Length of the data pointed to by message
 *
 * @return Returns the base64 encoding of data pointed to by message
 */
std::string base64_encode(const uint8_t *message, int message_len);

#endif