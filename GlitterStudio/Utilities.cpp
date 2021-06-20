#include "Utilities.h"

void Utilities::initRandom()
{
	std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

float Utilities::random(float min, float max)
{
	if (min == max)
		return min;

	return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

float Utilities::randomize(const float f1, const float f2)
{
	return random(f1 - f2, f1 + f2);
}

Glitter::Vector3 Utilities::randomize(const Glitter::Vector3& v1, const Glitter::Vector3& v2)
{
	float x = random(v1.x - v2.x, v1.x + v2.x);
	float y = random(v1.y - v2.y, v1.y + v2.y);
	float z = random(v1.z - v2.z, v1.z + v2.z);

	return Glitter::Vector3(x, y, z);
}

Glitter::Vector2 Utilities::randomize(const Glitter::Vector2& v1, const Glitter::Vector2& v2)
{
	float x = random(v1.x - v2.x, v1.x + v2.x);
	float y = random(v1.y - v2.y, v1.y + v2.y);

	return Glitter::Vector2(x, y);
}

float Utilities::interpolate(float f, float t1, float v1, float p1, float t2, float v2, float p2)
{
	float bias = (f - t1) / (t2 - t1);

	float factor = 1 - bias;
	return ((factor - 1.0f) * 2.0f - 1.0f) * (factor * factor) * (v2 - v1) +
		((factor - 1.0f) * p2 + factor * p1) * (factor - 1.0f) * (f - t2) + v2;
}

float Utilities::interpolate(float f, float t1, float v1, float t2, float v2)
{
	return lerp(v1, v2, f / (t1 + t2));
}