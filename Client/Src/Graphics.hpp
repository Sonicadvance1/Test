#ifndef GRAPHICS_HPP_
#define GRAPHICS_HPP_
#include "Common.hpp"
#include "Players.hpp"
#include <GL/glxew.h>
#include <GL/gl.h>
#include <string>

// Rectangles for 2D
struct sRect
{
	s64 x, y, w, h, z;
};
struct sfRect
{
	f32 x, y, w, h, z;
};
// Cube for 3D
// X, Y, Z is coordinates
// W H D is size
// Width, Height, Depth. Depth Goes down away from viewer so a -1 depth is a 1x1x1 block

struct sCube
{
	s32 x, y, z, w, h, d;
};
struct sfCube
{
	f32 x, y, z, w, h, d;
};

namespace Graphics
{
	void Init();
	GLuint LoadTexture(std::string filename);
	void DrawRect(sfRect Quad, GLuint Tex);
	void DrawCube(sfCube Cube, GLuint Tex);
	void DrawLines(sCoord *Lines, int Count);
	// Draws player PlayerID in relation to Relation
	void DrawPlayer(cPlayer *Player, cPlayer* Relation);
	void Clear();
	void Swap();
}
#endif
