#include "RawReader.hpp"


namespace RawReader
{
	// Writes a string to the buffer
	// First writes the size of the string
	// Then the actual string
	u32 WriteString(u8 **Buffer, const char *Str, s32 Size)
	{
		u16 StrSize;
		if(Size == -1) // Just a null terminated string
			StrSize = strlen(Str);
		else // a Set Size
			StrSize = Size;
		Write<u16>(Buffer, StrSize);
		memcpy(*Buffer, Str, StrSize);
		*Buffer += StrSize;
		return StrSize + 2;
	}
	u16 ReadString(u8 **Buffer, u8 *Out)
	{
		u16 Size = Read<u16>(Buffer);
		printf("Size is %d:%p\n", Size, *Buffer);
		memcpy(Out, *Buffer, Size);
		Out[Size] = '\0'; // Null Terminate it
		*Buffer += Size;
		return Size;
	}

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
			memcpy(&Buffer[7], Data, DataSize);
		memcpy(&Buffer[7 + DataSize + 1], &HalfPacketSize, 2);
		return PacketSize;
	}
	// Just gets the packets expected size
	u16 GetFullSize(u8* Buffer)
	{
		return (u16)*(Buffer + 2);
	}
	// Just gets the Command
	CommandType GetCommand(u8* Buffer)
	{
		// Command is at offset 0-1
		return (CommandType)*Buffer;
	}
	// Just gets the SubCommand
	SubCommandType GetSubCommand(u8* Buffer)
	{
		// SubCommand is at offset 4
		return (SubCommandType)Buffer[4];
	}
	// Just gets the ID
	u16 GetID(u8* Buffer)
	{
		// ID is at offset 5-6
		return (u16)*(Buffer + 5);
	}

	// Just gets a pointer to the Data section
	u8* GetData(u8 *Buffer)
	{
		return &Buffer[7];
	}
	// Just gets the Datasize
	u32 GetDataSize(u8 *Buffer)
	{
		// Datasize is just the packetsize - 9
		// PacketSize is offset 2-3
		u16 PacketSize = (u16)*(Buffer + 2);
		return PacketSize - 9;
	}
}
