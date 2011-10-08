#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.hpp"
#include "Players.hpp"
#include "Window.hpp"
#include "Map.hpp"
#include "Graphics.hpp"
#include <GL/gl.h>
// We should move these to specific files later

int main(int argc, char **argv)
{
	cMap Test;
	Test.Load(5);
	Windows::Init();
	Graphics::Init();
	for(;;usleep(500000))
	{

		Graphics::Clear();
		glLoadIdentity();                           // Reset The Projection Matrix
		glRotatef(-25, 1, 0,0);
		glTranslated(0, 16, 0);
		Test.Draw();
		Graphics::Swap();
	}
	Windows::Shutdown();
	return 0;
}
