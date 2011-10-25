#include "Players.hpp"

#include <map>
namespace Players
{
	// A Map makes this really easy
	std::map<u32, cPlayer*> _Players;
	
	void InsertPlayer(const u32 ID, cPlayer *Player)
	{
		// again, makes this really simple
		_Players[ID] = Player;
	}
}

cPlayer::cPlayer(cSocket *Socket)
{
	
	_Socket = Socket;
	// Currently setting ID to socket
	// TODO: It should be a unique ID tied to the account name
	// Currently setting to the IP
	_ID = Socket->IP();
	_t = std::thread(&cPlayer::Player_Thread, this);
}

cPlayer::~cPlayer()
{
	delete _Socket;
}
sCoord cPlayer::Coord()
{
	return _Coord;
}
