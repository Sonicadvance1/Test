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
#include <vector>

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
std::vector<cPlayer*> Deletions;
std::mutex mDeletions;

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
			switch(RawReader::GetCommand(buf))
			{
				case CommandType::LOGIN:
				{
					u8 *Data = RawReader::GetData(buf);
					u8 *Username = new u8[64];
					u8 *Password = new u8[65];
					RawReader::ReadString(&Data, Username);
					RawReader::ReadString(&Data, Password); // Should be a hash
					
					char Command[512];
					char **Results;
					int Rows, Cols;

					sprintf(Command, "SELECT * FROM "PLAYER_TABLE" WHERE PlayerName='%s' AND Password='%s'", Username, Password);
					Database::Table(Command, &Results, &Rows, &Cols);

					u8 Packet[256];
					if(Rows > 1) // Found our user
					{
						// Should send all of our shit here
						printf("Found User %s\n", Username);
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
						printf("Couldn't find User %s\n", Username);
						// Send the user a Login packet with a zero ID signifying we couldn't find that account
						int Size = RawReader::CreatePacket(Packet, CommandType::LOGIN, SubCommandType::NONE, 0, 0, 0);
						_Socket->Send(Packet, Size);
						// Remove the player from the array
						Players::RemovePlayer(_ID);
						// Now clean up this object
						mDeletions.lock();
						// Add this to the vector so we can delete it
						Deletions.push_back(this);
						mDeletions.unlock();
						Database::FreeTable(Results);
						delete Username;
						delete Password;
						return;
					}
					// TODO: Grab player shit from the database

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
					printf("We Don't know command: %02X\n", RawReader::GetCommand(buf));
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
		// Check for disconnected people
		mDeletions.lock();
		if(Deletions.size() > 0) // yay, people to remove
		{
			for(int a = 0; a < Deletions.size(); ++a)
				delete Deletions[a];
			Deletions.clear();
		}
		mDeletions.unlock();
	}
	// Shutdown our Database
	Database::Shutdown();
	return 0;
} // end of int main
                    
