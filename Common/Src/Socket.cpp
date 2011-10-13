#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include "Socket.hpp"

// This opens a new socket on a port
bool cSocket::Open(unsigned short Port)
{
	int turn_on_option_flag = 1;

	struct sockaddr_in sa;

	/* Clear it out */
	memset(&sa, 0, sizeof(sa));

	// Open our socket
	_Socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* Error */
	if( _Socket < 0 ){
		printf("Could not create socket");
		printf("Press [ENTER] to quit...");
		return false;
	} 

	sa.sin_family = AF_INET;
	sa.sin_addr.s_addr = INADDR_ANY;
	sa.sin_port = htons(Port);

	/* Reuse port */

	setsockopt(_Socket, SOL_SOCKET, SO_REUSEADDR, &turn_on_option_flag, sizeof(turn_on_option_flag));

	/* bind() the socket to the interface */
	if (bind(_Socket, (struct sockaddr *)&sa, sizeof(struct sockaddr)) < 0){
		printf("Could not bind to port");
		printf("Press [ENTER] to quit...");
		return false;
	}
	return true;
}
// This will set the socket to listen for connections
bool cSocket::Listen()
{
	if (listen(_Socket, 10) < 0)
	{
		printf("Could not listen for connection");
		printf("Press [ENTER] to quit...");
		return false;
	}
	else
		printf("\nListening....\n");
	return true;
}
