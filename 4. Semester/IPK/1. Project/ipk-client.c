/**
 * IPK Project 1: Client-Server application for retrieving user information
 * Author: Tomas Danis
 * Login: xdanis05
 * File: ipk-client.c
 */
#include "tump.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <limits.h>

#define USAGE "Usage: %s -h <host> -p <port> [-n|-f|-l] login\n"
#define BUFFER_SIZE 1500
#define MAX_LOGIN_LENGTH 256


int main(int argc, char *argv[])
{
	opterr = 0; //getopt silent
	char *host = NULL;
	char *port = NULL;
	bool nflag = false;
	bool fflag = false;
	bool lflag = false;
	char *login = NULL;
	char req_type = 0;

	int opt;
	/* Process arguments and set appropriate flags */
	while((opt = getopt(argc, argv, "h:p:n:f:l::")) != -1)
	{
		switch(opt)
		{
			case 'h':
				if(host != NULL)
				{
					fprintf(stderr,"Duplicate argument -h!\n");
					fprintf(stderr, USAGE, argv[0]);
					return ARG_ERR;
				}
				host = optarg;

				break;
			case 'p':
				if(port != NULL)
				{
					fprintf(stderr,"Duplicate argument -p!\n");
					fprintf(stderr, USAGE, argv[0]);
					return ARG_ERR;
				}
				port = optarg;
				break;
			case 'n':
				if(nflag)
				{
					fprintf(stderr,"Duplicate argument -n!\n");
					fprintf(stderr, USAGE, argv[0]);
					return ARG_ERR;
				}
				nflag = true;
				login = optarg;
				break;
			case 'f':
				if(fflag)
				{
					fprintf(stderr,"Duplicate argument -f!\n");
					fprintf(stderr, USAGE, argv[0]);
					return ARG_ERR;
				}
				fflag = true;
				login = optarg;
				break;
			case 'l':
				if(lflag)
				{
					fprintf(stderr,"Duplicate argument -l!\n");
					fprintf(stderr, USAGE, argv[0]);
					return ARG_ERR;
				}
				lflag = true;
				/* :: option does not store the value into optarg, read it ourselves */
				if(optind != argc && argv[optind][0] != '-')
				{
					login = argv[optind++];
				}
				break;
			case '?':
				switch(optopt)
				{
					case 'h':
						fprintf(stderr, "Host address was not specified!\n");
						fprintf(stderr, USAGE, argv[0]);
						return ARG_ERR;
						break;
					case 'p':
						fprintf(stderr, "Port to connect to was not specified!\n");
						fprintf(stderr, USAGE, argv[0]);
						return ARG_ERR;
						break;
					case 'n': case 'f': case 'l':
						fprintf(stderr, "Login of the requested user was not specified!\n");
						fprintf(stderr, USAGE, argv[0]);
						return ARG_ERR;
						break;
					default:
						fprintf(stderr, "Unknown option!\n");
						fprintf(stderr, USAGE, argv[0]);
						return ARG_ERR;
						break;
				}
				break;
			default:
				fprintf(stderr, USAGE, argv[0]);
				return ARG_ERR;
				break;
		}
	}

	if(argc != optind)
	{
		fprintf(stderr, "Unrecognized arguments!\n");
		fprintf(stderr, USAGE, argv[0]);
		return ARG_ERR;
	}

	if(host == NULL)
	{
		fprintf(stderr, "Host address was not specified!\n");
		fprintf(stderr, USAGE, argv[0]);
		return ARG_ERR;
	}

	if(port == NULL)
	{
		fprintf(stderr, "Port to connect to was not specified!\n");
		fprintf(stderr, USAGE, argv[0]);
		return ARG_ERR;
	}

	/* Determine request type */
	if(nflag && !fflag && !lflag)
	{
		req_type = TUMP_REQ_NAME;
	}
	else if(!nflag && fflag && !lflag)
	{
		req_type = TUMP_REQ_HOME;
	}
	else if(!nflag && !fflag && lflag)
	{
		req_type = TUMP_REQ_LIST;
	}
	else
	{
		fprintf(stderr, "You must specify exactly one of the [-n|-f|-l] arguments!\n");
		fprintf(stderr, USAGE, argv[0]);
		return ARG_ERR;
	}

	int success;
	struct addrinfo hints, *result, *it;
	int connect_socket = -1;

	memset(&hints, 0, sizeof hints);
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0; //Any
	hints.ai_family = AF_UNSPEC;

	/* Get information about the server */
	if((success = getaddrinfo(host, port, &hints, &result)) != 0)
	{
		tump_error("Error: getaddrinfo: %s\n", gai_strerror(success));
		return TUMP_ADDR_ERR;
	}

	/* Connect to the first possible address */
	for(it = result; it != NULL; it = it->ai_next)
	{
		connect_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if(connect_socket < 0)
		{
			tump_error("Socket could not have been created! Trying another address..\n");
			continue;
		}

		if(connect(connect_socket, result->ai_addr, result->ai_addrlen) < 0)
		{
			tump_error("Could not connect to the server!\n");
			return TUMP_CONNECT_ERR;
		}

        break;
	}

	if(connect_socket < 0)
	{
		tump_error("Could not create any socket!\n");
		return TUMP_SOCKET_ERR;
	}

	freeaddrinfo(result);

	/* Prepare the request to send to the server */
	int msg_len = 0;
	struct tump_req request;
	request.msg_type = TUMP_MSG_REQ;
	request.req_type = req_type;
	char buf[BUFFER_SIZE];
	msg_len += sizeof(struct tump_req);
	memcpy(buf, &request, msg_len);
	if(login != NULL)
	{
		if(strlen(login) > MAX_LOGIN_LENGTH)
		{
			tump_error("Maximum login length is %s", MAX_LOGIN_LENGTH);
			close(connect_socket);
			return TUMP_MAX_LOGIN_EXCEEDED_ERR;
		}
		memcpy(buf + msg_len, login, strlen(login)+1);
		msg_len += strlen(login)+1;
	}
	else
	{
		buf[msg_len++] = 0;
	}

	/* Send the prepared request based on the argument */
	/* If sending the whole request fails more than 3 times, exit with an error */
	int sent;
	int tries = 0;
	while((sent = send(connect_socket, buf, msg_len, 0)) != msg_len)
	{
		tries++;
		if(tries == 3)
		{
			tump_error("Could not send information request!\n");
			close(connect_socket);
			return TUMP_SENDREQ_ERR;
		}
	}

	/* Receive responses until the server closes connection */
	while((msg_len = recv(connect_socket, buf, BUFFER_SIZE - 1, 0)) != 0)
	{
		struct tump_response *response = (struct tump_response *)buf;
		buf[msg_len] = 0;
		/* Check message type, only process responses */
		if(response->msg_type == TUMP_MSG_ERROR)
		{
			struct tump_error *error = (struct tump_error *)response;
			tump_error("Server responded with an error! Error: %s\n", etos(error->err_type));
			close(connect_socket);
			return TUMP_ERROR_RECEIVED_ERR;
		}
		else if(response->msg_type == TUMP_MSG_REQ)
		{
			tump_error("Received a request from the server!\n");
			close(connect_socket);
			return TUMP_ROGUE_SERVER_ERR;
		}
		else if(response->msg_type == TUMP_MSG_RESPONSE)
		{
			if(!response->success)
			{
				tump_error("Specified login was not found in the database!\n");
				close(connect_socket);
				return TUMP_INVALID_LOGIN_ERR;
			}

			fprintf(stdout, "%s", buf + sizeof(struct tump_response));
		}
		else
		{
			/* For some unexplicable reason, some messages arrive withut the protocol header,
			   just with the raw data from the response, so write them out */
			fprintf(stdout, "%s", buf);
		}
	}

	close(connect_socket);

	return EXIT_SUCCESS;
}