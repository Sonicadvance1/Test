#include <GL/glxew.h>
#include <GL/gl.h>
#include "../Window.hpp"
#include <math.h>


namespace Graphics
{

	void DrawRect(sRect Quad)
	{
		// we need to NOT use immediate mode, is super slow compared to other ways
		glPushMatrix();
		glBegin(GL_QUADS);
			glColor4f(0.5, 1.0, 1.0, 1.0);
			glVertex3f(Quad.x, Quad.y, -32); // Top Left
			glVertex3f(Quad.x + Quad.w, Quad.y, -32); // Top Right
			glVertex3f(Quad.x + Quad.w, Quad.y + Quad.h, -32); // Bottom Right
			glVertex3f(Quad.x, Quad.y + Quad.h, -32); // Bottom Left
		glEnd();
		glPopMatrix();
	}

	void Init()
	{
		sRect Dims = Windows::GetScreenDim();
		glViewport(Dims.x, Dims.y, Dims.w, Dims.h); // Set our viewport to the window size
		glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
		glLoadIdentity();                           // Reset The Projection Matrix
		gluPerspective(45.0f, float(Dims.w) / float(Dims.h), 0.1f, 128.0f);
		glMatrixMode(GL_MODELVIEW);                        // Select The Projection Matrix
		glLoadIdentity();                           // Reset The Projection Matrix
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);                   // Black Background
		

		glClearDepth(1.0f);                         // Depth Buffer Setup
		glEnable(GL_DEPTH_TEST);                        // Enables Depth Testing
	}
	void Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 
	}
	void Swap()
	{
		// Just a redirect
		Windows::SwapBuffers();
	}
}
