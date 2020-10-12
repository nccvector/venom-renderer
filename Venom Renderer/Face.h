#pragma once

#include <glm.hpp>

#include "Material.h"

struct Face
{
	int index;
	glm::vec3 vertices[3];
	glm::vec2 uvs[3];
	glm::vec3 normals[3];

	Material* mat;
};