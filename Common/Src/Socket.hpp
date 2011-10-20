#include "Common.hpp"
#include <arpa/inet.h>


#ifndef SOCKET_HPP_
#define SOCKET_HPP_

// This may need to be reworked a bit for a Client side socket.
class cSocket
{
	private:
		// Our file descriptor of a socket
		s32 _Socket;
		// The IP address
		u32 _IP;
		// Our FD set
		fd_set _Set;

		struct sockaddr_in sa;
	public:
		cSocket();
		cSocket(s32 Socket, u32 IP);
		~cSocket();
		// Opens a new socket at the port
		bool Open(u16 Port);
		// Do we have Data?
		// On Listening socket, returns true if someone is connecting
		bool HasData();
		// Gets data from Socket
		s32 Recv(u8 *buf, u32 size);
		// Just returns the IP
		u32 IP();

		// Client only related
		bool Connect(const char* IP);
		
		// Server only related
		// Set our socket to listen for connections
		bool Listen();
		// This accepts the new connection and returns a socket
		// Returns 0 on error
		cSocket* Accept();
		bool Bind();
};
#endif
