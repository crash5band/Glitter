#include "AnimationSet.h"

namespace Glitter
{
	AnimationSet::AnimationSet() : start{ 0 }, end{ 0 }, fps{ 30 }, name{ "" }
	{

	}

	float AnimationSet::getStart() const
	{
		return start;
	}

	float AnimationSet::getEnd() const
	{
		return end;
	}

	float AnimationSet::getFPS() const
	{
		return fps;
	}

	std::string AnimationSet::getName() const
	{
		return name;
	}
}