#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <glm.hpp>

#include "Voxel.h"
#include "Octree.h"
#include "Ray.h"
#include "HitInfo.h"
#include "Material.h"
#include "Face.h"
#include "OBJLoader.h"

class Mesh
{
public:
    // List of vertices, uvs and normals in the scene
	std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;

    // List of faces in this scene
    std::vector<Face> faces; // Required for octree building and intersection queries

    bool recalculated_normals = false; // Required for shading purposes

    // List of materials required by this scene
    std::vector<Material> materials;

	Octree* octree;

    // Constructor
    Mesh(std::string infile)
    {
        loadOBJ(infile, this->vertices, this->uvs, this->normals);
        
        std::cout << vertices.size() << "\n";

        // Recalculate normals if missing
        if (normals.size() == 0)
        {
            std::cout << "NORMALS ARE MISSING...RECALCULATING...\n";
            this->recalculateNormals();
        }

        // Creating material list
        Material mat;
        materials.push_back(mat);

        
        // Computing faces
        // adding faces to octree
        for (int i = 0; i < vertices.size() / 3; i++)
        {
            Face temp_face;

            // Getting vertices from face
            temp_face.index = i;
            temp_face.vertices[0] = vertices[3 * i];
            temp_face.vertices[1] = vertices[3 * i + 1];
            temp_face.vertices[2] = vertices[3 * i + 2];

            // Pointer to the face material
            temp_face.mat = &this->materials[0]; // Pushing only the first material from list

            faces.push_back(temp_face);
        }

        // Computing octree
        this->computeOctree();
    }

    void recalculateNormals()
    {
        this->normals.clear(); // Clearing normals
        for (int i = 0; i < vertices.size() / 3; i++)
        {
            this->normals.push_back(this->getFaceNormal(i));
        }

        recalculated_normals = true;
    }

    glm::vec3 getFaceNormal(uint32_t face)
    {
        // Getting vertices from face
        glm::vec3 v0 = vertices[3 * face];
        glm::vec3 v1 = vertices[3 * face + 1];
        glm::vec3 v2 = vertices[3 * face + 2];

        glm::vec3 A = glm::normalize(v1 - v0);
        glm::vec3 B = glm::normalize(v2 - v1);

        return glm::normalize(glm::cross(A, B));
    }

	void computeOctree()
	{
        // Finding the min/max bounds of objects
        glm::vec3 minCoords(1000000, 1000000, 1000000);
        glm::vec3 maxCoords(-1000000, -1000000, -1000000);
        for (int i = 0; i < vertices.size(); i++)
        {
            // Assigning min values
            if (vertices[i].x < minCoords.x)
            {
                minCoords.x = vertices[i].x;
            }
            if (vertices[i].y < minCoords.y)
            {
                minCoords.y = vertices[i].y;
            }
            if (vertices[i].z < minCoords.z)
            {
                minCoords.z = vertices[i].z;
            }

            // Assigning max values
            if (vertices[i].x > maxCoords.x)
            {
                maxCoords.x = vertices[i].x;
            }
            if (vertices[i].y > maxCoords.y)
            {
                maxCoords.y = vertices[i].y;
            }
            if (vertices[i].z > maxCoords.z)
            {
                maxCoords.z = vertices[i].z;
            }
        }

        Voxel boundary(minCoords, glm::abs(maxCoords - minCoords));
        this->octree = new Octree(boundary);

        // Adding faces to octree
        for (int i=0; i<faces.size(); i++)
        {
            this->octree->insert(&this->faces[i]);
        }
	}

    HitInfo closestIntersection(Ray ray)
    {
        HitInfo hitInfo;
        hitInfo.hit = false;
        hitInfo.hitDistance = 1000000.f;

        // Getting the closest faces list
        std::vector<Face*> closest_faces;
        octree->query(ray, closest_faces);

        // Checking intersection aginst closest faces
        for (int i=0; i<closest_faces.size(); i++)
        {
            float t = rayFaceIntersect(ray, closest_faces[i]->index);

            if (t > 0.f && t < hitInfo.hitDistance)
            {
                hitInfo.hit = true;
                hitInfo.hitDistance = t;
                hitInfo.hitPoint = ray.origin + ray.direction * t;

                if (!recalculated_normals)
                    hitInfo.normal = glm::normalize(normals[3 * closest_faces[i]->index] + 
                        normals[3 * closest_faces[i]->index + 1] + 
                        normals[3 * closest_faces[i]->index + 2]);
                else
                    hitInfo.normal = normals[closest_faces[i]->index];
                
                hitInfo.face = closest_faces[i];
            }
        }

        return hitInfo;
    }

    float rayFaceIntersect(Ray ray, uint32_t face)
    {
        // Getting vertices from face
        glm::vec3 v0 = vertices[3 * face];
        glm::vec3 v1 = vertices[3 * face + 1];
        glm::vec3 v2 = vertices[3 * face + 2];

        return rayTriangleIntersect(ray, v0, v1, v2);
    }

    float rayTriangleIntersect(
        Ray ray,
        glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
    {
        glm::vec3 v0v1 = v1 - v0;
        glm::vec3 v0v2 = v2 - v0;
        glm::vec3 pvec = glm::cross(glm::vec3(ray.direction), v0v2);
        float det = glm::dot(v0v1, pvec);

        // if the determinant is negative the triangle is backfacing
        // if the determinant is close to 0, the ray misses the triangle
        if (det < 0.00000000001) return -1.f;

        // ray and triangle are parallel if det is close to 0
        if (fabs(det) < 0.00000000001) return -1.f;

        float invDet = 1 / det;

        glm::vec3 tvec = glm::vec3(ray.origin) - v0;
        float u = glm::dot(tvec, pvec) * invDet;
        if (u < 0 || u > 1) return -1.f;

        glm::vec3 qvec = glm::cross(tvec, v0v1);
        float v = glm::dot(glm::vec3(ray.direction), qvec) * invDet;
        if (v < 0 || u + v > 1) return -1.f;

        float t = glm::dot(v0v2, qvec) * invDet;

        return t;
    }
};