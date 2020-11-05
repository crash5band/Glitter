#pragma once
#include <string>
#include "MathGens.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
	class Parameter
	{
	public:
		std::string name;
		Color color;

		Parameter();
		Parameter(std::string n, Color c);

		void read(BinaryReader* reader);;
		void write(BinaryWriter* writer);
	};
}
