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
	u32 CreatePacket(u8 *Buffer, CommandType Command, SubCommandType SubCommand, u16 ID, u8 *Data, u32 DataSize)
	{
		// Basically create a buffer with
		// size of 2 + 2 + 1 + 2 + sizeof(Data) + 2
		// Really just DataSize + 9, no null terminator
		const u16 PacketSize = DataSize + 9;
		const u16 HalfPacketSize = PacketSize / 2; // Is this really needed?
		//Buffer = new u8[PacketSize]; // Should we allocate the packet size here?
		memcpy(&Buffer[0], &Command, 2);
		memcpy(&Buffer[2], &PacketSize, 2);
		memcpy(&Buffer[4], &SubCommand, 1);
		memcpy(&Buffer[5], &ID, 2);
		if(DataSize != 0) // Skip copying Data if there just isn't any
			memcpy(&Buffer[7], &Data, DataSize);
		memcpy(&Buffer[7 + DataSize + 1], &HalfPacketSize, 2);
		return PacketSize;
	}
}
#endif
