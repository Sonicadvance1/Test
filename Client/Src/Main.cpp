#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.h"
#include "Player.h"
#include "Window.hpp"
// We should move these to specific files later

int main(int argc, char **argv)
{
	Windows::Init();
	usleep(5000000);
	Windows::Shutdown();
	return 0;
}
