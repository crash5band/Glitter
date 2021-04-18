#pragma once
#include <stdio.h>
#include <string>
#include <vector>
#include <half.hpp>
#include "Endianness.h"
#include "Constants.h"
#include "MathGens.h"

namespace Glitter
{
	class BinaryWriter
	{
	private:
		FILE* file;
		Endianness endianness;
		uint32_t globalOffset;
		uint32_t rootNodeAddress;
		std::vector<uint32_t> addressTable;
		std::string filename;
		std::string filepath;
		int version;

	public:
		BinaryWriter(const std::string& filepath, Endianness en);
		~BinaryWriter();

		void writeSize(void* data, size_t size);
		void writeChar(const uint8_t &data);
		void writeInt16(const uint16_t &data);
		void writeInt32(const uint32_t &data);
		void writeAddress(const uint32_t &data, bool addToTable = true);
		void writeSingle(const float &data);
		void writeFloat8(const float &data);
		void writeString(const char *data, bool nullTerminated = true);
		void writeNull(size_t length);

		void write(const Vector2& val);
		void write(const Vector3& val);
		void write(const Quaternion& val);
		void write(const Matrix4& val);
		void write(const Color& val, bool argb);
		void write(const AABB& val);

		size_t fixPadding(size_t multiple = 4);
		size_t getFileSize() const;
		size_t getCurrentAddress() const;
		size_t getRootNodeAddress() const;
		int getVersion() const;
		bool valid() const;
		void close() const;
		void gotoAddress(size_t address) const;
		void moveAddress(size_t address) const;
		void gotoEnd() const;
		void setRootNodeAddress(size_t address);
		void setGlobalOffset(size_t offset);
		void writeAddressTableBBIN(size_t offset);
		void prepareHeader(int rootType, int rootOffset = fileHeaderDefaultOffset);
		void writeHeader(bool extraFooter);
		void sortAddressTable();
		void changeEndianness(Endianness en);
	};
}