#ifndef COMMON_HPP_
#define COMMON_HPP_
// Some defines

//unsigned types
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

// signed types
typedef signed char s8;
typedef signed short s16;
typedef signed int s32;
typedef signed long long s64;

// Float types
typedef float f32;
typedef double f64;

// Coordinates
struct sCoord
{
	f32 X, Y, Z;
};

// Running?
extern bool Running;
#endif

