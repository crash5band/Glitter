#include "EditorAnimation.h"
#include "Utilities.h"

namespace Glitter
{
	namespace Editor
	{
		EditorAnimation::EditorAnimation(Glitter::GlitterAnimation animation)
		{
			start = animation.getStartTime();
			end = animation.getEndTime();
			fps = 60;
			randomFlags = animation.getRandomFlags();
			repeat = animation.getRepeatType();
			type = animation.getType();
			name = glitterEnumToString(animationTypeTable, animationTypeTableSize, (int)animation.getType());

			std::vector<GlitterKey> gKeys = animation.getKeys();
			for (const GlitterKey& k : gKeys)
				keys.push_back(EditorKeyframe(k));
		}

		EditorAnimation::EditorAnimation(float st, float ed, float f)
		{
			start = st;
			end = ed;
			fps = f;
			randomFlags = 0;
			repeat = Glitter::RepeatType::Constant;
			type = Glitter::AnimationType::ColorA;
			name = "ColorA";
		}

		float EditorAnimation::getStart() const
		{
			return start;
		}

		float EditorAnimation::getEnd() const
		{
			return end;
		}

		float EditorAnimation::getFPS() const
		{
			return fps;
		}

		unsigned int EditorAnimation::getFlags() const
		{
			return randomFlags;
		}

		void EditorAnimation::setStart(float t)
		{
			if (t <= end && t >= 0.0f)
			{
				if (keys.size() && t > keys[0].time)
					t = keys[0].time;

				start = t;
			}
		}

		void EditorAnimation::setEnd(float t)
		{
			if (t >= start)
			{
				if (keys.size() && t < keys[keys.size() - 1].time)
					t = keys[keys.size() - 1].time;

				end = t;
			}
		}

		void EditorAnimation::setFPS(float f)
		{
			fps = f;
		}

		void EditorAnimation::setFlags(unsigned int f)
		{
			randomFlags = f;
		}

		float EditorAnimation::interpolate(const float time, const EditorKeyframe& k1, const EditorKeyframe& k2)
		{
			float bias = (time - k1.time) / (k2.time - k1.time);

			if (k1.interpolation == Glitter::InterpolationType::Hermite)
			{
				float factor = 1 - bias;
				return ((factor - 1.0f) * 2.0f - 1.0f) * (factor * factor) * (k2.value - k1.value) +
					((factor - 1.0f) * k2.inParam + factor * k1.outParam) *
					(factor - 1.0f) * (time - k2.time) + k2.value;
			}

			return k1.value + bias * (k2.value - k1.value);
		}

		float EditorAnimation::calcRandomRange(float range)
		{
			// color animations have a random range in one direction only(?)
			float min = ((size_t)type >= 10 && (size_t)type < 14) ? 0 : -range;
			return Utilities::random(min, range);
		}

		float EditorAnimation::verifyKeyOrder(unsigned int keyIndex, float newTime)
		{
			if (keys.size() < 2)
				return newTime;

			if (keyIndex > 0)
			{
				if (newTime <= keys[keyIndex - 1].time)
					newTime = keys[keyIndex - 1].time + 1;

				if (keyIndex < keys.size() - 1)
				{
					if (newTime >= keys[keyIndex + 1].time)
						newTime = keys[keyIndex + 1].time - 1;
				}
			}
			else
			{
				if (keys.size() > 1)
				{
					if (newTime >= keys[keyIndex + 1].time)
						newTime = keys[keyIndex + 1].time - 1;
				}
			}

			return newTime;
		}

		unsigned int EditorAnimation::insertKey(const EditorKeyframe& key)
		{
			for (size_t index = 0; index < keys.size(); ++index)
			{
				if (keys[index].time > key.time)
				{
					keys.insert(keys.begin() + index, key);
					return index;
				}
			}

			keys.emplace_back(key);
			return keys.size() - 1;
		}

		void EditorAnimation::removeKey(unsigned int index)
		{
			if (index < keys.size())
				keys.erase(keys.begin() + index);
		}

		GlitterAnimation EditorAnimation::toGlitterAnimation() const
		{
			GlitterAnimation animation;
			animation.setAnimationType(type);
			animation.setStartTime(start);
			animation.setEndTime(end);
			animation.setRepeatType(repeat);
			animation.setRandomFlags(randomFlags);

			for (const EditorKeyframe& key : keys)
				animation.addKey(GlitterKey{ key.time, key.value, key.interpolation, key.inParam, key.outParam, key.randomRange });

			return animation;
		}
	}
}