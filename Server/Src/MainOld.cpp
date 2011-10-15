/* 
 * File:   main.cpp
 * Author: dynamos
 *
 * Created on October 6, 2011, 12:04 AM
 */
#define __STDC_LIMIT_MACROS
#include <iostream>
#include <algorithm>
using namespace std;
#include <stdint.h>
#include <cstdlib>
#include <time.h>
#include <string.h>
#include <algorithm>
#include <arpa/inet.h>
#include "structs.h"
#include "socket.h"
#define SERVER_VERSION "0.0.02"
    
void tcp_listen (int sockfd);
int tcp_accept (int sockfd, struct sockaddr *client_addr, int *addr_len );
int tcp_sock_connect(char* dest_addr, int port);
int tcp_sock_open(struct in_addr ip, int port);
int blockConnections = 0;
unsigned blockTick = 0;
time_t servertime;
fd_set ReadFDs, WriteFDs, ExceptFDs;

struct timeval select_timeout = { 
	0,
	5000
};

/*
 * 
 */
 //**************************************
 //*********** Main ********************
 //************************************* //why? Coz it makes it easier to find <<
int main(int argc, char** argv) 
{
    unsigned ch,ch2,connectNum;
	struct in_addr ply_in;
	struct sockaddr_in listen_in;
    serverPort = 7110;
	unsigned listen_length;
    int conn_sockfd = -1;
    int pkt_len, pkt_c, bytes_sent;
    unsigned char* connectionChunk;
	unsigned char* connectionPtr;
        ch = 0;
        dp[0] = 0;
        
	printf ("\nUnnamed Project Server Version %s\n", SERVER_VERSION);
	printf ("-=-=-=-=-=-=-=-=-=-=-=-=-=-\n");
	printf ("(c)2011 Dynamaxx Software Ltd\n");
	printf ("\n\n");
        
    printf ("\nAllocating %u bytes of memory for connections... ", sizeof (BANANA) * MAX_CONNECTIONS );
	connectionChunk = (unsigned char*)malloc (sizeof (BANANA) * MAX_CONNECTIONS );
	if (!connectionChunk )
	{
		printf ("Out of memory!\n");
		printf ("Press [ENTER] to quit...");
		exit (1);
	}
	
	connectionPtr = connectionChunk;
	for (ch=0;ch<MAX_CONNECTIONS;ch++)
	{
		connections[ch] = (BANANA*) connectionPtr;
		connections[ch]->clientcard = 0;
		connections[ch]->mode = 0;
		initialize_connection (connections[ch]);
		connectionPtr += sizeof (BANANA);
	}

	printf ("OK!\n\n");
	printf ("Opening port %u for connections.\n", serverPort);

	ply_in.s_addr = INADDR_ANY;
	conn_sockfd = tcp_sock_open( ply_in, serverPort );
	tcp_listen (conn_sockfd);
	
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

	FD_SET (conn_sockfd, &ReadFDs);
	nfds = std::max(nfds,conn_sockfd);
	
	
	// Process player packets

	for (ch=0;ch<serverNumConnections;ch++)
	{
		connectNum = serverConnectionList[ch];
		workConnect = connections[connectNum];

		if (workConnect->plySockfd >= 0) 
		{
			if (blockConnections)
			{
				if (blockTick != (unsigned) servertime)
				{
					blockTick = (unsigned) servertime;
					printf ("Disconnected user %u, left to disconnect: %u\n", workConnect->clientcard, serverNumConnections - 1);
					workConnect->todc = 1;
				}
			}

			if (workConnect->lastTick != (unsigned) servertime)
			{
				//SendPing(workConnect);
				if (workConnect->lastTick > (unsigned) servertime)
					ch2 = 1;
				else
					ch2 = 1 + ((unsigned) servertime - workConnect->lastTick);
					workConnect->lastTick = (unsigned) servertime;
					workConnect->packetsSec /= ch2;
					workConnect->toBytesSec /= ch2;
					workConnect->fromBytesSec /= ch2;
			}

			FD_SET (workConnect->plySockfd, &ReadFDs);
			nfds = std::max(nfds, workConnect->plySockfd);
			FD_SET (workConnect->plySockfd, &ExceptFDs);
			nfds = std::max(nfds, workConnect->plySockfd);

			if (workConnect->snddata - workConnect->sndwritten)
			{
				FD_SET (workConnect->plySockfd, &WriteFDs);
				nfds = std::max(nfds, workConnect->plySockfd);
			}
		}
	}
	
	/* Check sockets for activity. */

	if ( select ( nfds + 1, &ReadFDs, &WriteFDs, &ExceptFDs, &select_timeout ) > 0 ) 
	{
		if (FD_ISSET (conn_sockfd, &ReadFDs))
		{
			// Someone's attempting to connect to the server.
			ch = free_connection();
			if (ch != 0xFFFF)
			{
				listen_length = sizeof (listen_in);
				workConnect = connections[ch];
				if ( ( workConnect->plySockfd = tcp_accept (conn_sockfd, (struct sockaddr*) &listen_in, &listen_length ) ) > 0 )
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
	// Process client connections

		for (ch=0;ch<serverNumConnections;ch++)
		{
			connectNum = serverConnectionList[ch];
			workConnect = connections[connectNum];

			if (workConnect->plySockfd >= 0)
			{
				if (FD_ISSET(workConnect->plySockfd, &WriteFDs)) //trying to send data to them?
				{
					
					// if so let's do it ;o

					bytes_sent = SendDataTo(workConnect, &workConnect->sndbuf[workConnect->sndwritten], workConnect->snddata - workConnect->sndwritten, 0);

					//someone set up me the bomb esse
					if (bytes_sent == -1)
					{

						printf ("Could not send data to client...\n");
						initialize_connection (workConnect);							
					}
					else
					{
						workConnect->toBytesSec += bytes_sent;
						workConnect->sndwritten += bytes_sent;
					}

					if (workConnect->sndwritten == workConnect->snddata)
						workConnect->sndwritten = workConnect->snddata = 0;
				}

				//byebye

				if (workConnect->todc)
					initialize_connection (workConnect);

				if (FD_ISSET(workConnect->plySockfd, &ReadFDs))
				{
					//OH SO YOU HAVE DATA FOR ME GAY BOI?
					if ( ( pkt_len = recv (workConnect->plySockfd, &tmprcv[0], TCP_BUFFER_SIZE - 1, 0) ) <= 0 )
					{

						initialize_connection (workConnect); //no?
					}
					else
					{
						workConnect->fromBytesSec += (unsigned) pkt_len;
						// Work with it...yeah nice and slow 
						memcpy(workConnect->rcvbuf, tmprcv, pkt_len);
						workConnect->packetsSec += pkt_len;
									ProcessPacket (workConnect); //ooo whats in here?
					}
				}
			}

				if (FD_ISSET(workConnect->plySockfd, &ExceptFDs)) // Exception?
					initialize_connection (workConnect);

			}
		} // end of inf loop
		return 0;
} // end of int main
                    
