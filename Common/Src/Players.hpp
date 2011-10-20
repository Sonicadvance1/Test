#ifndef PLAYERS_HPP_
#define PLAYERS_HPP_
#include "Common.hpp"
#include "Socket.hpp"
#include <thread>

#define MOVE_AMOUNT 0.2

class cPlayer
{
	private:
	// Coordinates
		sCoord _Coord;
	// Player ID
	// Right now it is just our IP address
	// Later it will be a ID number tied to the username
		u32 _ID;
	// Thread object
		std::thread _t;
	//Thread Function
		void Player_Thread();
	// Player Socket
		cSocket *_Socket;
	public:
		cPlayer() {};
		cPlayer(cSocket *Socket);
		~cPlayer();
		sCoord Coord();
		void Move(int VelocityX, int VelocityY)
		{
			_Coord.Y += VelocityY * MOVE_AMOUNT;
			_Coord.X += VelocityX * MOVE_AMOUNT;
		}
	
};
namespace Players
{
	// This inserts a player in to the global namespace.
	void InsertPlayer(const u32 ID, cPlayer *Player);
}

#endif
