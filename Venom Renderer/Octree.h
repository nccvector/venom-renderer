#pragma once
#include<list>
#include<glm.hpp>

#include "Voxel.h"
#include "Primitives.h"
#include "Ray.h"

class Octree
{
	Voxel boundary;
	int voxel_capacity;
	std::vector<Primitive*> primitive_list;
	bool divided = false;

	Octree* bottomNorthWest;
	Octree* bottomNorthEast;
	Octree* bottomSouthWest;
	Octree* bottomSouthEast;
	Octree* topNorthWest;
	Octree* topNorthEast;
	Octree* topSouthWest;
	Octree* topSouthEast;

public:
	Octree()
	{

	}

	// Constructor
	Octree(Voxel boundary, int voxel_capacity=2)
	{
		this->boundary = boundary;
		this->voxel_capacity = voxel_capacity;
	}

	void subdivide()
	{
		// Bottom Voxels
		Voxel bottomNorthWestBound(this->boundary.coordinate, 
			this->boundary.size / 2.f);
		Voxel bottomNorthEastBound(this->boundary.coordinate + glm::vec3(this->boundary.size.x / 2, 0.f, 0.f), 
			this->boundary.size / 2.f);
		Voxel bottomSouthWestBound(this->boundary.coordinate + glm::vec3(0.f, 0.f, this->boundary.size.z / 2), 
			this->boundary.size / 2.f);
		Voxel bottomSouthEastBound(this->boundary.coordinate + glm::vec3(this->boundary.size.x / 2, 0.f, this->boundary.size.z / 2),
			this->boundary.size / 2.f);

		// Top Voxels
		Voxel topNorthWestBound(this->boundary.coordinate + glm::vec3(0.f, this->boundary.size.y / 2, 0.f),
			this->boundary.size / 2.f);
		Voxel topNorthEastBound(this->boundary.coordinate + glm::vec3(this->boundary.size.x / 2, this->boundary.size.y / 2, 0.f),
			this->boundary.size / 2.f);
		Voxel topSouthWestBound(this->boundary.coordinate + glm::vec3(0.f, this->boundary.size.y / 2, this->boundary.size.z / 2),
			this->boundary.size / 2.f);
		Voxel topSouthEastBound(this->boundary.coordinate + glm::vec3(this->boundary.size.x / 2, this->boundary.size.y / 2, this->boundary.size.z / 2),
			this->boundary.size / 2.f);

		// Making Quadtrees
		this->bottomNorthWest = new Octree(bottomNorthWestBound);
		this->bottomNorthEast = new Octree(bottomNorthEastBound);
		this->bottomSouthWest = new Octree(bottomSouthWestBound);
		this->bottomSouthEast = new Octree(bottomSouthEastBound);

		this->topNorthWest = new Octree(topNorthWestBound);
		this->topNorthEast = new Octree(topNorthEastBound);
		this->topSouthWest = new Octree(topSouthWestBound);
		this->topSouthEast = new Octree(topSouthEastBound);

		// Set subdivided true
		this->divided = true;
	}

	void insert(Primitive* prim)
	{
		// Lets just get out if the primitive does not lie inside
		if(!prim->touches(&this->boundary))
		{
			return;
		}

		if (primitive_list.size() < voxel_capacity || this->boundary.size.x < 0.1f)
		{
			// Ignoring the voxel capacity if the min size is reached
			this->primitive_list.push_back(prim);
		}
		else
		{
			// Divide if not divided
			if (!this->divided)
			{
				this->subdivide();
			}

			// Try inserting into all the child octrees
			this->bottomNorthWest->insert(prim);
			this->bottomNorthEast->insert(prim);
			this->bottomSouthWest->insert(prim);
			this->bottomSouthEast->insert(prim);

			this->topNorthWest->insert(prim);
			this->topNorthEast->insert(prim);
			this->topSouthWest->insert(prim);
			this->topSouthEast->insert(prim);
		}
	}

	void query(Ray ray, std::vector<Primitive*>* objectList)
	{

		// Lets just get out if ray does not intersects this boundary
		if (!this->boundary.intersects(ray))
		{
			return;
		}

		// Copy the objects in prim list to the object list
		objectList->reserve(objectList->size() + this->primitive_list.size());
		objectList->insert(objectList->end(), this->primitive_list.begin(), this->primitive_list.end());

		// Adding the objects of children too if they exist
		if (this->divided)
		{
			this->bottomNorthWest->query(ray, objectList);
			this->bottomNorthEast->query(ray, objectList);
			this->bottomSouthWest->query(ray, objectList);
			this->bottomSouthEast->query(ray, objectList);

			this->topNorthWest->query(ray, objectList);
			this->topNorthEast->query(ray, objectList);
			this->topSouthWest->query(ray, objectList);
			this->topSouthEast->query(ray, objectList);
		}

		// Returning the filled list
		return;
	}
};