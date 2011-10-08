#ifndef RAWREADER_HPP_
#define RAWREADER_HPP_

#include "Common.hpp"
#include <stdio.h>
#include <string.h>

namespace RawReader
{
	template<class T>
	// Reads out a variable of the template size and increases buffer's pointer by the amount.
	T Read(u8 **Buffer)
	{
		T tmp;
		memcpy(&tmp, *Buffer, sizeof(tmp));
		*Buffer += sizeof(tmp);
		return tmp;
	}
}
#endif
