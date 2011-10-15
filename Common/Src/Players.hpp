#include "Common.hpp"
#include "Socket.hpp"
#include <thread>
class cPlayer
{
	private:
	// Coordinates
		f64 _X, _Y, _Z;
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
		cPlayer(cSocket *Socket);
		~cPlayer();
	
};
namespace Players
{
	// This inserts a player in to the global namespace.
	void InsertPlayer(const u32 ID, cPlayer *Player);
}
