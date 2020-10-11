#pragma once

#include <glm.hpp>

#include "Voxel.h"

class Face
{
public:
	int index;
	glm::vec3 vertices[3];
	glm::vec2 uvs[3];
	glm::vec3 normals[3];

	Face()
	{

	}
};