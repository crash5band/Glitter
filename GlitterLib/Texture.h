#pragma once
#include <string>
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
	class Texture
	{
	private:
		std::string name;
		std::string internalName;
		std::string unit;

	public:
		Texture();
		Texture(std::string name, std::string inName, std::string unit);

		inline void setName(std::string n)
		{
			name = n;
		}

		inline void setInternalName(std::string n)
		{
			internalName = n;
		}

		inline void setUnit(std::string u)
		{
			unit = u;
		}

		inline std::string getName() const
		{
			return name;
		}

		inline std::string getTexSet() const
		{
			return internalName;
		}

		inline std::string getUnit() const
		{
			return unit;
		}

		void read(BinaryReader* reader, std::string id);
		void write(BinaryWriter* writer);
	};
}
