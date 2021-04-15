#pragma once
#include <string>
#include "MathGens.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
	class MaterialParam
	{
	public:
		std::string name;
		Color color;

		MaterialParam();
		MaterialParam(std::string n, Color c);

		void read(BinaryReader* reader);;
		void write(BinaryWriter* writer);
	};
}
