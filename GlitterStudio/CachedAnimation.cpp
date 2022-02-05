#include "CachedAnimation.h"
#include "Utilities.h"

namespace Glitter
{
	namespace Editor
	{
		CachedAnimation::CachedAnimation(std::shared_ptr<EditorAnimationSet> set)
		{
			buildCache(set);
		}

		CachedAnimation::CachedAnimation()
		{

		}

		void CachedAnimation::buildCache(std::shared_ptr<EditorAnimationSet> set)
		{
			for (auto& c : cache)
				c.clear();

			for (EditorAnimation& anim : set->animations)
			{
				size_t type = (size_t)anim.type;
				size_t keyCount = anim.keys.size();
				size_t nextIndex = 0;

				if (!keyCount)
					continue;

				EditorKeyframe k1 = anim.keys[nextIndex++];
				k1.value += anim.calcRandomRange(k1.randomRange);

				int frame = 0;
				cache[type].reserve(anim.getEnd() + 1);
				repeatTable[type] = anim.repeat == RepeatType::Repeat;

				// fill frames before the first key with default values
				if (frame < k1.time)
				{
					float value = defaultValue;
					size_t type = (size_t)anim.type;

					if (type > 5 && type < 10)
					{
						value = defaultScale;
					}
					else if (type > 9 && type < 14)
					{
						value = defaultColor;
					}

					for (; frame < k1.time; ++frame)
						cache[type].push_back(value);
				}

				if (keyCount > 1)
				{
					EditorKeyframe k2 = anim.keys[nextIndex];
					k2.value += anim.calcRandomRange(k2.randomRange);

					for (; frame <= anim.getEnd(); ++frame)
					{
						if (frame >= k2.time)
						{
							k1 = k2;
							++nextIndex;

							if (nextIndex < keyCount)
							{
								k2 = anim.keys[nextIndex];
								k2.value += anim.calcRandomRange(k2.randomRange);
							}
						}

						if (k1.interpolation == Glitter::InterpolationType::Constant)
						{
							for (; frame < k2.time; ++frame)
								cache[type].push_back(k1.value);
						}
						else
						{
							while (frame < k2.time)
								cache[type].push_back(anim.interpolate(frame++, k1, k2));

							cache[type].push_back(k2.value);
						}
					}
				}
				else
				{
					cache[type].push_back(k1.value);
				}
			}
		}

		float CachedAnimation::getValue(Glitter::AnimationType type, float time, float fallback)
		{
			std::vector<float>& values = cache[(size_t)type];
			if (!values.size())
				return fallback;

			if (repeatTable[(size_t)type])
				time = fmodf(time, values.size());

			if (time >= values.size() || time < 0)
				return values[values.size() - 1];

			//interpolate key frame values for smoother animations. Mostly noticeable when using playback
			//speeds less than 1.0x.

			float start = values[time];
			float end = start;
			if (time < values.size() - 1)
				end = values[time + 1];

			float ratio = time - (int)time;
			return Utilities::lerp(start, end, ratio);
		}

		Glitter::Vector3 CachedAnimation::tryGetTranslation(float time)
		{
			Glitter::Vector3 pos;
			pos.x = getValue(Glitter::AnimationType::Tx, time);
			pos.y = getValue(Glitter::AnimationType::Ty, time);
			pos.z = getValue(Glitter::AnimationType::Tz, time);

			return pos;
		}

		Glitter::Vector3 CachedAnimation::tryGetRotation(float time)
		{
			Glitter::Vector3 rot;
			rot.x = getValue(Glitter::AnimationType::Rx, time);
			rot.y = getValue(Glitter::AnimationType::Ry, time);
			rot.z = getValue(Glitter::AnimationType::Rz, time);

			return rot;
		}

		Glitter::Vector3 CachedAnimation::tryGetScale(float time)
		{
			Glitter::Vector3 scale;
			scale.x = getValue(Glitter::AnimationType::Sx, time, 1.0f);
			scale.y = getValue(Glitter::AnimationType::Sy, time, 1.0f);
			scale.z = getValue(Glitter::AnimationType::Sz, time, 1.0f);

			scale *= getValue(Glitter::AnimationType::SAll, time, 1.0f);

			return scale;
		}

		Glitter::Color CachedAnimation::tryGetColor(float time)
		{
			Glitter::Color col;
			col.r = getValue(Glitter::AnimationType::ColorR, time, 255) / Glitter::COLOR_CHAR;
			col.g = getValue(Glitter::AnimationType::ColorG, time, 255) / Glitter::COLOR_CHAR;
			col.b = getValue(Glitter::AnimationType::ColorB, time, 255) / Glitter::COLOR_CHAR;
			col.a = getValue(Glitter::AnimationType::ColorA, time, 255) / Glitter::COLOR_CHAR;

			return col;
		}
	}
}