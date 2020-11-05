#include "Bone.h"

namespace Glitter
{
	Bone::Bone()
	{

	}

	void Bone::readDescription(BinaryReader* reader)
	{
		parentIndex = reader->readInt32();
		size_t nameAddress = reader->readAddress();
		reader->gotoAddress(nameAddress);
		name = reader->readString();
	}

	void Bone::readMatrix(BinaryReader* reader)
	{
		matrix = reader->readMatrix4();
	}

	void Bone::writeDescription(BinaryWriter* writer)
	{
		size_t nameAddress = writer->getCurrentAddress() + 8;
		writer->writeInt32(parentIndex);
		writer->writeAddress(nameAddress);
		writer->gotoAddress(nameAddress);
		writer->writeString(name.c_str());
		writer->fixPadding();
	}

	void Bone::writeMatrix(BinaryWriter* writer)
	{
		writer->write(matrix);
	}

	unsigned int Bone::getParentIndex()
	{
		return parentIndex;
	}

	std::string Bone::getName()
	{
		return name;
	}

	Matrix4 Bone::getMatrix()
	{
		return matrix;
	}

}
