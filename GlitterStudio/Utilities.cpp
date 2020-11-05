#include "Utilities.h"
#include "GlitterEnums.h"
#include "Camera.h"
#include "Transform.h"

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
