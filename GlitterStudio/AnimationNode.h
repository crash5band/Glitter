#pragma once
#include "Animation.h"
#include "MathGens.h"
#include <unordered_map>

constexpr float defaultValue = 0.0f;
constexpr float defaultScale = 1.0f;
constexpr float defaultColor = 255;

class AnimationNode
{
private:
	std::vector<Glitter::Animation> *animationList;
	std::array<std::vector<float>, Glitter::animationTypeTableSize> cache;
	std::unordered_map<Glitter::AnimationType, std::vector<float>> animValues;

public:
	AnimationNode(std::vector<Glitter::Animation>* list);

	std::vector<Glitter::Animation>* getAnimationList();
	Glitter::Animation* find(Glitter::AnimationType type);
	void buildCache();
	void add(Glitter::Animation &anim, size_t pos);
	void remove(size_t pos);

	float tryGetValue(Glitter::AnimationType type, float time, float fallback = 0.0f);
	Glitter::Vector3 tryGetTranslation(float time);
	Glitter::Vector3 tryGetRotation(float time);
	Glitter::Vector3 tryGetScale(float time);
	Glitter::Color tryGetColor(float time);
};
