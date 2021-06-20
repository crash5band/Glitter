#pragma once
#include "KeyFrameSet.h"
#include <string>
#include <vector>

namespace Glitter
{
	class AnimationSet
	{
	protected:
		std::vector<KeyframeSet> keyframeSets;
		float start;
		float end;
		float fps;
		std::string name;

	public:
		AnimationSet();

		float getStart() const;
		float getEnd() const;
		float getFPS() const;
		std::string getName() const;
	};
}