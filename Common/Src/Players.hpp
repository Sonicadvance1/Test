#ifndef PLAYERS_HPP_
#define PLAYERS_HPP_
#include "Common.hpp"
#include "Socket.hpp"
#include <thread>
#include <map>
#include <string.h>

#define MOVE_AMOUNT 0.2

class cPlayer
{
	private:
	// Coordinates
		sCoord _Coord;
	// Player ID
		u32 _ID;
	// Player Name
		u8 _Name[64];
	// Thread object
		std::thread _t;
	//Thread Function
		void Player_Thread();
	// Player Socket
		cSocket *_Socket;
	public:
		cPlayer() {}
		cPlayer(u32 ID) { _ID = ID; }
		cPlayer(cSocket *Socket);
		~cPlayer();
		sCoord Coord();
		void Move(u32 VelocityX, u32 VelocityY)
		{
			_Coord.Y = VelocityY;
			_Coord.X = VelocityX;
		}
		// Sends a packet
		s32 Send(u8 *buf, u32 size)
		{
			return _Socket->Send(buf, size);
		}
		// Sets and gets our player's name
		void SetName(u8 *Name)
		{
			strcpy((char*)_Name, (const char*)Name);
		}
		u8* GetName()
		{
			return _Name;
		}
	
};
namespace Players
{
	// This inserts a player in to the global namespace.
	void InsertPlayer(const u32 ID, cPlayer *Player);
	// removes player from global Namespace
	void RemovePlayer(const u32 ID);
	// Send a packet to all players
	void SendAll(u8* Buffer, u32 Size, u32 Skip = 0);
	std::map<u32, cPlayer*> GetArray();
}

#endif
