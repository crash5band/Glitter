#include "Parameter.h"

namespace Glitter
{
	Parameter::Parameter()
	{

	}

	Parameter::Parameter(std::string n, Color c) :
		name{ n }, color{ c }
	{

	}

	void Parameter::read(BinaryReader* reader)
	{
		size_t headerAddress = reader->getCurrentAddress();

		reader->gotoAddress(headerAddress + 4);
		size_t nameAddress = reader->readAddress();
		size_t colorAddress = reader->readAddress();

		reader->gotoAddress(nameAddress);
		name = reader->readString();
		reader->gotoAddress(colorAddress);
		color = reader->readRGBA();
	}

	void Parameter::write(BinaryWriter* writer)
	{
		size_t headerAddress = writer->getCurrentAddress();
		unsigned char flag1 = 2;
		unsigned char flag2 = 1;
		
		writer->writeChar(flag1);
		writer->writeNull(1);
		writer->writeChar(flag2);
		writer->writeNull(1);

		writer->writeNull(8);
		size_t nameAddress = writer->getCurrentAddress();
		writer->writeString(name.c_str());
		writer->fixPadding();

		size_t colorAddress = writer->getCurrentAddress();
		writer->write(color, false);

		writer->gotoAddress(headerAddress + 4);
		writer->writeAddress(nameAddress);
		writer->writeAddress(colorAddress);
		writer->gotoEnd();
	}
}