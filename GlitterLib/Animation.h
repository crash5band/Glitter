#pragma once
#include <vector>
#include "GlitterEnums.h"
#include "tinyxml2.h"

namespace Glitter
{
	struct Key
	{
		float time;
		float value;
		InterpolationType interpolationType;
		float inParam;
		float outParam;
		float randomRange;

		inline bool operator==(const Key& k)
		{
			return time == k.time && value == k.value && interpolationType == k.interpolationType
				&& inParam == k.inParam && outParam == k.outParam && randomRange == k.randomRange;
		}

		inline bool operator!=(const Key& k)
		{
			return !(*this == k);
		}
	};

	class Animation
	{
	private:
		AnimationType type;
		float startTime;
		float endTime;
		RepeatType repeatType;
		unsigned int randomFlags;
		std::vector<Key> keys;

	public:
		Animation();
		Animation(AnimationType type, float start);

		AnimationType getType() const;
		float getStartTime() const;
		float getEndTime() const;
		RepeatType getRepeatType() const;
		unsigned int getRandomFlags() const;
		std::vector<Key> &getKeys();

		void setAnimationType(AnimationType type);
		void setStartTime(float time);
		void setEndTime(float time);
		void setRepeatType(RepeatType type);
		void setRandomFlags(unsigned int flags);
		void verifyKeyOrder(unsigned int keyIndex);
		size_t insertKey(Key key);
		void addKey(Key key);
		void removeKey(unsigned int index);
		float interpolate(float time, const Key& k1, const Key& k2);

		void read(tinyxml2::XMLElement* element);
		void write(tinyxml2::XMLElement* element);

	};
}