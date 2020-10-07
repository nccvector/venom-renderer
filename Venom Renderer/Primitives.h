#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "Ray.h"
#include "Material.h"
#include "Voxel.h"

class Primitive
{
public:
	glm::vec4 position;
	glm::mat4 transform;
	Material mat;

	Primitive()
	{
		this->position = glm::vec4(0.f, 0.f, 0.f, 1.f);
		this->transform = glm::mat4(1.f);
	}

	// Shared functions
	virtual void translate(glm::vec3 translation)
	{
		// Get the new transform matrix after incorporating this translation
		this->transform = glm::translate(this->transform, translation);
		this->position = this->transform * this->position;
	}

	virtual void scale(glm::vec3 scale_factor)
	{

	}

	virtual void rotate(float yaw, float pitch, float roll)
	{
		// Get the new transform matrix after incorporating these rotations
		this->transform = glm::rotate(this->transform, glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
		this->transform = glm::rotate(this->transform, glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
		this->transform = glm::rotate(this->transform, glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));
		this->position = this->transform * this->position;
	}

	virtual float intersection(Ray r)
	{
		std::cout << "INTERSECTION::EXECUTED_FROM_BASE_CLASS\n";
		return -1.f;
	}

	virtual glm::vec4 getNormal(glm::vec4 point)
	{
		std::cout << "GET_NORMAL::EXECUTED_FROM_BASE_CLASS\n";
		return glm::vec4(0.f,0.f,0.f,0.f);
	}

	virtual bool touches(Voxel* vox)
	{
		std::cout << "TOUCHES::EXECUTED_FROM_BASE_CLASS\n";
		return false;
	}
};

class Sphere : public Primitive
{
	// Sphere is defined by its positions and radius
	float radius; // Unique attribute

public:
	Sphere()
	{
		this->radius = 0.1f;
	}

	float intersection(Ray r)
	{
		glm::vec4 oc = r.origin - this->position;
		float a = glm::dot(r.direction, r.direction);
		float b = 2.f * glm::dot(oc, r.direction);
		float c = glm::dot(oc, oc) - radius * radius;
		float discriminant = b * b - 4 * a * c;

		if (discriminant < 0) 
		{
			return -1.0;
		}
		else 
		{
			return (-b - sqrt(discriminant)) / (2.f * a);
		}
	}

	glm::vec4 getNormal(glm::vec4 point)
	{
		return glm::normalize(point - this->position);
	}

	virtual void scale(glm::vec3 scale_factor)
	{
		// Get the new transform matrix after incorporating this scale
		this->radius *= glm::length(scale_factor);
	}

	virtual bool touches(Voxel* vox)
	{
		if (this->position.x + this->radius < vox->coordinate.x ||
			this->position.x - this->radius > (vox->coordinate.x + vox->size.x))
		{
			// Outside bounds along x-axis
			return false;
		}

		if (this->position.y + this->radius < vox->coordinate.y ||
			this->position.y - this->radius > (vox->coordinate.y + vox->size.y))
		{
			// Outside bounds along y-axis
			return false;
		}

		if (this->position.z + this->radius < vox->coordinate.z ||
			this->position.z - this->radius > (vox->coordinate.z + vox->size.z))
		{
			// Outside bounds along z-axis
			return false;
		}

		// Inside all bounds
		return true;
	}
};

//class Plane : public Primitive
//{
//	glm::vec4 normal; // Unique attribute
//
//public:
//	Plane()
//	{
//		this->normal = glm::vec4(0.f, 1.f, 0.f, 0.f);
//	}
//
//	float intersection(Ray r)
//	{
//		float d = glm::dot(this->position, -normal);
//		float t = -(d + r.origin.z * normal.z + r.origin.y * normal.y + r.origin.x * normal.x) /
//			(r.direction.z * normal.z + r.direction.y * normal.y + r.direction.x * normal.x);
//		return t;
//	}
//
//	glm::vec4 getNormal(glm::vec4 point)
//	{
//		return this->normal;
//	}
//
//	virtual void rotate(float yaw, float pitch, float roll)
//	{
//		// Get the new transform matrix after incorporating these rotations
//		this->transform = glm::rotate(this->transform, glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
//		this->transform = glm::rotate(this->transform, glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
//		this->transform = glm::rotate(this->transform, glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));
//		this->normal = this->transform * this->normal;
//	}
//};