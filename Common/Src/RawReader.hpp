#ifndef RAWREADER_HPP_
#define RAWREADER_HPP_

#include "Common.hpp"
#include "PacketTypes.hpp"
#include <stdio.h>
#include <string.h>

namespace RawReader
{
	template<class T>
	// Reads out a variable of the template size and increases buffer's pointer by the amount.
	T Read(u8 **Buffer)
	{
		T tmp;
		memcpy(&tmp, *Buffer, sizeof(tmp));
		*Buffer += sizeof(tmp);
		return tmp;
	}
	template<class T>
	// Writes a variable to a buffer, returns how much written
	u32 Write(u8 **Buffer, T Variable)
	{
		memcpy(*Buffer, &Variable, sizeof(Variable));
		*Buffer += sizeof(Variable);
		return sizeof(Variable);
	}
	// Writes a string to the buffer
	// First writes the size of the string
	// Then the actual string
	u32 WriteString(u8 **Buffer, const char *Str, s32 Size = -1);
	
	u16 ReadString(u8 **Buffer, u8 *Out);

	/* This Function correctly forms a packet
	 * [pkt header]  [Main Chunk                                                         ] [Footer]
	 * CC CC LL LL   FF II II DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD L2 L2
	 *
	 * CC = Command (this is the command the packet is trying to invoke)
	 * LL = Packet length
	 * FF = Function, If the command invoked has more than one function this can be used to distinguish them instead of a whole new command
	 * II = ID of target client, (more of a security thing than anything, Tells the server / client [mostly server] who sent the packet without relying on socketID)
	 * DD = Data, could be a large amount.. may need to make LL more than 2 bytes in the future..
	 * L2 = more security stuff. this is the length of the packet / 2 or the value of LL / 2. Mostly there to make sure people aren't mashing packets together and ensuring
	 * data read lengths aren't mixing together
	*/
	u32 CreatePacket(u8 *Buffer, CommandType Command, SubCommandType SubCommand, u16 ID, u8 *Data, u32 DataSize);
	
	// Just gets the packets expected size
	u16 GetFullSize(u8* Buffer);
	// Just gets the Command
	CommandType GetCommand(u8* Buffer);
	// Just gets the SubCommand
	SubCommandType GetSubCommand(u8* Buffer);
	// Just gets the ID
	u16 GetID(u8* Buffer);
	// Just gets a pointer to the Data section
	u8* GetData(u8 *Buffer);
	// Just gets the Datasize
	u32 GetDataSize(u8 *Buffer);
}
#endif
