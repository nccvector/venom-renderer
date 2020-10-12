#pragma once

#include<glm.hpp>

#include "Voxel.h"
#include "Ray.h"
#include "Face.h"
#include "TriangleBoxIntersection.h"

class Octree
{
	Voxel boundary;
	int voxel_capacity;
	std::vector<Face*> face_list;
	bool divided = false;

	Octree* bottomNorthWest;
	Octree* bottomNorthEast;
	Octree* bottomSouthWest;
	Octree* bottomSouthEast;
	Octree* topNorthWest;
	Octree* topNorthEast;
	Octree* topSouthWest;
	Octree* topSouthEast;

	int depth = 0;
	int max_depth = 20;

public:
	Octree()
	{

	}

	// Constructor
	Octree(Voxel boundary, int voxel_capacity = 8, int depth = 0)
	{
		this->boundary = boundary;
		this->voxel_capacity = voxel_capacity;
		this->depth = depth;
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
		this->bottomNorthWest = new Octree(bottomNorthWestBound, this->voxel_capacity, this->depth+1);
		this->bottomNorthEast = new Octree(bottomNorthEastBound, this->voxel_capacity, this->depth + 1);
		this->bottomSouthWest = new Octree(bottomSouthWestBound, this->voxel_capacity, this->depth + 1);
		this->bottomSouthEast = new Octree(bottomSouthEastBound, this->voxel_capacity, this->depth + 1);

		this->topNorthWest = new Octree(topNorthWestBound, this->voxel_capacity, this->depth + 1);
		this->topNorthEast = new Octree(topNorthEastBound, this->voxel_capacity, this->depth + 1);
		this->topSouthWest = new Octree(topSouthWestBound, this->voxel_capacity, this->depth + 1);
		this->topSouthEast = new Octree(topSouthEastBound, this->voxel_capacity, this->depth + 1);

		// Set subdivided true
		this->divided = true;
	}

	void insert(Face* face)
	{
		if (!triBoxOverlap(
			// Box center
			glm::vec3(this->boundary.coordinate.x + this->boundary.size.x / 2,
				this->boundary.coordinate.y + this->boundary.size.y / 2,
				this->boundary.coordinate.z + this->boundary.size.z / 2),

			// Box half-size
			glm::vec3(this->boundary.size.x / 2,
				this->boundary.size.y / 2,
				this->boundary.size.z / 2),

			// Triangle vertices
			face->vertices[0], face->vertices[1], face->vertices[2]
		))
		{
			return;
		}

		if (face_list.size() < voxel_capacity || this->depth > this->max_depth)
		{
			// Ignoring the voxel capacity if the min size is reached
			this->face_list.push_back(face);
		}
		else
		{
			// Divide if not divided
			if (!this->divided)
			{
				this->subdivide();
			}

			// Try inserting into all the child octrees
			this->bottomNorthWest->insert(face);
			this->bottomNorthEast->insert(face);
			this->bottomSouthWest->insert(face);
			this->bottomSouthEast->insert(face);

			this->topNorthWest->insert(face);
			this->topNorthEast->insert(face);
			this->topSouthWest->insert(face);
			this->topSouthEast->insert(face);
		}
	}

	void query(Ray ray, std::vector<Face*>& objectList)
	{

		// Lets just get out if ray does not intersects this boundary
		if (!this->boundary.intersects(ray))
		{
			return;
		}

		// Copy the objects in prim list to the object list
		objectList.reserve(objectList.size() + this->face_list.size());
		objectList.insert(objectList.end(), this->face_list.begin(), this->face_list.end());

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