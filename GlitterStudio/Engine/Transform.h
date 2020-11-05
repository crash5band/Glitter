#pragma once
#include "MathGens.h"

struct Transform
{
	Glitter::Vector3 position;
	Glitter::Vector3 rotation;
	Glitter::Vector3 scale;

	Transform()
	{
		scale	 = Glitter::Vector3(1.0f, 1.0f, 1.0f);
	}
};