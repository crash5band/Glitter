#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <half.hpp>
#include "Endianness.h"
#include "Constants.h"
#include "MathGens.h"

namespace Glitter
{
	class BinaryReader
	{
	private:
		FILE* file;
		Endianness endianness;
		uint32_t globalOffset;
		uint32_t rootNodeAddress;
		std::vector<uint32_t> addressTable;
		std::string filename;
		std::string filepath;
		int rootNodeType;

	public:
		BinaryReader(const std::string& filename, Endianness endianness);
		~BinaryReader();

		void readSize(void* data, size_t size);
		uint8_t readChar() const;
		uint16_t readInt16() const;
		uint32_t readInt32() const;
		uint32_t readAddress() const;
		float readSingle() const;
		float readHalf() const;
		std::string readString() const;
		std::string readString(size_t length) const;

		Vector2 readVector2() const;
		Vector2 readVector2Half() const;
		Vector3 readVector3() const;
		Vector3 readVector3Normal360() const;
		Quaternion readQuaternion() const;
		Matrix4 readMatrix4() const;
		Color readRGBA() const;
		Color readRGBA8() const;
		Color readARGB8() const;
		Color readABGR8() const;
		AABB readAABB() const;

		size_t fixPadding(size_t multiple);
		size_t getFileSize() const;
		size_t getCurrentAddress() const;
		size_t getRootNodeAddress() const;
		int getRootNodeType() const;
		bool valid() const;
		void close() const;
		void changeEndianness(Endianness en);
		void gotoAddress(size_t address) const;
		void moveAddress(size_t address) const;
		void gotoEnd() const;
		void readAddressTableBBIN(size_t size);
		void readHeader();
		void setGlobalOffset(size_t offset) ;
		void setRootNodeAddress(size_t offset) ;
	};
}

