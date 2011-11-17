#include <GL/glxew.h>
#include <GL/gl.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <thread>

#include "../Window.hpp"

	int screen;
	Window win;
	Window parent;
	// dpy used for glx stuff, evdpy for window events etc.
	// evdpy is to be used by XEventThread only
	Display *dpy;
	XVisualInfo *vi;
	GLXContext ctx;
	XSetWindowAttributes attr;
	std::thread xEventThread;
	
	std::vector<u32> _KeyStatus;

void XEventThread();
void CreateXWindow();

void CreateXWindow(void)
{
	Atom wmProtocols[1];

	// Setup window attributes
	attr.colormap = XCreateColormap(dpy,
			parent, vi->visual, AllocNone);
	attr.event_mask = KeyPressMask | StructureNotifyMask | FocusChangeMask | ButtonPressMask | ButtonReleaseMask | ButtonMotionMask;
	attr.background_pixel = BlackPixel(dpy, screen);
	attr.border_pixel = 0;

	// Create the window
	win = XCreateWindow(dpy, parent,
			0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, 0,
			vi->depth, InputOutput, vi->visual,
			CWBorderPixel | CWBackPixel | CWColormap | CWEventMask, &attr);
	wmProtocols[0] = XInternAtom(dpy, "WM_DELETE_WINDOW", True);
	XSetWMProtocols(dpy, win, wmProtocols, 1);
	XSetStandardProperties(dpy, win, "Test Program", "Test Program", None, NULL, 0, NULL);
	XMapRaised(dpy, win);
	XSync(dpy, True);

}

void XEventThread()
{
	// We will get key events and stuff here
	bool MousePressed[6]; // Mouse Press status
	while(Running)
	{
		XEvent event;
		KeySym key;
		// Let's loop through all the events
		for (u32 num_events = XPending(dpy); num_events > 0; --num_events)
		{
			XNextEvent(dpy, &event);
			switch (event.type)
			{
				// Is a key pressed?
				case KeyPress:
				{
					// Get what key it is.
					key = XLookupKeysym((XKeyEvent*)&event, 0);
					Windows::KeyLock.lock();
					switch (key)
					{
						case XK_w: // UP
							_KeyStatus.push_back(Key_Type::KEY_W);
						break;
						case XK_a: // LEFT
							_KeyStatus.push_back(Key_Type::KEY_A);
						break;
						case XK_s: // DOWN
							_KeyStatus.push_back(Key_Type::KEY_S);
						break;
						case XK_d: // RIGHT
							_KeyStatus.push_back(Key_Type::KEY_D);
						break;
					}
					Windows::KeyLock.unlock();
				}
				break;
				case ButtonPress: // Mouse Button Press
					MousePressed[event.xbutton.button] = true;
					switch(event.xbutton.button)
					{
						case 1: //Left
							_KeyStatus.push_back(Key_Type::MOUSE_1);
						break;
						case 2: // Middle
							_KeyStatus.push_back(Key_Type::MOUSE_2);
						break;
						case 3: // Right
							_KeyStatus.push_back(Key_Type::MOUSE_3);
						break;
						case 4: // Scroll up
							_KeyStatus.push_back(Key_Type::MOUSE_SCROLL_UP);
						break;
						case 5: // Scroll down
							_KeyStatus.push_back(Key_Type::MOUSE_SCROLL_DOWN);
						break;
						default:
							printf("We don't know mouse Press: %d\n", event.xbutton.button);
						break;
					}
					_KeyStatus.push_back(event.xbutton.x);
					_KeyStatus.push_back(event.xbutton.y);
					_KeyStatus.push_back(true);
				break;
				case ButtonRelease: // Mouse Button Release
					MousePressed[event.xbutton.button] = false;
					switch(event.xbutton.button)
					{
						case 1: //Left
							_KeyStatus.push_back(Key_Type::MOUSE_1);
						break;
						case 2: // Middle
							_KeyStatus.push_back(Key_Type::MOUSE_2);
						break;
						case 3: // Right
							_KeyStatus.push_back(Key_Type::MOUSE_3);
						break;
						case 4: // Scroll up
							_KeyStatus.push_back(Key_Type::MOUSE_SCROLL_UP);
						break;
						case 5: // Scroll down
							_KeyStatus.push_back(Key_Type::MOUSE_SCROLL_DOWN);
						break;
						default:
							printf("We don't know mouse Release: %d\n", event.xbutton.button);
						break;
					}
					_KeyStatus.push_back(event.xbutton.x);
					_KeyStatus.push_back(event.xbutton.y);
					_KeyStatus.push_back(false);
				break;
				case MotionNotify: // Mouse Movement
					if(MousePressed[1] || MousePressed[2])
					{
						if(MousePressed[1])
							_KeyStatus.push_back(Key_Type::MOUSE_1);
						else if(MousePressed[2])
							_KeyStatus.push_back(Key_Type::MOUSE_2);
						
						_KeyStatus.push_back(event.xmotion.x);
						_KeyStatus.push_back(event.xmotion.y);
						_KeyStatus.push_back(true);
					}
					// We don't handle this yet
				break;
				case ClientMessage: // For Window close event
				{
					if (event.xclient.data.l[0] == (long)XInternAtom(dpy, "WM_DELETE_WINDOW", False))
						Running = false;
				}
				break;
			}
		}
		usleep(100);
	}
}


namespace Windows
{
	bool bInitialized = false;
	int iBackBufferWidth, iBackBufferHeight;
	std::mutex KeyLock;

	sRect GetScreenDim()
	{
		return {0, 0, iBackBufferWidth, iBackBufferHeight};
	}
	bool Init()
	{
		XInitThreads(); 
		int glxMajorVersion, glxMinorVersion;
		iBackBufferWidth = DEFAULT_WIDTH;
		iBackBufferHeight = DEFAULT_HEIGHT;
		// attributes for a double buffered visual in RGBA format with at least
		// 8 bits per color and a 24 bit depth buffer
		int attrListDbl[] = {GLX_RGBA, GLX_DOUBLEBUFFER,
			GLX_RED_SIZE, 8,
			GLX_GREEN_SIZE, 8,
			GLX_BLUE_SIZE, 8,
			GLX_DEPTH_SIZE, 8,
			None };


		dpy = XOpenDisplay(0);
		screen = DefaultScreen(dpy);
		parent = RootWindow(dpy, screen);

		glXQueryVersion(dpy, &glxMajorVersion, &glxMinorVersion);
		printf("glX-Version %d.%d\n", glxMajorVersion, glxMinorVersion);

		// Get an appropriate visual
		vi = glXChooseVisual(dpy, screen, attrListDbl);
		if (vi == NULL)
		{
			printf("Couldn't get buffer!\n");
			return false;
		}
		else
			printf("Got double buffered visual!\n");

		// Create a GLX context.
		ctx = glXCreateContext(dpy, vi, 0, GL_TRUE);
		if (!ctx)
		{
			printf("Unable to create GLX context.\n");
			return false;
		}

		CreateXWindow();
		glXMakeCurrent(dpy, win, ctx);
		xEventThread = std::thread(XEventThread);
		return true;
	}

	void Shutdown()
	{
		XUnmapWindow(dpy, win);
		win = 0;
		if (xEventThread.joinable())
			xEventThread.join();
		XFreeColormap(dpy, attr.colormap);

		if (ctx && !glXMakeCurrent(dpy, None, NULL))
			printf("Couldn't grab drawing context\n");
		if (ctx)
		{
			glXDestroyContext(dpy, ctx);
			XCloseDisplay(dpy);
			ctx = NULL;
		}
	}
	void SwapBuffers()
	{
		glXSwapBuffers(dpy, win);
	}

	GLXContext Context()
	{
		return ctx;
	}
	Display* DPY()
	{
		return dpy;
	}
	std::vector<u32> GetKeyStatus()
	{
		std::vector<u32> tmp = _KeyStatus;
		_KeyStatus.clear();
		return tmp;
	}
}

