#pragma once
#include "glm/glm.hpp"

struct Light
{
	glm::vec3 direction;
	glm::vec4 color;
	float ambient;
	float diffuse;
	float specular;

	Light()
	{
		direction = glm::vec3(0.0, 1.0, 0.0);
		color = glm::vec4(1.0);
		ambient = 0.5f;
		diffuse = 1.0f;
		specular = 0.2f;
	}
};