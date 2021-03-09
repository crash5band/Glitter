#include "BinaryWriter.h"
#include "File.h"

namespace Glitter
{
	BinaryWriter::BinaryWriter(const std::string& path, Endianness en)
	{
		file = fopen(path.c_str(), "wb");
		if (file)
		{
			endianness = en;
			filename = File::getFileName(path);
			filepath = File::getFilePath(path);
			globalOffset = 0;
			rootNodeAddress = 0;
			rootNodeType = 0;
		}
	}

	BinaryWriter::~BinaryWriter()
	{
		close();
	}

	bool BinaryWriter::valid() const
	{
		return file != nullptr;
	}

	void BinaryWriter::close() const
	{
		if (file)
			fclose(file);
	}

	size_t BinaryWriter::getFileSize() const
	{
		if (file)
		{
			size_t currentAddress = ftell(file);
			fseek(file, 0, SEEK_END);
			size_t size = ftell(file);
			fseek(file, currentAddress, SEEK_SET);
			return size;
		}
		else
		{
			return 0;
		}
	}

	void BinaryWriter::gotoAddress(size_t address) const
	{
		if (file)
			fseek(file, address + globalOffset, SEEK_SET);
	}

	void BinaryWriter::moveAddress(size_t address) const
	{
		if (file)
			fseek(file, address, SEEK_CUR);
	}

	void BinaryWriter::gotoEnd() const
	{
		if (file)
			fseek(file, 0, SEEK_END);
	}

	size_t BinaryWriter::getCurrentAddress() const
	{
		if (file)
			return ftell(file) - globalOffset;
	}

	size_t BinaryWriter::getRootNodeAddress() const
	{
		return rootNodeAddress;
	}

	void BinaryWriter::setRootNodeAddress(size_t address)
	{
		rootNodeAddress = address;
	}

	void BinaryWriter::setGlobalOffset(size_t offset)
	{
		globalOffset = offset;
	}

	void BinaryWriter::writeSize(void* data, size_t size)
	{
		if (file)
			fwrite(data, size, 1, file);
	}

	void BinaryWriter::writeChar(const uint8_t &data)
	{
		if (file)
			fwrite(&data, sizeof(uint8_t), 1, file);
	}

	void BinaryWriter::writeInt16(const uint16_t &data)
	{
		if (file)
		{
			uint16_t val = data;
			if (endianness == Endianness::BIG)
				swapEndianness(val);

			fwrite(&val, sizeof(uint16_t), 1, file);
		}
	}

	void BinaryWriter::writeInt32(const uint32_t &data)
	{
		if (file)
		{
			uint32_t val = data;
			if (endianness == Endianness::BIG)
				swapEndianness(val);

			fwrite(&val, sizeof(uint32_t), 1, file);
		}
	}

	void BinaryWriter::writeAddress(const uint32_t &data, bool addToTable)
	{
		if (file)
		{
			uint32_t val = data;
			if (addToTable)
				addressTable.push_back(getCurrentAddress() - rootNodeAddress);

			val = val - rootNodeAddress;
			if (endianness == Endianness::BIG)
				swapEndianness(val);

			fwrite(&val, sizeof(uint32_t), 1, file);
		}
	}

	void BinaryWriter::writeSingle(const float &data)
	{
		if (file)
		{
			float val = data;
			if (endianness == Endianness::BIG)
				swapEndianness(*((uint32_t*)&val));

			fwrite(&val, sizeof(float), 1, file);
		}
	}

	void BinaryWriter::writeFloat8(const float &data)
	{
		if (file)
		{
			uint8_t val = (int)(data * 256.0f);
			writeChar(val);
		}
	}

	void BinaryWriter::writeString(const char* data, bool nullTerminated)
	{
		if (file)
		{
			size_t length = strlen(data);
			if (length)
			{
				fwrite(data, sizeof(char), length, file);
				if (nullTerminated)
					writeNull(1);
			}
			else
			{
				writeNull(1);
			}
		}
	}

	void BinaryWriter::writeNull(size_t length)
	{
		if (file)
		{
			uint8_t zero = 0;
			for (int i = 0; i < length; ++i)
			{
				fwrite(&zero, sizeof(uint8_t), 1, file);
			}
		}
	}

	size_t BinaryWriter::fixPadding(size_t multiple)
	{
		size_t address = getCurrentAddress();
		size_t extra = multiple - (address % multiple);

		if (extra == multiple)
		{
			extra = 0;
			return 0;
		}

		size_t zero = 0;
		for (size_t c = 0; c < extra; c++)
			fwrite(&zero, sizeof(char), 1, file);

		return extra;
	}

	void BinaryWriter::writeAddressTableBBIN(size_t negative_offset)
	{
		size_t current_address = negative_offset;
		for (std::vector<uint32_t>::iterator it = addressTable.begin(); it != addressTable.end(); it++)
		{
			size_t difference = (*it) - current_address;

			if (difference > 0xFFFC)
			{
				unsigned int offset_int = 0xC0000000 | (difference >> 2);
				writeInt32(offset_int);
			}
			else if (difference > 0xFC)
			{
				unsigned short offset_short = 0x8000 | (difference >> 2);
				writeInt16(offset_short);
			}
			else
			{
				char offset_byte = 0x40 | (difference >> 2);
				writeSize(&offset_byte, 1);
			}

			current_address += difference;
		}
	}

	void BinaryWriter::prepareHeader(int rootType, int rootOffset)
	{
		if (file)
		{
			rootNodeType = rootType;
			rootNodeAddress = rootOffset;

			writeNull(fileHeaderDefaultOffset);
		}
	}

	void BinaryWriter::writeHeader(bool extraFooter)
	{
		sortAddressTable();
		uint32_t finalTableAddress = getFileSize() - rootNodeAddress;
		uint32_t finalTableAddressAbs = getFileSize();
		uint32_t finalTableSize = addressTable.size();

		writeInt32(finalTableSize);
		for (int i = 0; i < addressTable.size(); ++i)
		{
			writeInt32(addressTable[i]);
		}

		if (extraFooter)
			writeNull(4);

		uint32_t size = getFileSize();
		uint32_t sizeFooter = extraFooter ? size - 4 : 0;

		fseek(file, 0 + globalOffset, SEEK_SET);
		writeInt32(size);
		writeInt32(((uint32_t)rootNodeType));
		writeInt32(finalTableAddress);
		writeInt32(rootNodeAddress);
		writeInt32(finalTableAddressAbs);
		writeInt32(sizeFooter);
	}

	void BinaryWriter::sortAddressTable()
	{
		std::vector<uint32_t> new_address_table;

		for (std::vector<uint32_t>::iterator it = addressTable.begin(); it != addressTable.end(); it++)
		{
			bool found = false;

			for (std::vector<uint32_t>::iterator it2 = new_address_table.begin(); it2 != new_address_table.end(); it2++)
			{
				if ((*it) <= (*it2)) {
					new_address_table.insert(it2, (*it));
					found = true;
					break;
				}
			}

			if (!found) new_address_table.push_back(*it);
		}


		addressTable = new_address_table;
	}

	void BinaryWriter::write(const Vector2& val)
	{
		if (!file)
			return;

		writeSingle(val.x);
		writeSingle(val.y);
	}

	void BinaryWriter::write(const Vector3& val)
	{
		if (!file)
			return;

		writeSingle(val.x);
		writeSingle(val.y);
		writeSingle(val.z);
	}

	void BinaryWriter::write(const Quaternion& val)
	{
		if (!file)
			return;

		writeSingle(val.x);
		writeSingle(val.y);
		writeSingle(val.z);
		writeSingle(val.w);
	}

	void BinaryWriter::write(const Matrix4& val)
	{
		if (!file)
			return;

		for (int x = 0; x < 4; ++x)
		{
			for (int y = 0; y < 4; ++y)
			{
				writeSingle(val[x][y]);
			}
		}
	}

	void BinaryWriter::write(const Color& val, bool argb)
	{
		if (!file)
			return;

		if (argb)
		{
			writeChar(val.a);
			writeChar(val.r);
			writeChar(val.g);
			writeChar(val.b);
		}
		else
		{
			writeSingle(val.r);
			writeSingle(val.g);
			writeSingle(val.b);
			writeSingle(val.a);
		}
	}
	
	void BinaryWriter::write(const AABB& val)
	{
		if (!file)
			return;

		uint32_t headerAddress = getCurrentAddress();

		writeSingle(val.start.x);
		writeSingle(val.end.x);
		writeSingle(val.start.y);
		writeSingle(val.end.y);
		writeSingle(val.start.z);
		writeSingle(val.end.z);
	}
}