#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <math.h>

#include "Socket.hpp"
#include "Players.hpp"
#include "Window.hpp"
#include "Map.hpp"
#include "Graphics.hpp"
#include "Crypto.hpp"

bool Running = true;
cPlayer *Player;
cSocket *Socket;
u32 CurrentPlayerID;
u16 SelectedTile = TILE_TYPE::GRASS;
std::vector<std::pair<u32, std::string>> TextList;

// This drawing code REALLY REALLY shouldn't be here!
void cMap::Draw(sCoord Player)
{
	TileMap::iterator it;
	for(it = _Tiles.begin(); it != _Tiles.end(); ++it)
	{
		Graphics::DrawTile(&it->second, Player);
	}
}

void cPlayer::Player_Thread()
{
	u8 *buf = new u8[MAXPACKETSIZE];
	u8 *pbuf = buf;
	s32 CurrentLoc;
	while(Running)
	{
		if(_Socket->HasData())
		{
			pbuf = buf;
			CurrentLoc = 0;
			
			s32 result = _Socket->Recv( buf, MAXPACKETSIZE);
			if(result == 0) // Player closed connection
				goto end; // For now this will auto destroy the class
			if(result < 0) // Recv error!
				goto end; // Just return for now
			do
			{
				switch(RawReader::GetCommand(pbuf))
				{
					case CommandType::LOGIN: // Return Login Packet packet
					{
						CurrentPlayerID = RawReader::GetID(pbuf);
						if(CurrentPlayerID == 0) // zero means error
						{
							printf("Server couldn't find this user. Exiting for now\n");
							Running = false;
							goto end;
						}
						Players::InsertPlayer(CurrentPlayerID, this); // Since we aren't inserted in to the array at all until we log in.
						u8 *Username = new u8[64];
						u8 *pData = RawReader::GetData(pbuf);
						RawReader::ReadString(&pData, Username);
						SetName(Username);
						f32 X, Y, Z;
						X = RawReader::Read<f32>(&pData);
						Y = RawReader::Read<f32>(&pData);
						Z = RawReader::Read<f32>(&pData);
						SetCoord(X, Y, Z);
						delete[] Username;
					}
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
								f32 X, Y, Z;
								RawReader::ReadString(&SubData, Username);
								X = RawReader::Read<f32>(&SubData);
								Y = RawReader::Read<f32>(&SubData);
								Z = RawReader::Read<f32>(&SubData);
								char Signin[64];
								sprintf(Signin, "Player %s is online!", (char*)Username);
								TextList.push_back(make_pair(2000, std::string(Signin)));
								printf("Player %s is online!\n", Username);
								cPlayer *tmp = new cPlayer(PlayerID);
								tmp->SetName(Username);
								tmp->SetCoord(X, Y, Z);
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
					
						// TODO: We should just put all the information in one packet
						switch(SubCommand)
						{
							case SubCommandType::PLAYERDATA_NAME:
							{
								u8* Username = new u8[64];
								RawReader::ReadString(&SubData, Username);
								char Signin[64];
								if(PlayerID != _ID)
								{
									sprintf(Signin, "Player %s logged in!", (char*)Username);
									TextList.push_back(make_pair(2000, std::string(Signin)));
									printf("Player %s Logged in!\n", Username);
									cPlayer *tmp = new cPlayer(PlayerID);
									tmp->SetName(Username);
									Players::InsertPlayer(PlayerID, tmp);
								}
								else
								{
									// Just setting our username here
									SetName(Username);
								}
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
						char Signin[64];
						sprintf(Signin, "Player %s logged out!", (char*)PlayerArray[PlayerID]->GetName());
						TextList.push_back(make_pair(2000, std::string(Signin)));
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
						PlayerArray[PlayerID]->SetMove(Angle);
					}
					break;
					case CommandType::STOPMOVEMENT:
					{
						u32 PlayerID = RawReader::GetID(pbuf);
						std::map<u32, cPlayer*> PlayerArray = Players::GetArray();
						f32 X, Y, Z;
						u8 *SubData = RawReader::GetData(pbuf);
						X = RawReader::Read<f32>(&SubData);
						Y = RawReader::Read<f32>(&SubData);
						Z = RawReader::Read<f32>(&SubData);
						PlayerArray[PlayerID]->StopMove();
						PlayerArray[PlayerID]->SetCoord(X, Y, Z);
					}
					break;
					case CommandType::MAP:
					{
						u8 *SubData = RawReader::GetData(pbuf);
						u32 SubDataSize = RawReader::GetDataSize(pbuf);
						u32 MapID = RawReader::GetID(pbuf);
						Maps::LoadBuffer(MapID, SubData, SubDataSize);
					}
					break;
					case CommandType::INSERT_TILE: // TODO: Should we allow these to be mashed together?
					{
						u8 *SubData = RawReader::GetData(pbuf);
						Maps::Map(0)->InsertObject(SubData);
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
	delete[] buf;
	Players::RemovePlayer(CurrentPlayerID);
	Running = false;
}

void HandleInput()
{
	std::vector<u32> _Status;
	Windows::KeyLock.lock();
	_Status = Windows::GetKeyStatus();
	Windows::KeyLock.unlock();
	static f32 tZ = 0.0f; // Remember, zero is ground level
	// Loop through until empty
	while(!_Status.empty())
	{
		switch(_Status[0])
		{
			case Key_Type::KEY_W:
				tZ++;
				printf("Z moved to %f\n", tZ);
			break;
			case Key_Type::KEY_S:
				tZ--;
				printf("Z moved to %f\n", tZ);
			break;
			case Key_Type::KEY_A:
				if(SelectedTile > TILE_TYPE::GRASS)
					SelectedTile--;
			break;
			case Key_Type::KEY_D:
				if((SelectedTile + 1) < TILE_TYPE::TILE_END)
					SelectedTile++;
			break;
			case Key_Type::MOUSE_1: // Left click move player
			case Key_Type::MOUSE_2: // Middle click undecided, just move player for now.
			{
				// After initial one on mouse, we have two more in the array for X and Y coordinates of mouse press.
				// TODO: Convert to world coordinates!
				// Right now it is really wierd movement when entirely zoomed in on player!
				s32 X, Y;
				bool Pressed;
				double Angle;
				X = (s32)_Status[1];
				Y = (s32)_Status[2];
				Pressed = (bool)_Status[3];
				Angle = atan2(X - DEFAULT_WIDTH / 2, Y - DEFAULT_HEIGHT / 2);
				_Status.erase(_Status.begin(), _Status.begin() + 3);

				// Setup some of our packet stuff here.
					u8 Packet[256];
					memset(Packet, 0x0, 256); // Just in case
					u8 SubData[64];
					u8 *pSubData = &SubData[0];
					int SubDataSize = 0;
				if(Pressed)
				{
					// We pressed mouse key
					SubDataSize = RawReader::Write<double>(&pSubData, Angle);
					int Size = RawReader::CreatePacket(Packet, CommandType::MOVEMENT, SubCommandType::NONE, CurrentPlayerID, SubData, SubDataSize);
					Player->Send(Packet, Size);
				}
				else
				{
					// We let go of mouse
					SubDataSize = RawReader::Write<f32>(&pSubData, Player->Coord().X);
					SubDataSize += RawReader::Write<f32>(&pSubData, Player->Coord().Y);
					SubDataSize += RawReader::Write<f32>(&pSubData, Player->Coord().Z);
					int Size = RawReader::CreatePacket(Packet, CommandType::STOPMOVEMENT, SubCommandType::NONE, CurrentPlayerID, SubData, SubDataSize);
					Player->Send(Packet, Size);
				}
			}
			break;
			case Key_Type::MOUSE_3: // Right click, add tile
			{
				bool Pressed = (bool)_Status[3];
				_Status.erase(_Status.begin(), _Status.begin() + 3);
				if(Pressed)
				{
					f32 tX = (f32)(s32)Player->Coord().X;
					f32 tY = (f32)(s32)Player->Coord().Y;
					
					u8 Buffer[32];
					u8 *pBuf = &Buffer[0];
					int SubSize = RawReader::Write<u8>(&pBuf, MAP_TILE); 
					SubSize += RawReader::Write<f32>(&pBuf, tX);
					SubSize += RawReader::Write<f32>(&pBuf, tY);
					SubSize += RawReader::Write<f32>(&pBuf, tZ);
					SubSize += RawReader::Write<TILE_TYPE>(&pBuf, (TILE_TYPE)SelectedTile);
					// Now let's send this to the server!
					u8 Packet[128];
					int Size = RawReader::CreatePacket(Packet, CommandType::INSERT_TILE, SubCommandType::NONE, CurrentPlayerID, Buffer, SubSize);
					Player->Send(Packet, Size);
				}
			}
			break;
			case Key_Type::MOUSE_SCROLL_UP:
			case Key_Type::MOUSE_SCROLL_DOWN:
			{
				bool Down = _Status[0] == MOUSE_SCROLL_DOWN ? true : false;
				bool Pressed = (bool)_Status[3];
				_Status.erase(_Status.begin(), _Status.begin() + 3);
				if(!Pressed)
					Graphics::MoveCamera(Down);
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
	struct timeval start, end;
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
	if(!Crypto::Encrypt(CryptoType::SHA256, argv[Passloc], strlen(argv[Passloc]), RawHashPass)) // returns NULL on error
		return 0;
	for(int a = 0; a < 32; ++a)
		sprintf((char*)HashPass, "%s%02X", (a == 0 ? "" : (char*)HashPass), RawHashPass[a]);
	DataSize += RawReader::WriteString(&pData, (char*)HashPass);

	u8 Packet[512];
	memset(Packet, 0x0, 512); // Just be extra safe here
	u32 Size = 0;

	// Are we creating this character?
	if(Create)
	{
		Size = RawReader::CreatePacket(&Packet[0], CommandType::CREATE_ACCOUNT, SubCommandType::NONE, 0, Data, DataSize);
		Socket->Send(Packet, Size);
	}
	
	Size = RawReader::CreatePacket(&Packet[0], CommandType::LOGIN, SubCommandType::NONE, 0, Data, DataSize);
	Socket->Send(Packet, Size);
	
	if(!Windows::Init())
		return 0;
	Graphics::Init();
	
	std::map<u32, cPlayer*> PlayerArray;
	std::map<u32, cPlayer*>::iterator it;
	gettimeofday(&start, NULL);
	while(Running)
	{
		gettimeofday(&end, NULL);
		if(((end.tv_usec + end.tv_sec * 1000000) - (start.tv_usec + start.tv_sec * 1000000)) >= 50000)
		{
			HandleInput();
			gettimeofday(&start, NULL);
		}
		Graphics::Clear();
		PlayerArray = Players::GetArray();
		Maps::Map(0)->Draw(PlayerArray[CurrentPlayerID]->Coord());
		for(it = PlayerArray.begin(); it != PlayerArray.end(); ++it)
		{
			it->second->Move();
			Graphics::DrawPlayer(it->second, PlayerArray[CurrentPlayerID]);
		}
		// Draw a rect around the square we are currently at.
		sCoord Lines[5] = { {0.0f, 0.0f, -32.0f},
							{1.0f, 0.0f, -32.0f},
							{1.0f, 1.0f, -32.0f},
							{0.0f, 1.0f, -32.0f},
							{0.0f, 0.0f, -32.0f} };
		Graphics::DrawLines(Lines, 5); 
		cTile _Tile(PlayerArray[CurrentPlayerID]->Coord().X, PlayerArray[CurrentPlayerID]->Coord().Y, 5, (TILE_TYPE)SelectedTile);
		Graphics::DrawTile(&_Tile, PlayerArray[CurrentPlayerID]->Coord());

		std::vector<std::pair<u32, std::string>>::iterator it;
		f32 _Y = 0;
		for(it = TextList.begin(); it < TextList.end(); ++it)
		{
			if(it->first-- > 0)
			{
				Graphics::DrawText(it->second.c_str(), {0, _Y});
				_Y += 16;
			}
			else
				TextList.erase(it);
		}
		Graphics::Swap();
	}
	Graphics::Shutdown();
	Windows::Shutdown();
	delete Player;
	return 0;
}
