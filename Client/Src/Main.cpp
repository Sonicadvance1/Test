#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "Socket.hpp"
#include "Players.hpp"
#include "Window.hpp"
#include "Map.hpp"
#include "Graphics.hpp"
#include <GL/gl.h>

bool Running = true;

void HandleInput()
{
	std::vector<Key_Type> _Status;
	Windows::KeyLock.lock();
	_Status = Windows::GetKeyStatus();
	Windows::KeyLock.unlock();
	// Loop through until empty
	while(!_Status.empty())
	{
		std::vector<Key_Type>::iterator it = _Status.begin();
		printf("%d\n", *it);
		_Status.erase(it);
	}
}
int main(int argc, char **argv)
{
	cMap Test;
	Windows::Init();
	Graphics::Init();
	Test.Load(5);
	for(;Running;usleep(500000))
	{
		HandleInput();
		Graphics::Clear();
		Test.Draw();
		Graphics::Swap();
	}
	Windows::Shutdown();
	return 0;
}
