#pragma once
#include <string>
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
	enum class TextureWrapMode : uint8_t
	{
		Repeat,
		Mirror,
		Clamp,
		MirrorOnce,
		Border
	};

	class Texture
	{
	private:
		std::string name;
		std::string internalName;
		std::string unit;
		TextureWrapMode wrapModeU;
		TextureWrapMode wrapModeV;
		uint8_t uvIndex;

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

		inline void setUVIndex(uint8_t index)
		{
			if (index >= 0 && index < 4)
				uvIndex = index;
		}

		inline void setTextureWrapModeU(TextureWrapMode mode)
		{
			wrapModeU = mode;
		}

		inline void setTextureWrapModeV(TextureWrapMode mode)
		{
			wrapModeV = mode;
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

		inline TextureWrapMode getWrapModeU() const
		{
			return wrapModeU;
		}

		inline TextureWrapMode getWrapModeV() const
		{
			return wrapModeV;
		}

		inline uint8_t getUVIndex() const
		{
			return uvIndex;
		}

		void read(BinaryReader* reader, std::string id);
		void write(BinaryWriter* writer);

		void readFile(const std::string& filename, const std::string& texset);
		void writeFile(const std::string& filename);
	};
}
