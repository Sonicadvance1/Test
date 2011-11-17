#ifndef WINDOW_HPP_
#define WINDOW_HPP_
#include "Graphics.hpp"
#include <mutex>
#include <vector>
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

enum Key_Type
{
	KEY_W = 0,
	KEY_A = 1,
	KEY_S = 2,
	KEY_D = 3,
	MOUSE_1 = 4,
	MOUSE_2 = 5,
	MOUSE_3 = 6,
	MOUSE_SCROLL_UP = 7,
	MOUSE_SCROLL_DOWN = 8
};

namespace Windows
{
	extern bool bInitialized;
	extern int iBackBufferWidth, iBackBufferHeight;
	extern std::mutex KeyLock;

	sRect GetScreenDim();

	bool Init();
	void Shutdown();
	void SwapBuffers();

	GLXContext Context();
	Display* DPY();
	
	std::vector<u32> GetKeyStatus();
}
#endif
