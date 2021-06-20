#include "Animation.h"
#include "KeyFrame.h"

namespace Glitter
{
	Animation::Animation() : name{ "" }
	{

	}

	void Animation::read(BinaryReader &reader)
	{
		if (!reader.valid())
			return;

		animationsAddress = reader.readAddress();
		unsigned int animsSize = reader.readInt32();
		size_t keyframesAddress = reader.readAddress();
		unsigned int keyframesSize = reader.readInt32();
		size_t stringsAddress = reader.readAddress();
		unsigned int stringsSize = reader.readInt32();

		reader.gotoAddress(keyframesAddress);
		readKeyframes(reader, keyframesSize);

		reader.gotoAddress(stringsAddress);
		readStrings(reader, stringsSize);
	}

	void Animation::readKeyframes(BinaryReader& reader, unsigned int size)
	{
		int count = size / 8;
		for (int i = 0; i < count; ++i)
		{
			Keyframe k;
			k.read(reader);
			keyframesBuffer.push_back(k);
		}
	}

	void Animation::readStrings(BinaryReader& reader, unsigned int size)
	{
		namesBuffer = new char[size];
		reader.readSize(namesBuffer, size);
	}

	std::string Animation::getName() const
	{
		return name;
	}
}