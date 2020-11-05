#include "Texture.h"

namespace Glitter
{
	Texture::Texture()
	{

	}

	Texture::Texture(std::string name, std::string internalName, std::string unit)
		: name{ name }, internalName{ internalName }, unit{ unit }
	{

	}

	void Texture::read(BinaryReader* reader, std::string id)
	{
		internalName = id;
		size_t headerAddress = reader->getCurrentAddress();
		
		size_t nameAddress = reader->readAddress();
		reader->gotoAddress(headerAddress + 8);
		size_t unitAddress = reader->readAddress();

		reader->gotoAddress(nameAddress);
		name = reader->readString();
		reader->gotoAddress(unitAddress);
		unit = reader->readString();
	}

	void Texture::write(BinaryWriter* writer)
	{
		size_t headerAddress = writer->getCurrentAddress();
		writer->writeNull(12);
		
		size_t fileAddress = writer->getCurrentAddress();
		writer->writeString(name.c_str());

		size_t unitAddress = writer->getCurrentAddress();
		writer->writeString(unit.c_str());

		writer->gotoAddress(headerAddress);
		writer->writeAddress(fileAddress);
		writer->gotoAddress(headerAddress + 8);
		writer->writeAddress(unitAddress);
		writer->gotoEnd();
	}
}