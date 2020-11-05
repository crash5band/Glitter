#include "Endianness.h"

namespace Glitter
{
#pragma optimize("", off)
	void swapEndianness(unsigned int& x)
	{
		x = (x >> 24) | (x << 24) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00);
	}

	void swapEndianness(int& x)
	{
		x = (x >> 24) | (x << 24) | ((x << 8) & 0x00ff0000) | ((x >> 8) & 0x0000ff00);
	}

	void swapEndianness(unsigned short& x)
	{
		x = (x >> 8) | (x << 8);
	}
#pragma optimize("", on)
}