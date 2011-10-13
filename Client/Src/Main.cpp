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
	Windows::Init();
	Graphics::Init();
	Test.Load(5);
	for(;;usleep(500000))
	{
		Graphics::Clear();
		Test.Draw();
		Graphics::Swap();
	}
	Windows::Shutdown();
	return 0;
}
