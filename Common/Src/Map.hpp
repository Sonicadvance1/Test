#ifndef MAP_HPP_
#define MAP_HPP_

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
	private:
		u64 _X, _Y, _Z;
	public:
	cTile(){}
	cTile(u8 **Buffer)
	{
		_X = RawReader::Read<u64>(Buffer);
		_Y = RawReader::Read<u64>(Buffer);
		_Z = RawReader::Read<u64>(Buffer);
	}
	
};
class cMap
{
	private:
		// Maximum width and height of map.
		u64 _Width, _Height;
		/*
		 * 	Array of Tiles
		 * TODO: Convert to a three dimensional array
		 * Or perhaps use a std::map with a std::triple<u64 _X, u64 _Y, u16/u32/u64 _X>?
		 * Not 100% how I want it to look yet
		*/
		cTile *_Tiles;

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
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// _X = 0
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// _Y = 0
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// _Z = 0
							0x00,											// Object Type = MAP_TILE							
							0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// _X = 1
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// _Y = 0
							0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// _Z = 0
						};
			// Hack for now, will look a lot nicer once we are actually loading from File
			u8 *pMap = &Map[0];
			// Get our width and height
			_Width = RawReader::Read<u64>(&pMap);
			_Height = RawReader::Read<u64>(&pMap);
			// This loads all of the objects now.
			while(pMap != (&Map[0] + sizeof(Map)))
				LoadObject(&pMap);
			
		}
};
#endif
