#include "Map.hpp"

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
				0x00, 0x00				// _Type = Grass
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
	// Loads map in to the global namespace from a buffer
	void LoadBuffer(u32 ID, u8 *Buffer, u32 Size)
	{
		cMap *Tmp = new cMap;
		Tmp->Load(Buffer, Size);
		_Maps[ID] = Tmp;
	}
	cMap* Map(u32 ID)
	{
		return _Maps[ID];
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
