#pragma once
#include <glm.hpp>

struct Ray
{
	glm::vec4 origin;
	glm::vec4 direction;
	float max_distance;
};