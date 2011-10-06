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
		for(int x = -17; x < 17; ++x)
			for(int y = -13; y < 13; ++y)
				if((y % 2 + x % 2))
					Graphics::DrawRect({x, y, 1, 1});
		Graphics::Swap();
	}
	Windows::Shutdown();
	return 0;
}
