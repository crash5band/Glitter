#pragma once
#include <vector>
#include "GlitterEnums.h"
#include "tinyxml2.h"

namespace Glitter
{
	struct GlitterKey
	{
		float time;
		float value;
		InterpolationType interpolationType;
		float inParam;
		float outParam;
		float randomRange;

		inline bool operator==(const GlitterKey& k)
		{
			return time == k.time && value == k.value && interpolationType == k.interpolationType
				&& inParam == k.inParam && outParam == k.outParam && randomRange == k.randomRange;
		}

		inline bool operator!=(const GlitterKey& k)
		{
			return !(*this == k);
		}
	};

	class GlitterAnimation
	{
	private:
		AnimationType type;
		float startTime;
		float endTime;
		RepeatType repeatType;
		unsigned int randomFlags;
		std::vector<GlitterKey> keys;

	public:
		GlitterAnimation();
		GlitterAnimation(AnimationType type, float start);

		AnimationType getType() const;
		float getStartTime() const;
		float getEndTime() const;
		RepeatType getRepeatType() const;
		unsigned int getRandomFlags() const;
		std::vector<GlitterKey> &getKeys();

		void setAnimationType(AnimationType type);
		void setStartTime(float time);
		void setEndTime(float time);
		void setRepeatType(RepeatType type);
		void setRandomFlags(unsigned int flags);
		void verifyKeyOrder(unsigned int keyIndex);
		size_t insertKey(GlitterKey key);
		void addKey(GlitterKey key);
		void removeKey(unsigned int index);
		float interpolate(float time, const GlitterKey& k1, const GlitterKey& k2);

		void read(tinyxml2::XMLElement* element);
		void write(tinyxml2::XMLElement* element);

		inline bool operator==(const GlitterAnimation& anim)
		{
			return startTime == anim.startTime && endTime == anim.endTime && repeatType == anim.repeatType
				&& randomFlags == anim.randomFlags;
		}

		inline bool operator!=(const GlitterAnimation& anim)
		{
			return !(*this == anim);
		}

	};
}