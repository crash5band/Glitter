#include "UVAnimation.h"
#include "File.h"

namespace Glitter
{
	UVAnimation::UVAnimation(const std::string& filename)
	{
		BinaryReader reader(filename, Endianness::BIG);

		name = File::getFileName(filename);

		if (reader.valid())
		{
			reader.readHeader();
			read(reader);
			reader.close();
		}
	}

	void UVAnimation::read(BinaryReader& reader)
	{
		size_t animationsAddress = reader.readAddress();
		unsigned int animationsSize = reader.readInt32();

		size_t keyframesAddress = reader.readAddress();
		unsigned int keyframesSize = reader.readInt32();

		size_t stringsAddress = reader.readAddress();
		unsigned int stringsSize = reader.readInt32();

		reader.gotoAddress(keyframesAddress);
		readKeyframes(reader, keyframesSize);

		reader.gotoAddress(stringsAddress);
		readStrings(reader, stringsSize);

		reader.gotoAddress(animationsAddress);
		readAnimations(reader);
	}

	void UVAnimation::readAnimations(BinaryReader& reader)
	{
		unsigned int materialNameOffset = reader.readInt32();
		unsigned int textureUnitOffset = reader.readInt32();
		unsigned int animationCount = reader.readInt32();
		size_t animationTableAddress = reader.readAddress();

		material = std::string(namesBuffer + materialNameOffset);
		texset = std::string(namesBuffer + textureUnitOffset);

		reader.gotoAddress(animationTableAddress);
		for (int a = 0; a < animationCount; ++a)
		{
			UVAnimationSet set;
			set.read(reader, namesBuffer, keyframesBuffer);
			animationSets.push_back(set);
		}
	}

	UVAnimationSet &UVAnimation::getAnimationSet(const std::string& name)
	{
		for (int i = 0; i < animationSets.size(); ++i)
		{
			if (name.size())
			{
				if (animationSets[i].getName() == name)
					return animationSets[i];
			}
			else
			{
				return animationSets[i];
			}
		}

		UVAnimationSet set;
		return set;
	}

	std::string UVAnimation::getMaterial() const
	{
		return material;
	}

	std::string UVAnimation::getTexset() const
	{
		return texset;
	}
}