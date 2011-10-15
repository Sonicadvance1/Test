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
#include <thread>
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
// This thread function needs to be somewhere else, probably within the player class
// Haven't thought that far yet.
void Player_Thread(u32 Socket)
{
	// TODO: Global Player Array, and we just pass through either a pointer to the player class that it is
	// Or just a number in the array?
	cSocket Player(Socket);
	u8 buf[512];
	for(;;)
	{
		if(Player.HasData())
		{
			s32 result = Player.Recv( buf, sizeof(buf));
			if(result == 0) // Player closed connection
				return; // For now this will auto destroy the class
			if(result < 0) // Recv error!
				return; // Just return for now
			printf("%s\n", buf);
		}
	}
}
std::thread t;

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
			u32 newfd = Listener.Accept();
			if(newfd) // No Error
				t = std::thread(Player_Thread, newfd);
		}
	}
	return 0;
} // end of int main
                    
