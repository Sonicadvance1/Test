#ifndef WINDOW_H_
#define WINDOW_H_
#define DEFAULT_WIDTH 1024
#define DEFAULT_HEIGHT 768

namespace Windows
{
	extern bool bInitialized;

	bool Init();
	void Shutdown();
	void SwapBuffers();
}
#endif
