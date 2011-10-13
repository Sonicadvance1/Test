
// This may need to be reworked a bit for a Client side socket.
class cSocket
{
	private:
		// Our file descriptor of a socket
		int _Socket;
	public:
		// Opens a new socket at the port
		bool Open(unsigned short Port);
		// Set our socket to listen for connections
		bool Listen();
};
