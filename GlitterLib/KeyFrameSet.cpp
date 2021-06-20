#include "KeyFrameSet.h"

namespace Glitter
{
	KeyframeSet::KeyframeSet() : flag{ 0 }
	{

	}

	void KeyframeSet::read(BinaryReader& reader, const std::vector<Keyframe> &buffer)
	{
		flag = reader.readInt32();

		unsigned int count = 0;
		unsigned int index = 0;
		count = reader.readInt32();
		index = reader.readInt32();
		
		for (size_t i = index; i < count + index; ++i)
			keyframes.push_back(buffer[i]);
		
	}

	/// <summary>
	///	INCOMPLETE
	/// </summary>
	/// <param name="currentFrame"></param>
	/// <returns></returns>
	float KeyframeSet::getValue(float currentFrame)
	{
		Keyframe prev;
		Keyframe next;

		float factor = 0;

		// find current frame in list
		for (size_t i = 0; i < keyframes.size(); ++i)
		{
			prev = next;
			next = keyframes[i];

			if (next.frame > currentFrame)
				break;
		}

		float offset = currentFrame - prev.frame;
		float gap = next.frame - prev.frame;
		factor = offset / gap;

		return prev.value + ((next.value - prev.value) * factor);
	}
}