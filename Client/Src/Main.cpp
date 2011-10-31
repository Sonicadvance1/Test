#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include "Socket.hpp"
#include "Players.hpp"
#include "Window.hpp"
#include "Map.hpp"
#include "Graphics.hpp"
#include "Crypto.hpp"
#include <GL/gl.h>

bool Running = true;
cPlayer *Player;
cSocket *Socket;
cMap Test;
u32 CurrentPlayerID;

// This drawing code REALLY REALLY shouldn't be here!
GLuint Tex;
void cMap::Draw()
{
	std::map<std::pair<std::pair<f32, f32>, f32>, cTile>::iterator it;
	for(it = _Tiles.begin(); it != _Tiles.end(); ++it)
	{
		Graphics::DrawCube({it->second._X, it->second._Y, it->second._Z, 1, 1, 1}, Tex);
	}
}

void cPlayer::Player_Thread()
{
	u8 buf[512];
	u8 *pbuf = buf;
	s32 CurrentLoc;
	while(Running)
	{
		if(_Socket->HasData())
		{
			pbuf = buf;
			CurrentLoc = 0;
			
			s32 result = _Socket->Recv( buf, sizeof(buf));
			if(result == 0) // Player closed connection
				goto end; // For now this will auto destroy the class
			if(result < 0) // Recv error!
				goto end; // Just return for now
			printf("Sweet, We got something %d big\n", result);
			do
			{
				switch(RawReader::GetCommand(pbuf))
				{
					case CommandType::LOGIN: // Return Login Packet packet
						CurrentPlayerID = RawReader::GetID(pbuf);
						if(CurrentPlayerID == 0) // zero means error
						{
							printf("Server couldn't find this user. Exiting for now\n");
							Running = false;
							goto end;
						}
						Players::InsertPlayer(CurrentPlayerID, this); // Since we aren't inserted in to the array at all until we log in.
					break;
					case CommandType::PLAYERDATA:
					{
						u32 PlayerID = RawReader::GetID(pbuf); // Get the other player's ID
						SubCommandType SubCommand = RawReader::GetSubCommand(pbuf);
						u8 *SubData = RawReader::GetData(pbuf);
						switch(SubCommand)
						{
							case SubCommandType::PLAYERDATA_NAME:
							{
								u8* Username = new u8[64];
								f32 X, Y;
								RawReader::ReadString(&SubData, Username);
								X = RawReader::Read<f32>(&SubData);
								Y = RawReader::Read<f32>(&SubData);
								printf("Player %s is online!\n", Username, X, Y);
								cPlayer *tmp = new cPlayer(PlayerID);
								tmp->SetName(Username);
								tmp->SetCoord(X, Y);
								Players::InsertPlayer(PlayerID, tmp);
								delete[] Username;
							}
							break;
							default:
								printf("We don't understand subcommand %d for command %d\n", SubCommand, RawReader::GetCommand(pbuf));
							break;
						}
						
					}
					break;
					case CommandType::LOGGED_IN: // For when someone logs in
					{
						u32 PlayerID = RawReader::GetID(pbuf); // Get the player's ID that logged in
						SubCommandType SubCommand = RawReader::GetSubCommand(pbuf);
						u8 *SubData = RawReader::GetData(pbuf);
						if(PlayerID == _ID)
							break; // Just ourselves logging in
						// TODO: We should just put all the information in one packet
						switch(SubCommand)
						{
							case SubCommandType::PLAYERDATA_NAME:
							{
								u8* Username = new u8[64];
								RawReader::ReadString(&SubData, Username);
								printf("Player %s Logged in!\n", Username);
								cPlayer *tmp = new cPlayer(PlayerID);
								tmp->SetName(Username);
								Players::InsertPlayer(PlayerID, tmp);
								delete[] Username;
							}
							break;
							default:
								printf("We don't understand subcommand %d for command %d\n", SubCommand, RawReader::GetCommand(pbuf));
							break;
						}
					}
					break;
					case CommandType::LOGGED_OUT: // Someone logged out!
					{
						u32 PlayerID = RawReader::GetID(pbuf);
						std::map<u32, cPlayer*> PlayerArray = Players::GetArray();
						printf("%s Logged out!\n", PlayerArray[PlayerID]->GetName());
						Players::RemovePlayer(PlayerID);
					}
					break;
					case CommandType::MOVEMENT:
					{
						u32 PlayerID = RawReader::GetID(pbuf);
						double Angle;
						u8 *SubData = RawReader::GetData(pbuf);
						Angle = RawReader::Read<double>(&SubData);
						std::map<u32, cPlayer*> PlayerArray = Players::GetArray();
						printf("Player %s moved with angle %lf\n", PlayerArray[PlayerID]->GetName(), Angle);
						PlayerArray[PlayerID]->Move(Angle);
					}
					break;
					case CommandType::MAP:
					{
						u8 *SubData = RawReader::GetData(pbuf);
						u32 SubDataSize = RawReader::GetDataSize(pbuf);
						Test.Load(SubData, SubDataSize);
					}
					break;
					case CommandType::INSERT_TILE: // TODO: Should we allow these to be mashed together?
					{
						u8 *SubData = RawReader::GetData(pbuf);
						Test.InsertObject(SubData);
					}
					break;
					default:
						printf("We Don't know command: %02X\n", RawReader::GetCommand(pbuf));
					break;
				}
			} while(RawReader::NextPacket(&pbuf, &CurrentLoc, result));
		}
	}
	end:
	Players::RemovePlayer(CurrentPlayerID);
	Running = false;
}

void HandleInput()
{
	std::vector<u32> _Status;
	Windows::KeyLock.lock();
	_Status = Windows::GetKeyStatus();
	Windows::KeyLock.unlock();
	
	// Loop through until empty
	while(!_Status.empty())
	{
		switch(_Status[0])
		{
			/*case Key_Type::KEY_W:
				Vy++;
			break;
			case Key_Type::KEY_A:
				Vx--;
			break;
			case Key_Type::KEY_S:
				Vy--;
			break;
			case Key_Type::KEY_D:
				Vx++;
			break;*/
			case Key_Type::MOUSE_1: // Left click move player
			case Key_Type::MOUSE_2: // Middle click undecided, just move player for now.
			{
				// After initial one on mouse, we have two more in the array for X and Y coordinates of mouse press.
				// TODO: Convert to world coordinates!
				s32 X, Y;
				double Angle;
				X = (s32)_Status[1];
				Y = (s32)_Status[2];
				Angle = atan2(Y - DEFAULT_HEIGHT / 2 , X - DEFAULT_WIDTH / 2);
				printf("Clicked %d at %d %d %lf g\n", _Status[0], X, Y, Angle);
				_Status.erase(_Status.begin(), _Status.begin() + 2);

				u8 Packet[256];
				u8 SubData[64];
				u8 *pSubData = &SubData[0];
				int SubDataSize = RawReader::Write<double>(&pSubData, Angle);
				Player->Move(Angle);
				int Size = RawReader::CreatePacket(Packet, CommandType::MOVEMENT, SubCommandType::NONE, CurrentPlayerID, SubData, SubDataSize);
				Player->Send(Packet, Size);
			}
			break;
			case Key_Type::MOUSE_3: // Right click, add tile
			{
				_Status.erase(_Status.begin(), _Status.begin() + 2);
				f32 tX = (f32)(s32)Player->Coord().X;
				f32 tY = (f32)(s32)Player->Coord().Y;
				f32 tZ = 0.0f; // Remember, zero is ground level
				u8 Buffer[32];
				u8 *pBuf = &Buffer[0];
				RawReader::Write<u8>(&pBuf, MAP_TILE); 
				RawReader::Write<f32>(&pBuf, tX);
				RawReader::Write<f32>(&pBuf, tY);
				RawReader::Write<f32>(&pBuf, tZ);
				// Now let's send this to the server!
				u8 Packet[128];
				int Size = RawReader::CreatePacket(Packet, CommandType::INSERT_TILE, SubCommandType::NONE, CurrentPlayerID, Buffer, 13);
				Player->Send(Packet, Size);
			}
			break;
			default:
				printf("Unknown Key Function: %d\n", _Status[0]);
			break;
			
		}
		_Status.erase(_Status.begin());
	}
   
}
int main(int argc, char **argv)
{
	if(argc < 3)
	{
		printf("Usage: %s [-c] <name> <pass>\n", argv[0]);
		return 0;
	}
	int Nameloc = 1;
	int Passloc = 2;
	bool Create = false;
	if(argc == 4 && !strcmp(argv[1], "-c"))
	{
		Nameloc++;
		Passloc++;
		Create = true;
	}
	Socket = new cSocket();
	Socket->Open(7110);
	Socket->Connect("127.0.0.1");

	Player = new cPlayer(Socket);
	
	u8 Data[128];
	u8 RawHashPass[32];
	u8 HashPass[64];
	u8 *pData = &Data[0];
	u32 DataSize = RawReader::WriteString(&pData, argv[Nameloc]);
	// Encrypt our password
	Crypto::Encrypt(CryptoType::SHA256, argv[Passloc], strlen(argv[Passloc]), RawHashPass);
	for(int a = 0; a < 32; ++a)
		sprintf((char*)HashPass, "%s%02X", (a == 0 ? "" : (char*)HashPass), RawHashPass[a]);
	DataSize += RawReader::WriteString(&pData, (char*)HashPass);

	u8 Packet[512];
	u32 Size;

	// Are we creating this character?
	if(Create)
	{
		Size = RawReader::CreatePacket(&Packet[0], CommandType::CREATE_ACCOUNT, SubCommandType::NONE, 0, Data, DataSize);
		Socket->Send(Packet, Size);
	}
	
	Size = RawReader::CreatePacket(&Packet[0], CommandType::LOGIN, SubCommandType::NONE, 0, Data, DataSize);
	Socket->Send(Packet, Size);
	
	Windows::Init();
	Graphics::Init();
	// Loading texture shouldn't really be here
	Tex = Graphics::LoadTexture("Resources/Watermelon.png");
	
	std::map<u32, cPlayer*> PlayerArray;
	std::map<u32, cPlayer*>::iterator it;
	while(Running)
	{
		usleep(200);
		HandleInput();
		Graphics::Clear();
		Test.Draw();
		PlayerArray = Players::GetArray();
		for(it = PlayerArray.begin(); it != PlayerArray.end(); ++it)
			Graphics::DrawPlayer(it->second);
		// Draw a rect around the square we are currently at.
		f32 tX = (f32)(s32)Player->Coord().X;
		f32 tY = (f32)(s32)Player->Coord().Y;
		sCoord Lines[5] = { {tX, tY, -32.0f},
							{tX + 1.0f, tY, -32.0f},
							{tX + 1.0f, tY + 1.0f, -32.0f},
							{tX, tY + 1.0f, -32.0f},
							{tX, tY, -32.0f} };
		Graphics::DrawLines(Lines, 5); 
		Graphics::DrawRect({0, 0, .1, .1}, 0);
		Graphics::Swap();
	}
	Windows::Shutdown();
	delete Player;
	return 0;
}
