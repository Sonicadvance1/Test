#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.h"
#include "Player.h"
#include "Window.hpp"
#include "Graphics.hpp"
#include <GL/gl.h>
// We should move these to specific files later

int main(int argc, char **argv)
{
	Windows::Init();
	Graphics::Init();
	for(;;usleep(500000))
	{

		Graphics::Clear();
		glLoadIdentity();                           // Reset The Projection Matrix
		glRotatef(-25, 1, 0,0);
		glTranslated(0, 16, 0);
		for(int x = -25; x < 25; ++x)
			for(int y = -17; y < 17; ++y)
				if((y % 2 + x % 2))
					Graphics::DrawRect({x, y, 1, 1});
		Graphics::Swap();
	}
	Windows::Shutdown();
	return 0;
}
