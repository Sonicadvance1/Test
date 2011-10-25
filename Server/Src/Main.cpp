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
#include "PacketTypes.hpp"
#include "RawReader.hpp"

#define SERVER_VERSION "0.0.02"
bool Running = true;
cSocket Listener;

unsigned short serverPort = 7110;
int blockConnections = 0;
unsigned blockTick = 0;
time_t servertime;

void cPlayer::Player_Thread()
{
	u8 buf[512];
	for(;;)
	{
		if(_Socket->HasData())
		{
			s32 result = _Socket->Recv( buf, sizeof(buf));
			if(result == 0) // Player closed connection
				return; // For now this will auto destroy the class
			if(result < 0) // Recv error!
				return; // Just return for now
			printf("Sweet, We got something %d big\n", result);
			switch(buf[0])
			{
				case CommandType::LOGIN:
				{
					u8 *Data = RawReader::GetData(buf);
					u8 *Username = new u8[64];
					u8 *Password = new u8[65];
					RawReader::ReadString(&Data, Username);
					RawReader::ReadString(&Data, Password); // Should be a hash
					
					// TODO: Send back player ID
					char Command[512];
					char **Results;
					int Rows, Cols;

					sprintf(Command, "SELECT * FROM "PLAYER_TABLE" WHERE PlayerName='%s' AND Password='%s'", Username, Password);
					Database::Table(Command, &Results, &Rows, &Cols);
					if(Rows > 1) // Found our user
					{
						// Should send all of our shit here
						printf("Found User %s\n", Username);
						u8 Packet[256];
						u32 OldID = _ID;
						_ID = atoi(Results[Cols]); // Gives us the First column in the returned array which is ID!
						// Create the Login packet and send it
						int Size = RawReader::CreatePacket(Packet, CommandType::LOGIN, SubCommandType::NONE, _ID, 0, 0);
						_Socket->Send(Packet, Size);
						// Now we need to move the player to the new map location
						Players::RemovePlayer(OldID);
						Players::InsertPlayer(_ID, this);
					}
					else // No user
					{
						// TODO: Need to eject the player and clean up here
					}

					Database::FreeTable(Results);
					delete Username;
					delete Password;
				}
				break;
				case CommandType::CREATE_ACCOUNT:
				{
					// TODO: Add Player to the Database
				}
				break;
				default:
					printf("We Don't know command: %02X\n", buf[0]);
				break;
			}
		}
	}
}

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
                    
