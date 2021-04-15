#pragma once
#include "MathGens.h"

enum class LightType
{
	Direct,
	Point,
	Spotlight
};

struct Light
{
	Glitter::Vector3 position;
	Glitter::Color color;
	LightType type;
	float ambient;
	float specular;
	float specularPower;
	float innerRadius;
	float outerRadius;
	float pointRadius;

	Light(LightType t)
	{
		position = Glitter::Vector3(0.4f, 1.0f, -0.7f);
		color = Glitter::Color(1.0f, 1.0f, 1.0f);

		type = t;
		ambient = 0.8f;
		specular = 0.5f;
		specularPower = 5.0f;
		innerRadius = 0.5f;
		outerRadius = 0.75f;
		pointRadius = 0.5f;
	}
};