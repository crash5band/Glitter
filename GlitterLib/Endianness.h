#pragma once

namespace Glitter
{
	enum class Endianness
	{
		LITTLE,
		BIG
	};

	void swapEndianness(unsigned int& x);
	void swapEndianness(int& x);
	void swapEndianness(unsigned short& x);
}