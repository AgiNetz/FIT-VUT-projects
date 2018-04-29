/**
 * IPK Project 1: Client-Server application for retrieving user information
 * Author: Tomas Danis
 * Login: xdanis05
 * File: tump.h
 */

/* Trivial User Management Protocol */
#ifndef TUMP_H
#define TUMP_H

#include <stdio.h>
#include <stdarg.h>

#define TUMP_ADDR_ERR 1
#define TUMP_SOCKET_ERR 2
#define TUMP_SOCKOPT_ERR 3
#define TUMP_BIND_ERR 4
#define TUMP_LISTEN_ERR 5
#define TUMP_ACCEPT_ERR 6
#define TUMP_CONNECT_ERR 7
#define TUMP_SENDREQ_ERR 8
#define TUMP_MAX_LOGIN_EXCEEDED_ERR 9
#define TUMP_ERROR_RECEIVED_ERR 10
#define TUMP_ROGUE_SERVER_ERR 11
#define TUMP_INVALID_LOGIN_ERR 12
#define TUMP_UNKNOWN_MESSAGE_ERR 13
#define TUMP_INT_ERR 99

#define EXIT_OK 0
#define ARG_ERR 100

#define TUMP_MSG_REQ 1
#define TUMP_MSG_RESPONSE 2
#define TUMP_MSG_ERROR 3

#define TUMP_REQ_NAME 1
#define TUMP_REQ_HOME 2
#define TUMP_REQ_LIST 3

#define TUMP_ERR_UNKNOWN_MESSAGE 1
#define TUMP_ERR_INVALID_FORMAT 2
#define TUMP_ERR_UNKNOWN_REQUEST 3

/* TUMP message headers structs */

/* TUMP_REQ message header struct */
struct tump_req
{
   char msg_type;
   char req_type;
};

/* TUMP_RESPONSE message header struct */
struct tump_response
{
   char msg_type;
   char success;
};

/* TUMP_ERROR message header struct */
struct tump_error
{
   char msg_type;
   char err_type;
};

/**
 * Prints out an error
 */
void tump_error(char *e, ...)
{
   va_list args;
   va_start (args, e);
   vfprintf (stderr, e, args);
   va_end (args);
}

/**
 * Converts an error code received from TUMP_ERROR message into a string
 */
const char *etos(int error)
{
   switch(error)
   {
      case TUMP_ERR_UNKNOWN_MESSAGE:
         return "Unknown message";
      case TUMP_ERR_INVALID_FORMAT:
         return "Invalid message format";
      case TUMP_ERR_UNKNOWN_REQUEST:
         return "Unknown request";
   }
   return "Unknown error";
}

#endif /* TUMP_H */