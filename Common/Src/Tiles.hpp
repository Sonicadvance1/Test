#ifndef TILES_HPP_
#define TILES_HPP_
#include <map>

#include "RawReader.hpp"

enum MAP_TYPES
{
	MAP_TILE = 0x0,
	MAP_ENTITY
};

enum TILE_TYPE : u16
{
	GRASS = 0x0,
	COTTON_BLACK,
	COTTON_GREY,
	COTTON_RED,
	COTTON_PINK,
	COTTON_GREEN,
	COTTON_LGREEN,
	COTTON_BROWN,
	COTTON_YELLOW,
	COTTON_DBLUE,
	COTTON_BLUE,
	COTTON_PURPLE,
	COTTON_LPURPLE,
	COTTON_LBLUE,
	COTTON_ORANGE,
	COTTON_LGREY
};

// std::triple
#define make_triple(X, Y, Z) std::make_pair(std::make_pair(X, Y), Z)

typedef std::pair<std::pair<f32, f32>, f32> TileTriple;
// Class cTile
class cTile
{
	public:
		f32 _X, _Y, _Z;
		TILE_TYPE _Type; // This is what type of tile it is
	public:
	cTile(){}
	cTile(f32 X, f32 Y, f32 Z, TILE_TYPE Type)
	{
		_X = X;
		_Y = Y;
		_Z = Z;
		_Type = Type;
	}
	cTile(u8 **Buffer) 
	{
		_X = RawReader::Read<f32>(Buffer);
		_Y = RawReader::Read<f32>(Buffer);
		_Z = RawReader::Read<f32>(Buffer);
		_Type = RawReader::Read<TILE_TYPE>(Buffer);
	}
	TileTriple Triple()
	{
		return make_triple(_X, _Y, _Z);
	}
	u32 Read(u8** Out)
	{
		u32 Size = RawReader::Write<u8>(Out, MAP_TILE);
		Size += RawReader::Write<f32>(Out, _X);
		Size += RawReader::Write<f32>(Out, _Y);
		Size += RawReader::Write<f32>(Out, _Z);
		Size += RawReader::Write<TILE_TYPE>(Out, _Type);
		return Size;
	}
	TILE_TYPE Type()
	{
		return _Type;
	}
	
};

typedef std::map<TileTriple, cTile> TileMap;
float* GetTexCoord(TILE_TYPE Type, float *Coords);
float* GetColourCoord(TILE_TYPE Type, float *Coords);
#endif

