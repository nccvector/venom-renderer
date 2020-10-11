#pragma once

#include <glm.hpp>

struct SurfaceProperties
{
	// Logical properties
	bool hit;

	// Geometric properties
	float hitDistance;
	glm::vec3 hitPoint;
	glm::vec3 normal;
	glm::vec2 texcoord;

	// Material properties
	glm::vec3 color;
	float roughness;
	float baseSpecular;
	float specularWeight;
};