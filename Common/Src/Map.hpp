#ifndef MAP_HPP_
#define MAP_HPP_
#include <map>
#include <iostream>
#include <utility>
// std::triple
#define make_triple(X, Y, Z) std::make_pair(std::make_pair(X, Y), Z)


#include "Common.hpp"
#include "RawReader.hpp"
enum
{
	MAP_TILE = 0x0,
	MAP_ENTITY
} MAP_TYPES;

// Class cTile might be moved to it's own file
class cTile
{
	public:
		f32 _X, _Y, _Z;
	public:
	cTile(){}
	cTile(u8 **Buffer)
	{
		_X = RawReader::Read<f32>(Buffer);
		_Y = RawReader::Read<f32>(Buffer);
		_Z = RawReader::Read<f32>(Buffer);
	}
	std::pair<std::pair<f32, f32>, f32> Triple()
	{
		return make_triple(_X, _Y, _Z);
	}
	
};
class cMap
{
	private:
		// Maximum width and height of map.
		u64 _Width, _Height;
		GLuint Tex;
		/*
		 * 	Array of Tiles
		 * Not 100% sure they should be in float for coordinates
		 * I mean, everything should just be decimal based right?
		 * Need to look in to how I want to do this more later
		*/
		std::map<std::pair<std::pair<f32, f32>, f32>, cTile> _Tiles;

		// This is direct to the correct loader
		// Be it entities, tiles or other things in the map
		void LoadObject(u8 **Buffer)
		{
			// Get what type of object it is
			u8 Object = RawReader::Read<u8>(Buffer);
			switch(Object)
			{
				// TODO: Load these in to the map class
				case MAP_TILE:
				{
					cTile tmp(Buffer);
					_Tiles[tmp.Triple()] = tmp;
				}
				break;
				case MAP_ENTITY:
				default:
					// Why are we loading something that we don't know what it is?
					printf("Trying to load unknown object %02x!\n", Object);
					exit(0);
				break;
			}
		}
	public:
		// This will just load the map from a file
		// Something like "./Maps/<ID>"?
		/* Map Layout
		 * <u64><u64>	#_Width, _Height, TODO: Might not need this if we just calculate size automagically
		 * <u8><s64><s64><s64>...	# _Type(Tile, Entity, Something Else?), _X, _Y, _Z. Each _Type will have a certain size and subtypes will have certain types
		 * Z of zero is "Sea" level so to say.
		 * X and Y of Zero is dead center of map.
		*/
		void Load(const u32 ID)
		{
			// Temporary Map
			// Will be loaded from file later.
			u8 Map[] =	{	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// _Width = 64
							0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// _Height = 64
							0x00,											// Object Type = MAP_TILE							
							0x00, 0x00, 0x00, 0x00,	// _X = 0
							0x00, 0x00, 0x00, 0x00,	// _Y = 0
							0x00, 0x00, 0x00, 0x00,	// _Z = 0
							0x00,											// Object Type = MAP_TILE							
							0x00, 0x00, 0x80, 0x3F, // _X = 1
							0x00, 0x00, 0x00, 0x00, // _Y = 0
							0x00, 0x00, 0x00, 0x00, // _Z = 0
							0x00,											// Object Type = MAP_TILE							
							0x00, 0x00, 0x00, 0x40,	// _X = 2
							0x00, 0x00, 0x00, 0x00, // _Y = 0
							0x3F, 0x80, 0x00, 0x00, // _Z = 1
						};
			// TODO: remove this from here, this was just for testing
			Tex = Graphics::LoadTexture("Resources/Watermelon.png");
			union uTest
			{
				f32 ftest;
				u8 utest[4];
			};
			uTest Test;
			Test.ftest = 3.0f;
			printf("%02X%02X%02X%02X\n", Test.utest[0],Test.utest[1],Test.utest[2],Test.utest[3]);
			// Hack for now, will look a lot nicer once we are actually loading from File
			u8 *pMap = &Map[0];
			// Get our width and height
			_Width = RawReader::Read<u64>(&pMap);
			_Height = RawReader::Read<u64>(&pMap);
			// This loads all of the objects now.
			while(pMap != (&Map[0] + sizeof(Map)))
				LoadObject(&pMap);
			
		}
		void Draw()
		{
			std::map<std::pair<std::pair<f32, f32>, f32>, cTile>::iterator it;
			for(it = _Tiles.begin(); it != _Tiles.end(); ++it)
			{
				Graphics::DrawCube({it->second._X, it->second._Y, it->second._Z, 1, 1, 1}, Tex);
			}
		}
};
#endif
