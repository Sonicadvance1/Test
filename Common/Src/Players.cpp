#include "Players.hpp"

#include <map>
namespace Players
{
	// A Map makes this really easy
	std::map<u32, cPlayer*> _Players;

	// Lock since we access this from multiple threads
	// Anything dealing with _Players should be around mutex locks
	std::mutex _Lock;
	
	void InsertPlayer(const u32 ID, cPlayer *Player)
	{
		// again, makes this really simple
		_Lock.lock();
		_Players[ID] = Player;
		_Lock.unlock();
	}
	void RemovePlayer(const u32 ID)
	{
		// Removing player from the map is really easy
		_Lock.lock();
		if(_Players.erase(ID) != 1)
			printf("Couldn't erase player by ID: %d\n", ID);
		_Lock.unlock();
	}
	void SendAll(u8* Buffer, u32 Size, u32 Skip)
	{
		std::map<u32, cPlayer*>::iterator it;
		_Lock.lock();
		for (it = _Players.begin(); it != _Players.end(); ++it)
		if(Skip != it->first)
			it->second->Send(Buffer, Size); // Send the packet away, we don't care about return types here
		_Lock.unlock();
	}
	std::map<u32, cPlayer*> GetArray()
	{
		_Lock.lock();
		std::map<u32, cPlayer*> Vector = _Players;
		_Lock.unlock();
		return Vector;
	}
}

cPlayer::cPlayer(cSocket *Socket)
{
	
	_Socket = Socket;
	// Sets the ID to the socket IP on initialization
	// Later on changes to actual ID pulled from the Database
	_ID = Socket->IP();
	_t = std::thread(&cPlayer::Player_Thread, this);
	// Initialize some of our variables here.
	_Init();
}

cPlayer::~cPlayer()
{
	delete _Socket;
	_t.join();
}
sCoord cPlayer::Coord()
{
	return _Coord;
}

void cPlayer::Move()
{
	// Player Movement
	if(_Moving)
	{
		_Coord.Y -= MOVE_AMOUNT * cos(_Angle);
		_Coord.X += MOVE_AMOUNT * sin(_Angle);
	}
	//Gravity
	s32 X = (s32)_Coord.X;
	s32 Y = (s32)_Coord.Y;
	s32 Z = (s32)_Coord.Z;
	// Subtract one to get the tile below the player
	Z--;
	if(!Maps::Map(_MapID)->Exists(make_triple(X, Y, Z))) // TODO: if no tile below
	{
		if(_Coord.Z < -64) // TODO: Max Width and height
			_Coord.Z = 64;
		_Coord.Z -= MOVE_AMOUNT;
	}
	
}
