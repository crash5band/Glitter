#pragma once
#include <cstdlib>
#include <ctime>
#include "MathGens.h"

class Utilities
{
public:
	static void initRandom();

	static float random(float min, float max);
	static float randomize(const float f1, const float f2);
	static Glitter::Vector3 randomize(const Glitter::Vector3& v1, const Glitter::Vector3& v2);
	static Glitter::Vector2 randomize(const Glitter::Vector2& v1, const Glitter::Vector2& v2);
	static float interpolate(float f, float t1, float v1, float p1, float t2, float v2, float p2);
	static float interpolate(float f, float t1, float v1, float t2, float v2);
	static const std::string getCurrentDateTime();

	inline static float lerp(const float v1, const float v2, const float bias)
	{
		return v1 + bias * (v2 - v1);
	}

	inline static bool isWithinRange(const float val, const float limit1, const float limit2)
	{
		return val >= limit1 && val <= limit2;
	}

	inline static float toRadians(const float degrees)
	{
		return degrees * (Glitter::PI / 180.0f);
	}

	inline static float toDegrees(const float radians)
	{
		return radians * (180.0f / Glitter::PI);
	}
};
