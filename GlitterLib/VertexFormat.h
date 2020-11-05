#pragma once
#include <list>
#include "BinaryReader.h"
#include "BinaryWriter.h"
#include "Constants.h"

namespace Glitter
{

	enum VertexElementData
	{
		VECTOR3 = 2761657,
		VECTOR3_360 = 2761104,
		VECTOR2 = 2892709,
		VECTOR2_HALF = 2892639,
		INDICESB = 1712774,
		INDICES = 1713030,
		VECTOR4 = 1713062,
		VECTOR4_CHAR = 1712262
	};

	enum VertexElementID
	{
		POSITION = 0,
		BONE_WEIGHTS = 1,
		BONE_INDICES = 2,
		NORMAL = 3,
		UV = 5,
		TANGENT = 6,
		BINORMAL = 7,
		RGBA = 10
	};

	class VertexFormatElement
	{
	protected:
		VertexElementData data;
		VertexElementID id;
		unsigned int offset;
		unsigned short index;

	public:
		VertexFormatElement()
		{
		}

		VertexFormatElement(unsigned int offset, VertexElementData data, VertexElementID id, unsigned short index) :
			offset{ offset }, data{ data }, id{ id }, index{ index }
		{
		}
		
		inline VertexElementData getData() const
		{
			return data; 
		}

		inline VertexElementID getId() const
		{
			return id;
		}

		inline unsigned int getOffset() const
		{
			return offset;
		}

		inline unsigned short getIndex() const
		{
			return index;
		}

		inline void setData(VertexElementData data)
		{
			this->data = data;
		}

		inline void setId(VertexElementID id)
		{
			this->id = id;
		}

		inline void setOffset(unsigned int offset)
		{
			this->offset = offset;
		}

		inline void setIndex(unsigned short index)
		{
			this->index = index;
		}
	};

	class VertexFormat
	{
	protected:
		std::list<VertexFormatElement> elements;
		unsigned int size;

	public:
		VertexFormat();
		VertexFormat(VertexFormat* clone);
		VertexFormat(unsigned int type);

		std::list<VertexFormatElement> getElements();
		void addElement(VertexFormatElement element);
		unsigned int getSize();
		void setSize(unsigned int size);
		void fixForPC();

		void read(BinaryReader* reader);
		void write(BinaryWriter* writer);
	};
}