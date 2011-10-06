#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.h"
#include "Player.h"
#include "Window.hpp"
#include "Graphics.hpp"
// We should move these to specific files later

int main(int argc, char **argv)
{
	Windows::Init();
	Graphics::Init();
	for(;;usleep(5000))
	{
		Graphics::Clear();
		Graphics::DrawRect({100, 100, 50, 50});
		Graphics::Swap();
	}
	Windows::Shutdown();
	return 0;
}
