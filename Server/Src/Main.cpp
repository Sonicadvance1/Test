/* 
 * File:   main.cpp
 * Author: dynamos
 *
 * Created on October 6, 2011, 12:04 AM
 */
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <cstdlib>
#include <time.h>
#include <string.h>
#include <signal.h>

#include "Players.hpp"
#include "structs.h"
#include "Socket.hpp"
#include "Database.hpp"

#define SERVER_VERSION "0.0.02"
bool Running = true;
cSocket Listener;

unsigned short serverPort = 7110;
int blockConnections = 0;
unsigned blockTick = 0;
time_t servertime;

void Sighandler(int sig)
{
	Running = false;
}
int main(int argc, char** argv) 
{
	// Attach to various terminal signals
    signal(SIGABRT, &Sighandler);
	signal(SIGTERM, &Sighandler);
	signal(SIGINT, &Sighandler);
	// Initialize our Database
	if(!Database::Init())
		return 1;
		
	// Open the socket
	if(!Listener.Open(serverPort))
		return 2;
	// Bind our socket
	if(!Listener.Bind())
		return 4;
	// Set us to listen
	if(!Listener.Listen())
		return 3;
	while(Running)
	{
		// Do we have a connection?
		if(Listener.HasData())
		{
			printf("Yay, new person\n");
			// Yes we do
			cSocket *newfd = Listener.Accept();
			// TODO: Currently setting ID to IP
			Players::InsertPlayer(newfd->IP(), new cPlayer(newfd));
		}
	}
	// Shutdown our Database
	Database::Shutdown();
	return 0;
} // end of int main
                    
