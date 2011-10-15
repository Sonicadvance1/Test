#include "Common.hpp"
// This may need to be reworked a bit for a Client side socket.
class cSocket
{
	private:
		// Our file descriptor of a socket
		s32 _Socket;
		// Our FD set
		fd_set _Set;
	public:
		cSocket();
		cSocket(s32 Socket);
		~cSocket();
		// Opens a new socket at the port
		bool Open(u16 Port);
		// Do we have Data?
		// On Listening socket, returns true if someone is connecting
		bool HasData();
		s32 Recv(u8 *buf, u32 size);
		
		// Server only related
		// Set our socket to listen for connections
		bool Listen();
		// This accepts the new connection and returns a socket
		// Returns 0 on error
		s32 Accept();
};
