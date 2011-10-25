#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.hpp"
#include "Players.hpp"
#include "Window.hpp"
#include "Map.hpp"
#include "Graphics.hpp"
#include "Crypto.hpp"
#include <GL/gl.h>

bool Running = true;
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
				default:
					printf("We Don't know command: %02X\n", buf[0]);
				break;
			}
		}
	}
}

cPlayer *Player;
cSocket *Socket;
void HandleInput()
{
	std::vector<u32> _Status;
	Windows::KeyLock.lock();
	_Status = Windows::GetKeyStatus();
	Windows::KeyLock.unlock();
	s8 Vx, Vy;
	
	// Loop through until empty
	while(!_Status.empty())
	{
		switch(_Status[0])
		{
			case Key_Type::KEY_W:
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
			break;
			case Key_Type::MOUSE_1:
			case Key_Type::MOUSE_2:
			case Key_Type::MOUSE_3:
			{
				// After initial one on mouse, we have two more in the array for X and Y coordinates of mouse press.
				u32 X, Y;
				X = _Status[1];
				Y = _Status[2];
				printf("Clicked %d at %d %d\n", _Status[0], X, Y);
				_Status.erase(_Status.begin(), _Status.begin() + 2);
				
			}
			break;
			default:
				printf("Unknown Key Function: %d\n", _Status[0]);
			break;
			
		}
		Player->Move(Vx, Vy);
		_Status.erase(_Status.begin());
	}
	// TODO: This needs to be moved to when things are received
	
   /* if(Direction != -1)
    {
		u8 *pPacket = new u8[512];
		u8 *Start = pPacket;
		u16 size = 3;
		size += RawReader::Write<u16>(&pPacket, 0x0001); // Command
		u8 *pLength = pPacket; // Save this location for later
		size += RawReader::Write<u8>(&pPacket, 0x00); // Sub Function
		size += RawReader::Write<u16>(&pPacket, 0x0000); // Player ID
		size += RawReader::Write<u8>(&pPacket, Direction); // Send player moving direction
		RawReader::Write<u16>(&pPacket, size / 2); //Size divided by two in the end?
		RawReader::Write<u16>(&pLength, size); // Put our length in earlier in the stream
		for(int a = 0; a < size - 1; ++a)
			printf("%02X", Start[a]);
		printf("\n");
		delete[] Start;
	}*/
   
   
}
int main(int argc, char **argv)
{
	if(argc < 3)
	{
		printf("Usage: %s <name> <pass>\n", argv[0]);
		return 0;
	}
	Socket = new cSocket();
	Socket->Open(7110);
	Socket->Connect("127.0.0.1");

	Player = new cPlayer(Socket);
	
	u8 Data[128];
	u8 RawHashPass[32];
	u8 HashPass[64];
	u8 *pData = &Data[0];
	u32 DataSize = RawReader::WriteString(&pData, argv[1]);
	// Encrypt our password
	Crypto::Encrypt(CryptoType::SHA256, argv[2], strlen(argv[2]), RawHashPass);
	for(int a = 0; a < 32; ++a)
		sprintf((char*)HashPass, "%s%02X", (a == 0 ? "" : (char*)HashPass), RawHashPass[a]);
	DataSize += RawReader::WriteString(&pData, (char*)HashPass);
	
	u8 Packet[512];
	u32 Size;
	Size = RawReader::CreatePacket(&Packet[0], CommandType::LOGIN, SubCommandType::NONE, 0, Data, DataSize);
	Socket->Send(Packet, Size);
	
	cMap Test;
	Windows::Init();
	Graphics::Init();
	Test.Load(5);
	while(Running)
	{
		usleep(200);
		HandleInput();
		Graphics::Clear();
		Test.Draw();
		Graphics::DrawPlayer(Player);
		Graphics::Swap();
	}
	Windows::Shutdown();
	delete Player;
	return 0;
}
