#ifndef WINDOW_HPP_
#define WINDOW_HPP_
#include "Graphics.hpp"
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

namespace Windows
{
	extern bool bInitialized;
	extern int iBackBufferWidth, iBackBufferHeight;

	sRect GetScreenDim();

	bool Init();
	void Shutdown();
	void SwapBuffers();
}
#endif
