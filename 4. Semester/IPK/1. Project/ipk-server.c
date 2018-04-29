/**
 * IPK Project 1: Client-Server application for retrieving user information
 * Author: Tomas Danis
 * Login: xdanis05
 * File: ipk-server.c
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
#include <sys/wait.h>
#include <signal.h>

#define BUFFER_SIZE 1501
#define DATA_CHUNK_SIZE 512
#define CONNECTION_QUEUE_SIZE 10
#define INITIAL_MALLOC_SIZE 1024
#define USAGE "Usage: %s -p <port>\n"

void sigchld_handler(int s)
{
	(void)s;
	while(waitpid(-1, NULL, WNOHANG) > 0);
}


/**
 * Reads the specified column of the specified login from the given file. The function assumes
 * /etc/passwd file format, e.g columns are separated with ':'.
 * If the login was not found in the file, returns false and *result is NULL.
 * Otherwise, returns true and *result contains the read data.
 */
bool readColumn(FILE *file, int column, const char *login, char **result)
{
	if(column == 0)
	{
		*result = NULL;
		return true;
	}
	int currentColumn = 1;
	int bufSize = INITIAL_MALLOC_SIZE;
	*result = malloc(bufSize);
	if(*result == NULL) return false;
	int c = 0;
	int bufIndex = 0; //Index into the result buffer
	bool matched = false; //Was the login matched in this row?
	bool cmp_status = true; //Has the comparation of login and read login failed in this row?
	int loginIndex = 0; //Index into the login when comparing it char by char 
	int loginLen = strlen(login);
	while((c = fgetc(file)) != EOF)
	{
		if(column == currentColumn && matched)
		{
			/* Read the specified column from the file when we found a match */
			if(c == ':' || c == '\n') break;
			if(bufIndex == bufSize-1)
			{
				bufSize *= 2;
				char *tmp = realloc(*result, bufSize);
				if(tmp == NULL) 
				{
					free(*result);
					*result = NULL;
					return false;
				}
				*result = tmp;
			}
			(*result)[bufIndex++] = c;
		}
		else if(c == ':')
		{
			/* New column. If login comparation was successful to this point and the logins are the same size, we found a match */
			if(cmp_status && loginIndex == loginLen) 
				matched = true;
			currentColumn++;
			continue;
		}
		else if(currentColumn == 1 && cmp_status)
		{
			/* We are reading the login and the comparation of previous chars was successful */
			if(loginIndex == loginLen || login[loginIndex++] != c)
			{
				cmp_status = false;
			}
		}
		else if(c == '\n')
		{
			/* New line, reset variables */
			cmp_status = true;
			loginIndex = 0;
			currentColumn = 1;
		}
	}

	if(!matched)
	{
		free(*result);
		*result = NULL;
		return true;
	}

	(*result)[bufIndex] = 0;
	return true;
}

/**
 * Reads the first columns from all entries of the given file which start with the given prefix.
 * Expects /etc/passwd file format e.g columns are separated using ':'
 */
bool readList(FILE *file, const char *login, char **result)
{
	int bufSize = INITIAL_MALLOC_SIZE;
	*result = malloc(bufSize);
	if(*result == NULL) return false;
	int bufIndex = 0; //Index into the result buffer
	int loginLen = strlen(login);
	int loginIndex = 0; //Index into the login when comparing it
	int c = 0;
	bool firstColumn = true; //Are we reading the first column
	int lastBufIndex = 0; //Result buffer index from the last row, used to restore bufIndex when current's row login is not matched
	bool match = true; //Is the current's row login matched? Implicit yes, only false when found the prefixes differ
	while((c = fgetc(file)) != EOF)
	{
		if(c == ':')
		{
			firstColumn = false;
		}
		else if(c == '\n')
		{
			firstColumn = true;
			loginIndex = 0;
			if(match)
			{
				if(bufIndex == bufSize-1)
				{
					bufSize *= 2;
					char *tmp = realloc(*result, bufSize);
					if(tmp == NULL) 
					{
						free(*result);
						*result = NULL;
						return false;
					}
					*result = tmp;
				}

				(*result)[bufIndex++] = c;
			}
			lastBufIndex = bufIndex;
			match = true;
		}
		else if(firstColumn && match)
		{
			if(loginLen > loginIndex && login[loginIndex++] != c)
			{
				match = false;
				bufIndex = lastBufIndex;
			}
			else
			{
				if(bufIndex == bufSize-1)
				{
					bufSize *= 2;
					char *tmp = realloc(*result, bufSize);
					if(tmp == NULL) 
					{
						free(*result);
						*result = NULL;
						return false;
					}
					*result = tmp;
				}
				(*result)[bufIndex++] = c;
			}
		}
	}
	(*result)[bufIndex] = 0;
	return true;
}

int main(int argc, char *argv[])
{
	opterr = 0; //getopt silent
	char *port = NULL;

	/* Argument processing and setting flags */
	int opt;
	while((opt = getopt(argc, argv, "p:")) != -1)
	{
		switch(opt)
		{
			case 'p':
				port = optarg;
				break;
			case '?':
				if(optopt == 'p')
				{
					fprintf(stderr, "Port to listen on was not specified!\n");
					fprintf(stderr, USAGE, argv[0]);
					return ARG_ERR;
				}
				else
				{
					fprintf(stderr, "Unknown option!\n");
					fprintf(stderr, USAGE, argv[0]);
					return ARG_ERR;
				}
				break;
			default:
				fprintf(stderr, USAGE, argv[0]);
				return ARG_ERR;
				break;
		}
	}

	/* Unknown unprocessed arguments */
	if(argc != optind)
	{
		fprintf(stderr, "Unrecognized arguments!\n");
		fprintf(stderr, USAGE, argv[0]);
		return ARG_ERR;
	}

	if(port == NULL)
	{
		fprintf(stderr, "Port to listen on was not specified!\n");
		fprintf(stderr, USAGE, argv[0]);
		return ARG_ERR;
	}

	int success;
	struct addrinfo hints, *result, *it;
	int listen_socket = -1;
	int yes = 1;
	struct sockaddr_storage client_addr;
	struct sigaction sa;

	/* Get address information about the server */

	memset(&hints, 0, sizeof hints);
	hints.ai_flags = AI_PASSIVE;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0; //Any
	hints.ai_family = AF_INET;

	if((success = getaddrinfo(NULL, port, &hints, &result)) != 0)
	{
		tump_error("Error: getaddrinfo: %s\n", gai_strerror(success));
		return TUMP_ADDR_ERR;
	}

	/* Bind to the first available address */
	for(it = result; it != NULL; it = it->ai_next)
	{
		listen_socket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if(listen_socket < 0)
		{
			tump_error("Socket could not have been created! Trying another address..\n");
			continue;
		}

		if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) < 0) 
		{
			tump_error("Error: setsockopt\n");
			return TUMP_SOCKOPT_ERR;
        }

        if(bind(listen_socket, result->ai_addr, result->ai_addrlen) < 0)
        {
        	tump_error("Failed to bind socket to port %s",  port);
        	close(listen_socket);
        	return TUMP_BIND_ERR;
        }

        break;
	}

	if(listen_socket < 0)
	{
		tump_error("Could not create any socket!\n");
		return TUMP_SOCKET_ERR;
	}

	freeaddrinfo(result);

	/* Mark the socket as listening */
	if(listen(listen_socket, CONNECTION_QUEUE_SIZE) < 0)
	{
		tump_error("Attempting to listen failed!\n");
		return TUMP_LISTEN_ERR;
	}

	/* Setup child process reaping */
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) 
	{
		fprintf(stderr, "Failed to setup child process reaping!\n");
		return TUMP_INT_ERR;
	}

	/* Main connection accepting loop */
	while(1)
	{
		int client_socket;
		unsigned int client_addrlen = sizeof(client_addr);
		if((client_socket = accept(listen_socket, (struct sockaddr *)&client_addr, &client_addrlen)) < 0)
		{
			tump_error("Failed to accept connection from a client!\n");
			continue;
		}

		if(fork() == 0)
		{
			//Child process
			close(listen_socket); //Unnecessary in the child

			char buf[BUFFER_SIZE];
			struct tump_req *msg = (struct tump_req *)buf;
			int msg_len = recv(client_socket, buf, BUFFER_SIZE-1, 0);

			/* Check correct message type, only process requests */
			if(msg->msg_type != TUMP_MSG_REQ)
			{
				struct tump_error *err_msg = (struct tump_error*)buf;
				err_msg->msg_type = TUMP_MSG_ERROR;
				err_msg->err_type = TUMP_ERR_UNKNOWN_MESSAGE;
				send(client_socket, buf, sizeof(struct tump_error), 0);
			}
			else
			{
				/* Is the request valid? */
				if(buf[msg_len-1] != 0)
				{
					struct tump_error *err_msg = (struct tump_error*)buf;
					err_msg->msg_type = TUMP_MSG_ERROR;
					err_msg->err_type = TUMP_ERR_INVALID_FORMAT;
					send(client_socket, buf, sizeof(struct tump_error), 0);
				}
				else
				{
					char *login = buf + sizeof(struct tump_req);
					FILE *passwd = fopen("/etc/passwd", "r");
					if(passwd == NULL)
					{
						tump_error("Failed to open /etc/passwd!\n");
						close(client_socket);
						return TUMP_INT_ERR;
					}

					bool success = false;
					char *payload = NULL;

					/* Determine what information was requested */
					if(msg->req_type == TUMP_REQ_NAME)
					{
						success = readColumn(passwd, 5, login, &payload);
					}
					else if(msg->req_type == TUMP_REQ_HOME)
					{
						success = readColumn(passwd, 6, login, &payload);
					}
					else if(msg->req_type == TUMP_REQ_LIST)
					{	
						success = readList(passwd, login, &payload);
					}
					else
					{
						struct tump_error *err_msg = (struct tump_error*)buf;
						err_msg->msg_type = TUMP_MSG_ERROR;
						err_msg->err_type = TUMP_ERR_UNKNOWN_REQUEST;
						send(client_socket, buf, sizeof(struct tump_error), 0);
						fclose(passwd);
						close(client_socket);
						return EXIT_SUCCESS;
					}

					/* Memory allocation has failed*/
					if(!success)
					{
						tump_error("Reading /etc/passwd failed!\n");
						fclose(passwd);
						close(client_socket);
						return TUMP_INT_ERR;
					}

					int payloadLen = 0;
					/* If payload is NULL, the login was not found, but we need to initialise it for send */
					if(payload == NULL)
					{
						payloadLen = 0;
						payload = "";
						success = false;
					}
					else
						payloadLen = strlen(payload);

					/* Send the data in 512B chunks with appropriate headers */
					int totalSent = 0;
					do
					{
						char buf[BUFFER_SIZE];
						int toSend = (payloadLen < DATA_CHUNK_SIZE) ? payloadLen : DATA_CHUNK_SIZE;
						struct tump_response *response = (struct tump_response *)buf;
						response->msg_type = TUMP_MSG_RESPONSE;
						response->success = (int)success;
						memcpy(buf + sizeof(struct tump_response), payload+totalSent, toSend);
						int sent = send(client_socket, buf, toSend + sizeof(struct tump_response), 0);
						sent -= sizeof(struct tump_response);
						payloadLen -= sent;
						totalSent += sent;
					} 
					while(payloadLen > 0);


					if(success)
						free(payload);
					fclose(passwd);
				}
			}

			close(client_socket);
			return EXIT_SUCCESS;

		}

		/* Parent code */
		close(client_socket);
	}

	return EXIT_SUCCESS;
}