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

	// These two should be used with tiles that are the same texture on all sides, if we
	// Want one that is different on each side, won't be much harder 
	f32 XMulti, YMulti;
	switch(Type)
	{
		case TILE_TYPE::GRASS:
		// Grass is at tile 0, 15
			XMulti = 0;
			YMulti = 15;
		break;
		case TILE_TYPE::COTTON_BLACK:
		//Black at 1, 8
			XMulti = 1;
			YMulti = 8;
		break;
		case TILE_TYPE::COTTON_GREY:
			XMulti = 2;
			YMulti = 8;
		break;
		case TILE_TYPE::COTTON_RED:
			XMulti = 1;
			YMulti = 7;
		break;
		case TILE_TYPE::COTTON_PINK:
			XMulti = 2;
			YMulti = 7;
		break;
		case TILE_TYPE::COTTON_GREEN:
			XMulti = 1;
			YMulti = 6;
		break;
		case TILE_TYPE::COTTON_LGREEN:
			XMulti = 2;
			YMulti = 6;
		break;
		case TILE_TYPE::COTTON_BROWN:
			XMulti = 1;
			YMulti = 5;
		break;
		case TILE_TYPE::COTTON_YELLOW:
			XMulti = 2;
			YMulti = 5;
		break;
		case TILE_TYPE::COTTON_DBLUE:
			XMulti = 1;
			YMulti = 4;
		break;
		case TILE_TYPE::COTTON_BLUE:
			XMulti = 2;
			YMulti = 4;
		break;
		case TILE_TYPE::COTTON_PURPLE:
			XMulti = 1;
			YMulti = 3;
		break;
		case TILE_TYPE::COTTON_LPURPLE:
			XMulti = 2;
			YMulti = 3;
		break;
		case TILE_TYPE::COTTON_LBLUE:
			XMulti = 1;
			YMulti = 2;
		break;
		case TILE_TYPE::COTTON_ORANGE:
			XMulti = 2;
			YMulti = 2;
		break;
		case TILE_TYPE::COTTON_LGREY:
			XMulti = 1;
			YMulti = 1;
		break;
		default:
			printf("No idea what %d is!\n", Type);
			exit(0);
		break;
	}
	for(int a = 0; a < 48; a +=8)
	{
		Coords[a] = ONETILE * (XMulti); Coords[a + 1] = ONETILE * (YMulti + 1); // top left
		Coords[a + 2] = ONETILE * (XMulti); Coords[a + 3] = ONETILE * (YMulti); // Bottom Left
		Coords[a + 4] = ONETILE * (XMulti + 1); Coords[a + 5] = ONETILE * (YMulti); // Bottom Right
		Coords[a + 6] = ONETILE * (XMulti + 1); Coords[a + 7] = ONETILE * (YMulti + 1); // Top Right
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
		case TILE_TYPE::COTTON_BLACK:
		case TILE_TYPE::COTTON_GREY:
		case TILE_TYPE::COTTON_RED:
		case TILE_TYPE::COTTON_PINK:
		case TILE_TYPE::COTTON_GREEN:
		case TILE_TYPE::COTTON_LGREEN:
		case TILE_TYPE::COTTON_BROWN:
		case TILE_TYPE::COTTON_YELLOW:
		case TILE_TYPE::COTTON_DBLUE:
		case TILE_TYPE::COTTON_BLUE:
		case TILE_TYPE::COTTON_PURPLE:
		case TILE_TYPE::COTTON_LPURPLE:
		case TILE_TYPE::COTTON_LBLUE:
		case TILE_TYPE::COTTON_ORANGE:
		case TILE_TYPE::COTTON_LGREY:
		default:
			// For anything default, just white colour
			for(int a = 0; a < 96; a += 4)
			{
				Coords[a] = 1.0f; // Red
				Coords[a + 1] = 1.0f; // Green
				Coords[a + 2] = 1.0; // Blue
				Coords[a + 3] = 1.0f; // Alpha
			}
		break;
	}
	return Coords;
}
