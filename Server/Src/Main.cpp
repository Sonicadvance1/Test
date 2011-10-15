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
#include <thread>
#include "Players.hpp"
#include "structs.h"
#include "Socket.hpp"
#define SERVER_VERSION "0.0.02"
unsigned short serverPort;
int blockConnections = 0;
unsigned blockTick = 0;
time_t servertime;
fd_set ReadFDs, WriteFDs, ExceptFDs;

struct timeval select_timeout = { 
	0,
	5000
};

int main(int argc, char** argv) 
{
	// Set our port
	serverPort = 7110;
	cSocket Listener;
	// Bind our port
	if(!Listener.Open(serverPort))
		return 1;
	// Set us to listen
	if(!Listener.Listen())
		return 1;
	for(;;)
	{
		// Do we have a connection?
		if(Listener.HasData())
		{
			// Yes we do
			cSocket *newfd = Listener.Accept();
			// TODO: Currently setting ID to IP
			Players::InsertPlayer(newfd->IP(), new cPlayer(newfd));
		}
	}
	return 0;
} // end of int main
                    
