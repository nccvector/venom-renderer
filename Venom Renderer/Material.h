#pragma once
#include <glm.hpp>

class Material
{
public:
	glm::vec3 color;
	float roughness;
	float specular_weight;
	float base_specular;
	bool metal;

	Material()
	{
		this->color = glm::vec3(0.f, 0.f, 0.f);
		this->specular_weight = 0.5f;
		this->roughness = 0.5f;
		this->base_specular = 1.f;
		metal = false;
	}
};