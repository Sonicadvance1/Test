#ifndef PACKETTYPES_HPP_
#define PACKETTYPES_HPP_
#include "Common.hpp"

enum class CommandType : u16
{
	LOGIN = 0x0,
	CREATE_ACCOUNT,
	PLAYERDATA,
	MOVEMENT,

	NONE = 0xFFFF // End of types
};
enum class SubCommandType : u8 // Not sure if we will need this
{
	PLAYERDATA_NAME = 0x0,
	NONE = 0xFF
};

#endif
