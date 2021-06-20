#pragma once
#include <string>
#include <vector>
#include "BinaryReader.h"

namespace Glitter
{
	struct Keyframe;

	class Animation
	{
	protected:
		std::string name;
		char* namesBuffer;
		std::vector<Keyframe> keyframesBuffer;
		size_t animationsAddress;

	public:
		Animation();

		void read(BinaryReader &reader);
		void readKeyframes(BinaryReader &reader, unsigned int size);
		void readStrings(BinaryReader& reader, unsigned int size);
		std::string getName() const;
	};
}