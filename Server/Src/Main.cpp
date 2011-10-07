/* 
 * File:   main.cpp
 * Author: dynamos
 *
 * Created on October 6, 2011, 12:04 AM
 */

#include <cstdlib>
#include <time.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <algorithm>
#include <arpa/inet.h>
#include "structs.h"
#include "socket.h"

#define SERVER_VERSION "0.0.1a"

void tcp_listen (int sockfd);
int tcp_accept (int sockfd, struct sockaddr *client_addr, int *addr_len );
int tcp_sock_connect(char* dest_addr, int port);
int tcp_sock_open(struct in_addr ip, int port);
int blockConnections = 0;
time_t servertime;
fd_set ReadFDs, WriteFDs, ExceptFDs;

struct timeval select_timeout = { 
	0,
	5000
};

/*
 * 
 */
int main(int argc, char** argv) 
{
    unsigned ch,ch2,ch3,ch4,ch5,connectNum;
	struct in_addr my_socket;
	struct sockaddr_in listen_in;
    serverPort = 8866;
	unsigned listen_length;
    int sockfd = -1;
    int pkt_len, pkt_c, bytes_sent;
    unsigned char* connectionChunk;
	unsigned char* connectionPtr;
        ch = 0;
        dp[0] = 0;
        
	printf ("\nUnnamed Project Server Version %s\n", SERVER_VERSION);
	printf ("-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
	printf ("(c)2011 Dynamaxx Software Ltd\n");
	printf ("\n\n");
        
    printf ("\nAllocating %u bytes of memory for connections... ", sizeof (BANANA) * serverMaxConnections );
	connectionChunk = (unsigned char*)malloc ( sizeof (BANANA) * MAX_CONNECTIONS );
	if (!connectionChunk )
	{
		printf ("Out of memory!\n");
		printf ("Press [ENTER] to quit...");
		exit (1);
	}
	connectionPtr = connectionChunk;
	for (ch=0;ch<serverMaxConnections;ch++)
	{
		connections[ch] = (BANANA*) connectionPtr;
		connections[ch]->clientcard = 0;
		connections[ch]->mode = 0;
		initialize_connection (connections[ch]);
		connectionPtr += sizeof (BANANA);
	}

	printf ("OK!\n\n");
        printf ("Opening port %u for connections.\n", serverPort);

		my_socket.s_addr = INADDR_ANY;
        sockfd = tcp_sock_open( my_socket, serverPort );
        tcp_listen (sockfd);
        printf ("\nListening...\n");
        
        for (;;)
        {
			int nfds = 0;
			
			//ping pong
            servertime = time(NULL);
                    
        /* Clear socket activity flags. */

		FD_ZERO (&ReadFDs);
		FD_ZERO (&WriteFDs);
		FD_ZERO (&ExceptFDs);
		
		// Listen for connections

		FD_SET (sockfd, &ReadFDs);
		//nfds = max (nfds, sockfd);
		
		
		/* Check sockets for activity. */

		if ( select ( nfds + 1, &ReadFDs, &WriteFDs, &ExceptFDs, &select_timeout ) > 0 ) 
		{
			if (FD_ISSET (sockfd, &ReadFDs))
			{
				// Someone's attempting to connect to the server.
				ch = free_connection();
				if (ch != 0xFFFF)
				{
					listen_length = sizeof (listen_in);
					workConnect = connections[ch];
					if ( ( workConnect->plySockfd = tcp_accept ( sockfd, (struct sockaddr*) &listen_in, &listen_length ) ) > 0 )
					{
						if ( !blockConnections )
						{
							workConnect->connection_index = ch;
							serverConnectionList[serverNumConnections++] = ch;
							memcpy ( &workConnect->IP_Address[0], inet_ntoa (listen_in.sin_addr), 16 );
							*(unsigned *) &workConnect->ipaddr = *(unsigned *) &listen_in.sin_addr;
							printf ("Accepted connection from %s:%u\n", workConnect->IP_Address, listen_in.sin_port );
							printf ("Player Count: %u\n", serverNumConnections);
						}
						else
							initialize_connection ( workConnect );
					}
				}
			}
                
        }
	}
                    
        }
