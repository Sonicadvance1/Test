#ifndef GRAPHICS_HPP_
#define GRAPHICS_HPP_

struct sRect
{
	int x, y, w, h;
};
struct sfRect
{
	float x, y, w, h;
};

namespace Graphics
{
	void Init();
	void DrawRect(sRect Quad);
	void Clear();
	void Swap();
}
#endif
