#pragma once
#include <string>
#include "MathGens.h"
#include "BinaryReader.h"
#include "BinaryWriter.h"

namespace Glitter
{
	class Bone
	{
	private:
		Matrix4 matrix;
		std::string name;
		unsigned int parentIndex;

	public:
		Bone();
		void readDescription(BinaryReader* reader);
		void writeDescription(BinaryWriter* writer);
		void readMatrix(BinaryReader* reader);
		void writeMatrix(BinaryWriter* writer);
		std::string getName();
		unsigned int getParentIndex();
		Matrix4 getMatrix();
	};
}

