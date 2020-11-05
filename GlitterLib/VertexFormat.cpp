#include "VertexFormat.h"

namespace Glitter
{
	VertexFormat::VertexFormat()
	{

	}

	VertexFormat::VertexFormat(VertexFormat *clone)
	{
		elements = clone->elements;
		size = clone->size;
	}

	VertexFormat::VertexFormat(unsigned int type)
	{
		if (type == VERTEX_FORMAT_PC)
		{
			VertexFormatElement position(0, VECTOR3, POSITION, 0);
			VertexFormatElement normal(12, VECTOR3, NORMAL, 0);
			VertexFormatElement tangent(24, VECTOR3, TANGENT, 0);
			VertexFormatElement binormal(36, VECTOR3, BINORMAL, 0);
			VertexFormatElement uv_1(48, VECTOR2, UV, 0);
			VertexFormatElement uv_2(56, VECTOR2, UV, 1);
			VertexFormatElement uv_3(64, VECTOR2, UV, 1);
			VertexFormatElement uv_4(72, VECTOR2, UV, 1);
			VertexFormatElement color(80, VECTOR4, RGBA, 0);
			VertexFormatElement bone_indices(96, INDICES, BONE_INDICES, 0);
			VertexFormatElement bone_weights(100, VECTOR4_CHAR, BONE_WEIGHTS, 0);

			addElement(position);
			addElement(normal);
			addElement(binormal);
			addElement(tangent);
			addElement(uv_1);
			addElement(uv_2);
			addElement(uv_3);
			addElement(uv_4);
			addElement(color);
			addElement(bone_indices);
			addElement(bone_weights);

			size = 104;
		}
		else if (type == VERTEX_FORMAT_PC_TERRAIN)
		{
			VertexFormatElement position(0, VECTOR3, POSITION, 0);
			VertexFormatElement normal(12, VECTOR3, NORMAL, 0);
			VertexFormatElement tangent(24, VECTOR3, TANGENT, 0);
			VertexFormatElement binormal(36, VECTOR3, BINORMAL, 0);
			VertexFormatElement uv_1(48, VECTOR2, UV, 0);
			VertexFormatElement uv_2(56, VECTOR2, UV, 1);
			VertexFormatElement color(64, VECTOR4, RGBA, 0);

			addElement(position);
			addElement(normal);
			addElement(binormal);
			addElement(tangent);
			addElement(uv_1);
			addElement(uv_2);
			addElement(color);

			size = 80;
		}
	}

	std::list<VertexFormatElement> VertexFormat::getElements()
	{
		return elements;
	}

	void VertexFormat::addElement(VertexFormatElement element)
	{
		elements.push_back(element);
	}

	unsigned int VertexFormat::getSize()
	{
		return size;
	}

	void VertexFormat::setSize(unsigned int size)
	{
		this->size = size;
	}

	void VertexFormat::fixForPC()
	{
		size_t currentOffset = 0;
		for (std::list<VertexFormatElement>::iterator it = elements.begin(); it != elements.end(); ++it)
		{
			(*it).setOffset((*it).getOffset() + currentOffset);
			if ((*it).getData() == VECTOR3_360)
			{
				(*it).setData(VECTOR3);
				currentOffset += 8;
			}

			if ((*it).getData() == VECTOR2_HALF)
			{
				(*it).setData(VECTOR2);
				currentOffset += 4;
			}

			if ((*it).getData() == VECTOR4_CHAR && (*it).getId() == RGBA)
			{
				(*it).setData(VECTOR4);
				currentOffset += 12;
			}
		}

		size += currentOffset;
	}

	void VertexFormat::read(BinaryReader* reader)
	{
		uint32_t headerAddress = reader->getCurrentAddress();
		for (int i = 0; i < VERTEX_FORMAT_MAX_ENTRIES; ++i)
		{
			reader->gotoAddress(headerAddress + i * 12);
			unsigned int offset = reader->readInt32();
			if (offset > 1000)
				break;

			unsigned int data = reader->readInt32();
			unsigned short id = reader->readInt16();
			unsigned char index = reader->readChar();

			VertexFormatElement formatElement;
			formatElement.setOffset(offset);
			formatElement.setData((VertexElementData)data);
			formatElement.setId((VertexElementID)id);
			formatElement.setIndex(index);

			elements.push_back(formatElement);
		}
	}

	void VertexFormat::write(BinaryWriter* writer)
	{
		uint32_t headerAddress = writer->getCurrentAddress();
		for (std::list<VertexFormatElement>::iterator it = elements.begin(); it != elements.end(); ++it)
		{
			unsigned int offset	= (*it).getOffset();
			unsigned int data	= (*it).getData();
			unsigned short id	= (*it).getId();
			unsigned char index = (*it).getIndex();

			writer->writeInt32(offset);
			writer->writeInt32(data);
			writer->writeInt16(id);
			writer->writeChar(index);
			writer->fixPadding();
		}

		unsigned int fillerOffset = 16711680;
		unsigned int fillerId = 0xFFFFFFFF;
		writer->writeInt32(fillerOffset);
		writer->writeInt32(fillerId);
		writer->writeNull(4);
	}
}