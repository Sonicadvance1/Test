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
		void Move(int VelocityX, int VelocityY)
		{
			_Coord.Y += VelocityY * MOVE_AMOUNT;
			_Coord.X += VelocityX * MOVE_AMOUNT;
		}
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
	void RemovePlayer(const u32 ID);
	std::map<u32, cPlayer*> GetArray();
}

#endif
