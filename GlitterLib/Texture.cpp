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

		uvIndex = reader->readChar();
		wrapModeU = (TextureWrapMode)reader->readChar();
		wrapModeV = (TextureWrapMode)reader->readChar();

		// padding
		reader->moveAddress(1);

		size_t unitAddress = reader->readAddress();

		reader->gotoAddress(nameAddress);
		name = reader->readString();
		reader->gotoAddress(unitAddress);
		unit = reader->readString();
	}

	void Texture::readFile(const std::string& filename, const std::string& texset)
	{
		BinaryReader reader(filename, Endianness::BIG);
		if (!reader.valid())
			return;

		reader.readHeader();
		read(&reader, texset);
		reader.close();
	}

	void Texture::write(BinaryWriter* writer)
	{
		size_t headerAddress = writer->getCurrentAddress();
		writer->writeNull(4);
		writer->writeChar(uvIndex);
		writer->writeChar((uint8_t)wrapModeU);
		writer->writeChar((uint8_t)wrapModeV);
		writer->writeNull(5);
		
		size_t fileAddress = writer->getCurrentAddress();
		writer->writeString(name.c_str());

		size_t unitAddress = writer->getCurrentAddress();
		writer->writeString(unit.c_str());
		writer->fixPadding();

		writer->gotoAddress(headerAddress);
		writer->writeAddress(fileAddress);
		writer->gotoAddress(headerAddress + 8);
		writer->writeAddress(unitAddress);
		writer->gotoEnd();
	}

	void Texture::writeFile(const std::string& filename)
	{
		BinaryWriter writer(filename, Endianness::BIG);
		
		writer.prepareHeader(1);
		writer.writeHeader(true);
		write(&writer);
		writer.close();
	}
}