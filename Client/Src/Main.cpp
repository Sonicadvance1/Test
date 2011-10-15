#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.hpp"
#include "Players.hpp"
#include "Window.hpp"
#include "Map.hpp"
#include "Graphics.hpp"
#include "RawReader.hpp"
#include <GL/gl.h>

bool Running = true;

void HandleInput()
{
	std::vector<Key_Type> _Status;
	Windows::KeyLock.lock();
	_Status = Windows::GetKeyStatus();
	Windows::KeyLock.unlock();
	s8 Direction = -1;
	// Loop through until empty
	while(!_Status.empty())
	{
		std::vector<Key_Type>::iterator it = _Status.begin();
		switch(*it)
		{
			case Key_Type::KEY_W:
				Direction = 0;
			break;
			case Key_Type::KEY_A:
				Direction = 3;
			break;
			case Key_Type::KEY_S:
				Direction = 2;
			break;
			case Key_Type::KEY_D:
				Direction = 1;
			break;
		}
		printf("%d\n", *it);
		_Status.erase(it);
	}
	/*[pkt header]  [Main Chunk                                                         ] [Footer]        
	CC CC LL LL   FF II II DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD L2 L2

	CC = Command (this is the command the packet is trying to invoke)
	LL = Packet length
	FF = Function, If the command invoked has more than one function this can be used to distinguish them instead of a whole new command
	II = ID of target client, (more of a security thing than anything, Tells the server / client [mostly server] who sent the packet without relying on socketID)
	DD = Data, could be a large amount.. may need to make LL more than 2 bytes in the future..
	L2 = more security stuff. this is the length of the packet / 2 or the value of LL / 2. Mostly there to make sure people aren't mashing packets together and ensuring
        data read lengths aren't mixing together*/
    if(Direction != -1)
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
	}
   
   
}
int main(int argc, char **argv)
{
	cMap Test;
	Windows::Init();
	Graphics::Init();
	Test.Load(5);
	for(;Running;usleep(500000))
	{
		HandleInput();
		Graphics::Clear();
		Test.Draw();
		Graphics::Swap();
	}
	Windows::Shutdown();
	return 0;
}
