#pragma once
#include "KeyFrame.h"
#include <vector>

namespace Glitter
{
	class KeyframeSet
	{
	public:
		unsigned int flag;
		std::vector<Keyframe> keyframes;

		KeyframeSet();

		void read(BinaryReader& reader, const std::vector<Keyframe> &buffer);
		float getValue(float currentFrame);
	};
}