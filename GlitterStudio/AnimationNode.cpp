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

void AnimationNode::buildCache()
{
	for (auto& anim : cache)
		anim.clear();

	for (size_t a = 0; a < Glitter::animationTypeTableSize; ++a)
	{
		for (size_t b = 0; b < animationList->size(); ++b)
		{
			if ((Glitter::AnimationType)a == animationList->at(b).getType())
			{
				Glitter::Animation& anim = animationList->at(b);
				size_t keyCount = anim.getKeys().size();
				size_t index = 0;

				if (!keyCount)
					break;

				Glitter::Key k1 = anim.getKeys()[index++];

				// color and integer animations have a random range in one direction only
				float min = ((size_t)anim.getType() >= 10 && (size_t)anim.getType() < 14) ? 0 : -k1.randomRange;
				k1.value += Utilities::random(min, k1.randomRange);

				if (keyCount > 1)
				{
					cache[a].reserve(anim.getEndTime() + 1);
					Glitter::Key k2{ -1.0f };

					int frame = 0;

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
							cache[a].push_back(value);
					}

					for (; frame <= anim.getEndTime(); ++frame)
					{
						if (index < keyCount)
						{
							// color animations have their random range in one direction only.
							float min = ((size_t)anim.getType() >= 10 && (size_t)anim.getType() < 14) ? 0 : -k2.randomRange;

							float value = k2.time != anim.getKeys()[index].time ? Utilities::random(min, k2.randomRange) : 0.0f;
							k2 = anim.getKeys()[index];
							k2.value += value;
						}

						if (k1.interpolationType == Glitter::InterpolationType::Constant)
						{
							cache[a].push_back(k1.value);
						}
						else
						{
							cache[a].push_back(anim.interpolate(frame, k1, k2));
						}

						if (frame >= k2.time)
						{
							k1 = k2;
							index++;
						}
					}

					break;
				}
				else
				{
					cache[a].push_back(k1.value);
				}
			}
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
		time = (int)time % (int)anim->getEndTime();

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
