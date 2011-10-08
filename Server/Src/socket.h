// -*- C++ -*-
/* 
 * File:   socket.H
 * Author: dynamos
 *
 * Created on October 6, 2011, 8:05 PM
 */
#ifndef SOCKET_H
#define SOCKET_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include "structs.h"

#define USEADDR_ANY
#define TCP_BUFFER_SIZE 64000
#define PACKET_BUFFER_SIZE ( TCP_BUFFER_SIZE * 16 )
#define MAX_CONNECTIONS 500
BANANA * connections[MAX_CONNECTIONS];
BANANA * workConnect;

unsigned char dp[TCP_BUFFER_SIZE*4];
unsigned short serverPort;
unsigned short serverMaxConnections;
unsigned short serverNumConnections;
unsigned serverConnectionList[MAX_CONNECTIONS];
unsigned char tmprcv[PACKET_BUFFER_SIZE];

int tcp_sock_open(struct in_addr ip, int port)
{
	int fd, turn_on_option_flag = 1, rcSockopt;

	struct sockaddr_in sa;

	/* Clear it out */
	memset((void *)&sa, 0, sizeof(sa));

	fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	/* Error */
	if( fd < 0 ){
		printf("Could not create socket");
		printf("Press [ENTER] to quit...");
		exit(1);
	} 

	sa.sin_family = AF_INET;
	memcpy((void *)&sa.sin_addr, (void *)&ip, sizeof(struct in_addr));
	sa.sin_port = htons((unsigned short) port);

	/* Reuse port */

	rcSockopt = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *) &turn_on_option_flag, sizeof(turn_on_option_flag));

	/* bind() the socket to the interface */
	if (bind(fd, (struct sockaddr *)&sa, sizeof(struct sockaddr)) < 0){
		printf("Could not bind to port");
		printf("Press [ENTER] to quit...");
		exit(1);
	}

	return(fd);
}

void tcp_listen (int sockfd)
{
	if (listen(sockfd, 10) < 0)
	{
		printf("Could not listen for connection");
		printf("Press [ENTER] to quit...");
		exit(1);
	}
	else
	printf("\nListening....\n");
}

int tcp_accept (int sockfd, struct sockaddr *cliaddr, socklen_t *addrlen )
{
	int fd;

	if ((fd = accept (sockfd, cliaddr, addrlen)) < 0)
		printf("Could not accept connection");

	return (fd);
}

unsigned free_connection()
{
	unsigned fc;
	BANANA* wc;

	for (fc=0;fc<serverMaxConnections;fc++)
	{
		wc = connections[fc];
		if (wc->plySockfd<0)
			return fc;
	}
	return 0xFFFF;
}

void initialize_connection (BANANA* connect)
{
	unsigned ch, ch2;


        
	if (connect->plySockfd >= 0)
	{
		ch2 = 0;
		for (ch=0;ch<serverNumConnections;ch++)
		{
			if (serverConnectionList[ch] != connect->connection_index)
				serverConnectionList[ch2++] = serverConnectionList[ch];
		}
		serverNumConnections = ch2;
		close(connect->plySockfd);
	}

	memset (connect, 0, sizeof (BANANA) );
	connect->plySockfd = -1;
	connect->connected = 1;
	connect->lastTick = 0xFFFFFFFF;
	connect->slotnum = -1;
}

int SendDataTo(BANANA* client, unsigned char * buffer, int buflen, int flags)
{
	int snd;
	
	snd = send(client->plySockfd, buffer, buflen, flags);
	return(snd);
}

void SendDataToAll(unsigned char * buffer, int buflen, int flags)
{
	int ch, connectNum;
	
	for (ch=0;ch<serverNumConnections;ch++)
	{
		connectNum = serverConnectionList[ch];
		if (connections[connectNum]->connected)
		{
			SendDataTo(connections[connectNum],buffer,buflen,flags);
		}	
	}
	
}

void SendPing(BANANA* client)
{
	//ping stuff here to prevent stale connections
}

void ProcessPacket(BANANA* client)
{

		printf("GOT ITEM!");
}

#endif


