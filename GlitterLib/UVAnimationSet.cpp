#include "UVAnimationSet.h"

namespace Glitter
{
	void UVAnimationSet::read(BinaryReader& reader, char* names, std::vector<Keyframe> keyframes)
	{
		unsigned int animationNameOffset = reader.readInt32();

		fps = reader.readSingle();
		start = reader.readSingle();
		end = reader.readSingle();
		name = std::string(names + animationNameOffset);

		unsigned int keyframeSetsCount = reader.readInt32();

		for (int i = 0; i < keyframeSetsCount; ++i)
		{
			KeyframeSet set;
			set.read(reader, keyframes);
			keyframeSets.push_back(set);
		}
	}
}