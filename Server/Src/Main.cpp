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
#include "Map.hpp"

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
	u8 buf[MAXPACKETSIZE];
	u8 *pbuf = buf;
	s32 CurrentLoc;
	for(;;)
	{
		if(_Socket->HasData())
		{
			memset(buf, MAXPACKETSIZE, 0);
			pbuf = buf;
			CurrentLoc = 0;
			
			s32 result = _Socket->Recv( buf, MAXPACKETSIZE);
			if(result == 0) // Player closed connection
				goto end; // For now this will auto destroy the class
			if(result < 0) // Recv error!
				goto end; // Just return for now
			printf("Sweet, We got something %d big\n", result);
			do
			{
				switch(RawReader::GetCommand(pbuf))
				{
					case CommandType::LOGIN:
					{
						u8 *Data = RawReader::GetData(pbuf);
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
							u32 OldID = _ID;
							_ID = atoi(Results[Cols]); // Gives us the First column in the returned array which is ID!
							printf("Found User %s: %d\n", Username, _ID);
							// Create the Login packet and send it
							int Size = RawReader::CreatePacket(Packet, CommandType::LOGIN, SubCommandType::NONE, _ID, 0, 0);
							_Socket->Send(Packet, Size);
							// Now we need to move the player to the new map location
							Players::RemovePlayer(OldID);
							Players::InsertPlayer(_ID, this);
							SetName(Username);
							
							// Send player all connected Users
							std::map<u32, cPlayer*> PlayerArray = Players::GetArray();
							std::map<u32, cPlayer*>::iterator it;
							u8 SubData[64];
							u8 *pSubData;
							memset(Packet, 256, 0);
							// Send User all connected players
							for(it = PlayerArray.begin(); it != PlayerArray.end(); ++it)
							{
								memset(&SubData, 64, 0);
								pSubData = &SubData[0];
								if(it->first < MAXUSERS && it->first != _ID) // Make sure it isn't a client that isn't logged in and isn't ourselves
								{
									int SubDataSize = RawReader::WriteString(&pSubData, (const char*)it->second->GetName(), strlen((const char*)it->second->GetName()));
									sCoord tCoord = it->second->Coord();
									SubDataSize += RawReader::Write<f32>(&pSubData, tCoord.X);
									SubDataSize += RawReader::Write<f32>(&pSubData, tCoord.Y);
									SubDataSize += RawReader::Write<f32>(&pSubData, tCoord.Z);
									Size = RawReader::CreatePacket(Packet, CommandType::PLAYERDATA, SubCommandType::PLAYERDATA_NAME, it->first, SubData, SubDataSize);
									_Socket->Send(Packet, Size);
								}
							}
							// Send Users the newly connected player
							memset(Packet, 256, 0);
							memset(&SubData, 64, 0);
							pSubData = &SubData[0];
							int SubDataSize = RawReader::WriteString(&pSubData, (const char*)GetName(), strlen((const char*)GetName()));
							Size = RawReader::CreatePacket(Packet, CommandType::LOGGED_IN, SubCommandType::PLAYERDATA_NAME, _ID, SubData, SubDataSize);
							Players::SendAll(Packet, Size, _ID);

							// Send the User the map
							// TODO: Send in chunks
							u8 *MapPacket;
							u32 MapSize;
							u8* MapArray = Maps::_Maps[0]->GetMap(&MapSize);
							// We need to allocate this size because it could get massive.
							// We align it to a 4-byte boundary because I've gotten problems with it before
							MapPacket = new u8[(4 - (MapSize + 9) % 4) + (MapSize + 9)];
							Size = RawReader::CreatePacket(MapPacket, CommandType::MAP, SubCommandType::NONE, 0/* MAP ID? */, MapArray, MapSize);
							_Socket->Send(MapPacket, Size);
							delete[] MapPacket;
							
							
						}
						else // No user
						{
							printf("Couldn't find User %s\n", Username);
							// Send the user a Login packet with a zero ID signifying we couldn't find that account
							int Size = RawReader::CreatePacket(Packet, CommandType::LOGIN, SubCommandType::NONE, 0, 0, 0);
							_Socket->Send(Packet, Size);
							Database::FreeTable(Results);
							delete Username;
							delete Password;
							goto end;
						}
						// TODO: Grab player shit from the database

						Database::FreeTable(Results);
						delete[] Username;
						delete[] Password;
					}
					break;
					case CommandType::CREATE_ACCOUNT:
					{
						// TODO: Add Player to the Database
						u8 *Data = RawReader::GetData(pbuf);
						u8 *Username = new u8[64];
						u8 *Password = new u8[65];
						RawReader::ReadString(&Data, Username);
						RawReader::ReadString(&Data, Password); // Should be a hash
						
						char Command[512];
						char **Results;
						int Rows, Cols;

						sprintf(Command, "SELECT * FROM "PLAYER_TABLE" WHERE PlayerName='%s'", Username);
						Database::Table(Command, &Results, &Rows, &Cols);
						if(Rows > 1) // Player already exists
						{
							printf("User %s already exists!\n", Username);
							// TODO: Send player a message saying it isn't available
							Database::FreeTable(Results);
							delete[] Username;
							delete[] Password;
							goto end;
						}
						else
						{
							printf("Creating Character %s\n", Username);
							// TODO: Send player a message saying that their account has been created
							sprintf(Command, "INSERT INTO "PLAYER_TABLE" (Playername, Password) VALUES('%s', '%s')", Username, Password);
							// TODO: We need to do some password checking and scrubbing
							Database::Exec(Command); // Just execute the insertion
						}
						Database::FreeTable(Results);
						delete[] Username;
						delete[] Password;
					}
					break;
					case CommandType::MOVEMENT: // A Player moved
					{
						// Send to all except the ID that sent it
						// TODO: Validate movement
						double Angle;
						u8 *SubData = RawReader::GetData(pbuf);
						Angle = RawReader::Read<double>(&SubData);
						Move(Angle);
						Players::SendAll(pbuf, RawReader::GetFullSize(pbuf), _ID);
					}
					break;
					case CommandType::INSERT_TILE:
					{
						// TODO: We will need to change where this is getting changed later on.
						u8 *SubData = RawReader::GetData(pbuf);
						// Insert it in to our map
						// Currently Map[0] just because that is the ONLY map we have.
						Maps::_Maps[0]->InsertObject(SubData);
						// Send the change to everyone
						Players::SendAll(pbuf, RawReader::GetFullSize(pbuf));
					}
					break;
					default:
						printf("We Don't know command: %02X\n", RawReader::GetCommand(pbuf));
					break;
				}
			}while(RawReader::NextPacket(&pbuf, &CurrentLoc, result));
		}
	}
	end:
	printf("Player %s left!\n", GetName());
	// Send everyone that that person left
	u8 Packet[256];
	int Size = RawReader::CreatePacket(Packet, CommandType::LOGGED_OUT, SubCommandType::NONE, _ID, 0, 0);
	Players::SendAll(Packet, Size, _ID);
	// Remove the player from the array
	Players::RemovePlayer(_ID);
	// Now clean up this object
	mDeletions.lock();
	// Add this to the vector so we can delete it
	Deletions.push_back(this);
	mDeletions.unlock();
	
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
	Maps::Load(0); // TODO: Check how many maps we have and load them all at once, for now loading default 0 will work.
		
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
			{
				
				memset(&Deletions[a], sizeof(Deletions[a]), 0);
				delete Deletions[a];
			}
			Deletions.clear();
		}
		mDeletions.unlock();
	}
	// Shutdown our Database
	Database::Shutdown();
	// Shutdown the maps
	Maps::Shutdown();
	return 0;
} // end of int main
                    
