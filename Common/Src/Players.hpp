#ifndef PLAYERS_HPP_
#define PLAYERS_HPP_
#include "Common.hpp"
#include "Socket.hpp"
#include <thread>
#include <map>
#include <string.h>
#include <math.h>

#define MOVE_AMOUNT 0.05

class cPlayer
{
	private:
	// Coordinates
		sCoord _Coord;
	// Movement Angle
		double _Angle;
	// Are we moving?
		bool _Moving;
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
	// Just does some pre-initialization
		inline void _Init()
		{
			_Moving = false;
			_Angle = 0;
		}
	public:
		cPlayer(u32 ID = 0)
		{
			_ID = ID;
			// Initialize some variables here
			_Init();
		}
		cPlayer(cSocket *Socket);
		~cPlayer();
		sCoord Coord();
		// Move with a radian angle
		// Negative is up, positive is down
		void SetMove(double Angle)
		{
			_Angle = Angle;
			_Moving = true;
		}
		void StopMove()
		{
			_Moving = false;
		}
		void Move()
		{
			if(_Moving)
			{
				_Coord.Y -= MOVE_AMOUNT * cos(_Angle);
				_Coord.X += MOVE_AMOUNT * sin(_Angle);
			}
		}
		void SetCoord(f32 X, f32 Y, f32 Z)
		{
			_Coord.Y = Y;
			_Coord.X = X;
			_Coord.Z = Z;
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
