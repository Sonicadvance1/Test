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

void XEventThread();
void CreateXWindow();

void CreateXWindow(void)
{
	Atom wmProtocols[1];

	// Setup window attributes
	attr.colormap = XCreateColormap(dpy,
			parent, vi->visual, AllocNone);
	attr.event_mask = KeyPressMask | StructureNotifyMask | FocusChangeMask;
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

	xEventThread = std::thread(XEventThread);
}

void XEventThread()
{
	// We will get key events and stuff here
}


namespace Windows
{
	bool bInitialized = false;
	int iBackBufferWidth, iBackBufferHeight;

	sRect GetScreenDim()
	{
		return {0, 0, iBackBufferWidth, iBackBufferHeight};
	}
	bool Init()
	{
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
}

