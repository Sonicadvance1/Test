#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "Socket.hpp"

struct timeval Maxtimeout = { 
	1,
	0
};

// Our Constructor
cSocket::cSocket()
{
	// Zero out this socket's select
	FD_ZERO(&_Set);
	
}
// Constructor with a valid socket
cSocket::cSocket(s32 Socket, u32 IP)
{
	//	Zero out our set
	FD_ZERO(&_Set);
	// Add This socket to the set right away
	FD_SET(Socket, &_Set);
	// Set our internal socket
	_Socket = Socket;
	// Set our IP
	_IP = IP;
}

// Destructor
cSocket::~cSocket()
{
	close(_Socket);
}

// This opens a new socket on a port
bool cSocket::Open(u16 Port)
{
	int turn_on_option_flag = 1;

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

	return true;
}
bool cSocket::Bind()
{
	/* bind() the socket to the interface */
	if (bind(_Socket, (struct sockaddr *)&sa, sizeof(sa)) < 0){
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
	// Add the Listening socket to the set
	FD_SET(_Socket, &_Set);
	return true;
}

// Do we have data?
bool cSocket::HasData()
{
	fd_set tmpSet = _Set;
	timeval tmpVal = Maxtimeout;
	// Select has the ability to modify both the set and the timeval....WHICH IT DOES! D:<
    if(select(_Socket+1, &tmpSet, NULL, NULL, &tmpVal) == -1)
    {
        printf("Couldn't select socket!\n");
        return false;
    }
    // Do we have anyone trying to connect?
    if(FD_ISSET(_Socket, &tmpSet))
    {
    	return true;
    }
    return false;
}

// Let's get our data!
s32 cSocket::Recv(u8 *buf, u32 size)
{
	// Just return what recv does
    return recv(_Socket, buf, size, 0);
}

// Accepts new connections
cSocket* cSocket::Accept()
{

	s32 newfd; // New Socket
	struct sockaddr_in clientaddr;
    socklen_t addrlen = sizeof(clientaddr);
    if((newfd = accept(_Socket, (struct sockaddr *)&clientaddr, &addrlen)) == -1)
    {
        printf("Couldn't accept connection!\n");
        return 0;
    }
    else
    {
        printf("Server-accept() is OK...\n");
        printf("New connection from %s on socket %d\n", inet_ntoa(clientaddr.sin_addr), newfd);
        return new cSocket(newfd, clientaddr.sin_addr.s_addr);
    }
}
u32 cSocket::IP()
{
	return _IP;
}
bool cSocket::Connect(const char* IP)
{
	struct hostent *server = gethostbyname(IP);
	bcopy((char *)server->h_addr, (char *)&sa.sin_addr.s_addr, server->h_length);
	if (connect(_Socket,(struct sockaddr *) &sa,sizeof(sa)) < 0)
	{
        printf("error connecting to %s\n", IP);
		return false;
	}
    return true;
}
