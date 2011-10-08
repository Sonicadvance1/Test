#ifndef GRAPHICS_HPP_
#define GRAPHICS_HPP_
#include "Common.hpp"

// Rectangles for 2D
struct sRect
{
	s64 x, y, w, h;
};
struct sfRect
{
	float x, y, w, h;
};
// Cube for 3D
struct sCube
{
	s64 x, y, w, h;
	s64 depth;
};

namespace Graphics
{
	void Init();
	void DrawRect(sRect Quad);
	void DrawCube(sCube Cube);
	void Clear();
	void Swap();
}
#endif
