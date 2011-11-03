#include "Tiles.hpp"
#include <iostream>
#include <utility>
#define TERRAIN_SQUARE 4096.0f // Texture width and height
#define TILE_SQUARE 256.0f // A tile's width and height
#define ONETILE (1.0f / (TERRAIN_SQUARE / TILE_SQUARE))
float* GetTexCoord(TILE_TYPE Type, float *Coords)
{
	// Texture coordinates
	// 0,0 is at bottom left of texture
	switch(Type)
	{
		case TILE_TYPE::GRASS:
			// Grass is at tile 0, 15
			for(int a = 0; a < 48; a +=8)
			{
				Coords[a] = 0; Coords[a + 1] = ONETILE * 16; // top left
				Coords[a + 2] = 0; Coords[a + 3] = ONETILE * 15; // Bottom Left
				Coords[a + 4] = ONETILE; Coords[a + 5] = ONETILE * 15; // Bottom Right
				Coords[a + 6] = ONETILE; Coords[a + 7] = ONETILE * 16; // Top Right
			}
		
		break;
		default:
			printf("No idea what %d is!\n", Type);
			exit(0);
		break;
	}
	return Coords;
}
float* GetColourCoord(TILE_TYPE Type, float *Coords)
{
	// Texture coordinates
	// 0,0 is at bottom left of texture
	switch(Type)
	{
		case TILE_TYPE::GRASS:
			// Grass is at tile 0, 15
			for(int a = 0; a < 96; a += 4)
			{
				Coords[a] = 0; // Red
				Coords[a + 1] = 1.0f; // Green
				Coords[a + 2] = 0; // Blue
				Coords[a + 3] = 1.0f; // Alpha
			}
		
		break;
		default:
			printf("No idea what %d is!\n", Type);
			exit(0);
		break;
	}
	return Coords;
}
