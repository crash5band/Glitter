#include "BinaryReader.h"
#include "File.h"

namespace Glitter
{
	BinaryReader::BinaryReader(const std::string& path, Endianness en)
	{
		file = fopen(path.c_str(), "rb");
		if (file)
		{
			filename = File::getFileName(path);
			filepath = File::getFilePath(path);
		}

		endianness = en;
		rootNodeAddress = 0;
		rootNodeType = 0;
		globalOffset = 0;
	}

	BinaryReader::~BinaryReader()
	{
		close();
	}

	void BinaryReader::changeEndianness(Endianness en)
	{
		endianness = en;
	}

	bool BinaryReader::valid() const
	{
		return file != NULL;
	}

	void BinaryReader::close() const
	{
		if (file)
			fclose(file);
	}

	void BinaryReader::setGlobalOffset(size_t offset)
	{
		globalOffset = offset;
	}

	void BinaryReader::gotoAddress(size_t address) const
	{
		if (file)
			fseek(file, globalOffset + address, SEEK_SET);
	}

	void BinaryReader::moveAddress(size_t address) const
	{
		if (file)
			fseek(file, address, SEEK_CUR);
	}

	size_t BinaryReader::getCurrentAddress() const
	{
		if (file)
			return ftell(file) - globalOffset;
	}

	void BinaryReader::gotoEnd() const
	{
		if (file)
			fseek(file, 0, SEEK_END);
	}

	int BinaryReader::getRootNodeType() const
	{
		return rootNodeType;
	}

	size_t BinaryReader::getRootNodeAddress() const
	{
		return rootNodeAddress;
	}

	void BinaryReader::setRootNodeAddress(size_t address)
	{
		rootNodeAddress = address;
	}

	size_t BinaryReader::getFileSize() const
	{
		if (!file)
			return 0;

		size_t prev = ftell(file);
		fseek(file, 0, SEEK_END);
		size_t size = ftell(file);
		fseek(file, prev, SEEK_SET);
		return size;
	}

	void BinaryReader::readHeader()
	{
		uint8_t nextGenCheck = readChar();
		gotoAddress(0);

		if (nextGenCheck == 0x80)
		{
			// ERROR: UNSUPPORTED FILE
		}
		else
		{
			fseek(file, fileHeaderRootTypeAddress + globalOffset, SEEK_SET);
			rootNodeType = readInt32();
			fseek(file, fileHeaderRootNodeAddress + globalOffset, SEEK_SET);
			rootNodeAddress = readInt32();
			fseek(file, rootNodeAddress + globalOffset, SEEK_SET);
		}
	}

	void BinaryReader::readSize(void* data, size_t size)
	{
		if (file)
			fread(data, size, 1, file);
	}

	uint8_t BinaryReader::readChar() const
	{
		uint8_t data = 0;
		if (file)
			fread(&data, sizeof(uint8_t), 1, file);
		
		return data;
	}

	uint16_t BinaryReader::readInt16() const
	{
		uint16_t data = 0;
		if (file)
		{
			fread(&data, sizeof(uint16_t), 1, file);
			if (endianness == Endianness::BIG)
				swapEndianness(data);
		}

		return data;
	}

	uint32_t BinaryReader::readInt32() const
	{
		uint32_t data = 0;
		if (file)
		{
			fread(&data, sizeof(uint32_t), 1, file);
			if (endianness == Endianness::BIG)
				swapEndianness(data);
		}
		return data;
	}

	uint32_t BinaryReader::readAddress() const
	{
		uint32_t data = 0;
		if (file)
		{
			fread(&data, sizeof(uint32_t), 1, file);
			if (endianness == Endianness::BIG)
				swapEndianness(data);

			data += rootNodeAddress;
		}

		return data;
	}

	float BinaryReader::readSingle() const
	{
		float data = 0;
		if (file)
		{
			fread(&data, sizeof(float), 1, file);
			if (endianness == Endianness::BIG)
				swapEndianness(*(uint32_t*)&data);

		}
		return data;
	}

	float BinaryReader::readHalf() const
	{
		uint16_t data = 0;
		float target = 0;
		if (file)
		{
			fread(&data, sizeof(uint16_t), 1, file);
			half_float::half halfValue(data);
			if (endianness == Endianness::BIG)
				swapEndianness(data);

			target = halfValue.operator float();
		}

		return target;
	}

	std::string BinaryReader::readString() const
	{
		char c = 'a';
		std::string data = "";
		if (file)
		{
			while (c && !feof(file))
			{
				fread(&c, sizeof(uint8_t), 1, file);
				if (c)
					data += c;
			}

		}
		return data;
	}

	std::string BinaryReader::readString(size_t length) const
	{
		char c = 'a';
		std::string data = "";
		if (file)
		{
			int count = 0;
			while (c && !feof(file) && count < length)
			{
				fread(&c, sizeof(uint8_t), 1, file);
				if (c)
				{
					data += c;
					++count;
				}
			}
		}

		return data;
	}

	void BinaryReader::readAddressTableBBIN(size_t table_size)
	{
		size_t current_address = rootNodeAddress;
		unsigned char* offset_table = new unsigned char[table_size];
		readSize(offset_table, table_size);

		addressTable.clear();

		for (size_t i = 0; i < table_size; i++)
		{
			size_t low = offset_table[i] & 0x3F;

			if ((offset_table[i] & 0x80) && (offset_table[i] & 0x40))
			{
				i += 3;
				current_address += (low * 0x4000000) + (offset_table[i - 2] * 0x40000) + (offset_table[i - 1] * 0x400) + (offset_table[i] * 0x4);
			}
			else if (offset_table[i] & 0x80)
			{
				i++;
				current_address += (low * 0x400) + (offset_table[i] * 4);
			}
			else if (offset_table[i] & 0x40)
			{
				current_address += 4 * low;
			}

			addressTable.push_back(current_address - rootNodeAddress);
		}
	}

	size_t BinaryReader::fixPadding(size_t multiple)
	{
		size_t extra = 0;
		if (file)
		{
			size_t address = getCurrentAddress();
			extra = multiple - (address % multiple);

			if (extra == multiple)
			{
				extra = 0;
				return 0;
			}
			gotoAddress(address + extra);
		}
		return extra;
	}

	Vector2 BinaryReader::readVector2() const
	{
		Vector2 v2;
		if (file)
		{
			v2.x = readSingle();
			v2.y = readSingle();
		}
		return v2;
	}

	Vector2 BinaryReader::readVector2Half() const
	{
		Vector2 v2;
		if (file)
		{
			v2.x = readHalf();
			v2.y = readHalf();
		}
		return v2;
	}

	Vector3 BinaryReader::readVector3() const
	{
		Vector3 v3;
		if (file)
		{
			v3.x = readSingle();
			v3.y = readSingle();
			v3.z = readSingle();
		}
		return v3;
	}

	Vector3 BinaryReader::readVector3Normal360() const
	{
		uint32_t value = 0;
		Vector3 v3;
		if (file)
		{
			value = readInt32();
			v3.x = ((value & 0x00000400 ? -1 : 0) + (float)((value >> 2) & 0x0FF) / 256.0f);
			v3.y = ((value & 0x00200000 ? -1 : 0) + (float)((value >> 13) & 0x0FF) / 256.0f);
			v3.z = ((value & 0x80000000 ? -1 : 0) + (float)((value >> 23) & 0x0FF) / 256.0f);
		}
		return v3;
	}

	Quaternion BinaryReader::readQuaternion() const
	{
		Quaternion q;
		if (file)
		{
			q.x = readSingle();
			q.y = readSingle();
			q.z = readSingle();
			q.w = readSingle();
		}
		return q;
	}

	Matrix4 BinaryReader::readMatrix4() const
	{
		Matrix4 m;
		if (file)
		{
			for (int x = 0; x < 4; ++x)
				for (int y = 0; y < 4; ++y)
					m[x][y] = readSingle();
		}
		return m;
	}

	Color BinaryReader::readRGBA() const
	{
		Color c;
		if (file)
		{
			c.r = readSingle();
			c.g = readSingle();
			c.b = readSingle();
			c.a = readSingle();
		}
		return c;
	}

	Color BinaryReader::readRGBA8() const
	{
		Color c;
		if (file)
		{
			uint8_t r = readChar();
			uint8_t g = readChar();
			uint8_t b = readChar();
			uint8_t a = readChar();

			c.r = ((float)r) / COLOR_CHAR;
			c.g = ((float)g) / COLOR_CHAR;
			c.b = ((float)b) / COLOR_CHAR;
			c.a = ((float)a) / COLOR_CHAR;
		}
		return c;
	}

	Color BinaryReader::readARGB8() const
	{
		Color c;
		if (file)
		{
			uint8_t a = readChar();
			uint8_t r = readChar();
			uint8_t g = readChar();
			uint8_t b = readChar();

			c.a = ((float)a) / COLOR_CHAR;
			c.r = ((float)r) / COLOR_CHAR;
			c.g = ((float)g) / COLOR_CHAR;
			c.b = ((float)b) / COLOR_CHAR;
		}

		return c;
	}

	Color BinaryReader::readABGR8() const
	{
		Color c;
		if (file)
		{
			uint8_t a = readChar();
			uint8_t b = readChar();
			uint8_t g = readChar();
			uint8_t r = readChar();

			c.a = ((float)a) / COLOR_CHAR;
			c.b = ((float)b) / COLOR_CHAR;
			c.g = ((float)g) / COLOR_CHAR;
			c.r = ((float)r) / COLOR_CHAR;
		}

		return c;
	}

	AABB BinaryReader::readAABB() const
	{
		AABB aabb;
		if (file)
		{
			uint32_t headerAddress = getCurrentAddress();

			aabb.start.x = readSingle();
			aabb.end.x = readSingle();
			aabb.start.y = readSingle();
			aabb.end.y = readSingle();
			aabb.start.z = readSingle();
			aabb.end.z = readSingle();
		}
		return aabb;
	}
}