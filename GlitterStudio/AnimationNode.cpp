#include "AnimationNode.h"
#include "Utilities.h"

AnimationNode::AnimationNode(std::vector<Glitter::Animation>* list) :
	animationList{ list }
{

}

std::vector<Glitter::Animation>* AnimationNode::getAnimationList()
{
	return animationList;
}

Glitter::Animation* AnimationNode::find(Glitter::AnimationType type)
{
	auto ptr = std::find_if(animationList->begin(), animationList->end(),
		[&type](Glitter::Animation& anim) {return anim.getType() == type; });

	return ptr != animationList->end() ? &(*ptr) : nullptr;
}

void AnimationNode::add(Glitter::Animation& anim, size_t pos)
{
	animationList->insert(animationList->begin() + pos, anim);
}

void AnimationNode::remove(size_t pos)
{
	animationList->erase(animationList->begin() + pos);
}

float AnimationNode::calcRandomRange(Glitter::AnimationType type, float randomRange)
{
	// color animations have a random range in one direction only(?)
	float min = ((size_t)type>= 10 && (size_t)type < 14) ? 0 : -randomRange;
	return Utilities::random(min, randomRange);
}

void AnimationNode::buildCache()
{
	for (auto& anim : cache)
		anim.clear();

	for (Glitter::Animation& anim : *animationList)
	{
		size_t type = (size_t)anim.getType();
		size_t keyCount = anim.getKeys().size();
		size_t nextIndex = 0;

		if (!keyCount)
			break;

		Glitter::Key k1 = anim.getKeys()[nextIndex++];
		k1.value += calcRandomRange(anim.getType(), k1.randomRange);

		int frame = 0;
		cache[type].reserve(anim.getEndTime() + 1);

		// fill frames before the first key with default values
		if (frame < k1.time)
		{
			float value = defaultValue;
			size_t type = (size_t)anim.getType();

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
			Glitter::Key k2 = anim.getKeys()[nextIndex];
			k2.value += calcRandomRange(anim.getType(), k2.randomRange);

			for (; frame <= anim.getEndTime(); ++frame)
			{
				if (frame >= k2.time)
				{
					k1 = k2;
					++nextIndex;

					if (nextIndex < keyCount)
					{
						k2 = anim.getKeys()[nextIndex];
						k2.value += calcRandomRange(anim.getType(), k2.randomRange);
					}
				}

				if (k1.interpolationType == Glitter::InterpolationType::Constant)
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

float AnimationNode::tryGetValue(Glitter::AnimationType type, float time, float fallback)
{
	std::vector<float>& values = cache[(size_t)type];
	if (!values.size())
		return fallback;

	Glitter::Animation* anim = find(type);
	if (anim->getRepeatType() == Glitter::RepeatType::Repeat)
		time = (int)anim->getEndTime() == 0 ? 0 : (int)time % (int)anim->getEndTime();

	if (time >= values.size() || time < 0)
		return values[values.size() - 1];

	/*
		interpolate key frame values for smoother animations. Mostly noticeable when using playback
		speeds less than 1.0x.
	*/
	float start = values[(int)time];
	float end = start;
	if ((int)time < values.size() - 1)
		end = values[(int)time + 1];

	float ratio = time - (int)time;
	return Utilities::lerp(start, end, ratio);
}

Glitter::Vector3 AnimationNode::tryGetTranslation(float time)
{
	Glitter::Vector3 pos;
	pos.x = tryGetValue(Glitter::AnimationType::Tx, time);
	pos.y = tryGetValue(Glitter::AnimationType::Ty, time);
	pos.z = tryGetValue(Glitter::AnimationType::Tz, time);

	return pos;
}

Glitter::Vector3 AnimationNode::tryGetRotation(float time)
{
	Glitter::Vector3 rot;
	rot.x = tryGetValue(Glitter::AnimationType::Rx, time);
	rot.y = tryGetValue(Glitter::AnimationType::Ry, time);
	rot.z = tryGetValue(Glitter::AnimationType::Rz, time);

	return rot;
}

Glitter::Vector3 AnimationNode::tryGetScale(float time)
{
	Glitter::Vector3 scale;
	scale.x = tryGetValue(Glitter::AnimationType::Sx, time, 1.0f);
	scale.y = tryGetValue(Glitter::AnimationType::Sy, time, 1.0f);
	scale.z = tryGetValue(Glitter::AnimationType::Sz, time, 1.0f);

	scale *= tryGetValue(Glitter::AnimationType::SAll, time, 1.0f);

	return scale;
}

Glitter::Color AnimationNode::tryGetColor(float time)
{
	Glitter::Color col;
	col.r = tryGetValue(Glitter::AnimationType::ColorR, time, 255) / Glitter::COLOR_CHAR;
	col.g = tryGetValue(Glitter::AnimationType::ColorG, time, 255) / Glitter::COLOR_CHAR;
	col.b = tryGetValue(Glitter::AnimationType::ColorB, time, 255) / Glitter::COLOR_CHAR;
	col.a = tryGetValue(Glitter::AnimationType::ColorA, time, 255) / Glitter::COLOR_CHAR;

	return col;
}
