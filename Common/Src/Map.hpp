#ifndef MAP_HPP_
#define MAP_HPP_
#include <map>
#include <iostream>
#include <utility>
#include "Tiles.hpp"

#define MAP_DIR "Maps/"

#include "Common.hpp"
#include "RawReader.hpp"


class cMap
{
	private:
		// Maximum width and height of map.
		u64 _Width, _Height;
		u8* _Map;
		u32 _MapSize;
		/*
		 * 	Array of Tiles
		 * Not 100% sure they should be in float for coordinates
		 * I mean, everything should just be decimal based right?
		 * Need to look in to how I want to do this more later
		*/
		TileMap _Tiles;

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
					printf("-5, %02X %02X %02X %02X %02X\n", (*Buffer)[-6], (*Buffer)[-5], (*Buffer)[-4], (*Buffer)[-3], (*Buffer)[-2]);
					printf("+5, %02X %02X %02X %02X %02X\n", (*Buffer)[0], (*Buffer)[1], (*Buffer)[2], (*Buffer)[3], (*Buffer)[4]);
					exit(0);
				break;
			}
		}
		void ReloadMap()
		{
			free(_Map);
			_MapSize = 0;
			_Map = (u8*)malloc(_Tiles.size() * 64);
			u8 *pMap = &_Map[0];

			_MapSize += RawReader::Write<u64>(&pMap, _Width);
			_MapSize += RawReader::Write<u64>(&pMap, _Height);
			TileMap::iterator it;
			for(it = _Tiles.begin(); it != _Tiles.end(); ++it)
			{
				_MapSize += it->second.Read(&pMap);
			}
			printf("New Map size: %d\n", _MapSize); 
		}
	public:

		~cMap()
		{
			free(_Map);
			_Tiles.clear();
		}
		// This will just load the map from a file
		// Something like "./Maps/<ID>"?
		/* Map Layout
		 * <u64><u64>	#_Width, _Height, TODO: Might not need this if we just calculate size automagically
		 * <u8><s64><s64><s64>...	# _Type(Tile, Entity, Something Else?), _X, _Y, _Z. Each _Type will have a certain size and subtypes will have certain types
		 * Z of zero is "Sea" level so to say.
		 * X and Y of Zero is dead center of map.
		*/
		void Load(u8 *Buffer, u32 Size) // Load a map from a buffer
		{
			u8 *pBuffer = &Buffer[0];
			_Width = RawReader::Read<u64>(&pBuffer);
			_Height = RawReader::Read<u64>(&pBuffer);
			while(pBuffer != (&Buffer[0] + Size))
				LoadObject(&pBuffer);
			_Map = (u8*)malloc(Size);
			memcpy(_Map, Buffer, Size);
			// Need to record mapsize because sizeof doesn't work here!
			_MapSize = Size;
		}
		void Load(FILE *fp) // Load the map from a file pointer
		{
			if(!fp) // What?
				return;
			long Filesize;
			// First we need to get the file size.
			fseek(fp, 0, SEEK_END);
			Filesize = ftell(fp);
			rewind(fp);
			// Alright, let's malloc us an array.
			u8 *Buffer = new u8[Filesize];
			int result = fread(Buffer, 1, Filesize, fp);
			if(result != Filesize) // What?
			{
				printf("Couldn't read all of the file?! Size: %ld, Read: %d\n", Filesize, result);
				return;
			}
			Load(Buffer, Filesize);
			delete[] Buffer;
		}

		void Save(FILE *fp) // Saves the map to a file pointer
		{
			if(!fp) // What?
				return;
			fwrite(_Map, 1, _MapSize, fp);
		}
		// This will insert an object in to the map
		void InsertObject(u8 *Buffer)
		{
			LoadObject(&Buffer);
			ReloadMap();
		}
		u8* GetMap(u32 *Size)
		{
			*Size = _MapSize;
			return _Map;
		}
		// Draws the map in relation to player
		void Draw(cPlayer* Player);
};

namespace Maps
{
	std::map<u32, cMap*> _Maps;

	// This will load the map in to the global namespace
	void Load(u32 ID)
	{
		char Filename[64];
		cMap *Tmp = new cMap;
		sprintf(Filename, "./"MAP_DIR"%d.map", ID);
		FILE *fp;
		// First let us check to see if it exists or not.
		fp = fopen(Filename, "r");
		if(!fp) // Doesn't exist
		{
			fp = fopen(Filename, "w");
			if(!fp) //derp, folder probably doesn't exist. User will get temporary maps!
				printf("WARNING: PROCEEDING WITH TEMPORARY MAP %d!!\n", ID);
			u8 DefaultMap[] =	{	0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// _Width = 64
				0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// _Height = 64
				0x00,											// Object Type = MAP_TILE							
				0x00, 0x00, 0x00, 0x00,	// _X = 0
				0x00, 0x00, 0x00, 0x00,	// _Y = 0
				0x00, 0x00, 0x00, 0x00,	// _Z = 0
				0x00, 0x00				// _Type = 0
			};
			// Alright, write the default map to this file
			if(fp)
			{
				// Make sure it isnt' a temp map
				fwrite(DefaultMap, 1, sizeof(DefaultMap), fp);
				fclose(fp);
			}
			// Now let's load it as default.
			Tmp->Load(DefaultMap, sizeof(DefaultMap));
			_Maps[ID] = Tmp;
		}
		else // It does exist
		{
			// cMap class supports file descriptors in the load function
			Tmp->Load(fp);
			_Maps[ID] = Tmp;
			fclose(fp);
		}
	}
	void Shutdown()
	{
		std::map<u32, cMap*>::iterator it;
		for(it = _Maps.begin(); it != _Maps.end(); ++it)
		{
			char Filename[64];
			printf("Shutting down: %d\n", it->first);
			sprintf(Filename, "./"MAP_DIR"%d.map", it->first);
			FILE *fp;
			// We don't care if it exists now, just overwrite
			fp = fopen(Filename, "wb");
			if(fp) // Check if it was a temp map.
			{
				it->second->Save(fp);
				fclose(fp);
				delete it->second;
			}
			printf("Shutdown: %d\n", it->first);
		}
		_Maps.clear();
	}
}
#endif
