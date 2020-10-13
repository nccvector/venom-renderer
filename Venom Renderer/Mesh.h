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
    // List of faces in this scene
    std::vector<Face> faces; // Required for octree building and intersection queries
    
    // For Importance sampling
    std::vector<Face*> emissives;
    std::vector<float> lightImportancePDF;
    std::vector<float> lightImportanceCDF;

    bool recalculated_normals = false; // Required for shading purposes

    // List of materials required by this scene
    std::vector<Material> materials;
    Material defaultMaterial;

	Octree* octree;

    // Constructor
    Mesh(std::string infile)
    {
        loadOBJ(infile, this->faces, this->materials);
        std::cout << "Loaded scene successfully\n";

        // Apply a default material to any face that has no material
        for (int i=0; i<faces.size(); i++)
        {
            if (faces[i].mat == NULL)
            {
                faces[i].mat = &defaultMaterial;
            }
        }

        // Finding out the emissive faces for importance sampling
        float sum = 0;
        for (int i=0; i<faces.size(); i++)
        {
            if (faces[i].mat->emission > 0.f) {
                emissives.push_back(&faces[i]);
                sum += faces[i].mat->emission;
            }
        }
        std::cout << "Emission Sum:         " << sum << "\n";
        std::cout << "Emissives Count:      " << emissives.size() << "\n";

        // Contructing Propotional PDF
        for (int i=0; i<emissives.size(); i++)
        {
            lightImportancePDF.push_back(emissives[i]->mat->emission / sum);
        }

        // Constructing CDF
        float runningSum = 0;
        for (int i=0; i<emissives.size(); i++)
        {
            runningSum += lightImportancePDF[i];
            lightImportanceCDF.push_back(runningSum);
        }

        std::cout << "Final face count:     " << faces.size() << "\n";

        // Perform a normals check here...

        // Computing octree
        this->computeOctree();
    }

    //void recalculateNormals()
    //{
    //    for (int i = 0; i < faces.size(); i++)
    //    {
    //        this->faces[i].normals.push_back(this->getFaceNormal(i));
    //    }

    //    recalculated_normals = true;
    //}

    glm::vec3 getFaceNormal(Face face)
    {

        glm::vec3 A = glm::normalize(face.vertices[1] - face.vertices[0]);
        glm::vec3 B = glm::normalize(face.vertices[2] - face.vertices[1]);

        return glm::normalize(glm::cross(A, B));
    }

	void computeOctree()
	{
        // Finding the min/max bounds of objects
        glm::vec3 minCoords(1000000, 1000000, 1000000);
        glm::vec3 maxCoords(-1000000, -1000000, -1000000);
        for (int f = 0; f < faces.size(); f++)
        {
            // Looping over vertices of this face
            for (int i=0; i<3; i++)
            {
                // Assigning min values
                if (faces[f].vertices[i].x < minCoords.x)
                {
                    minCoords.x = faces[f].vertices[i].x;
                }
                if (faces[f].vertices[i].y < minCoords.y)
                {
                    minCoords.y = faces[f].vertices[i].y;
                }
                if (faces[f].vertices[i].z < minCoords.z)
                {
                    minCoords.z = faces[f].vertices[i].z;
                }

                // Assigning max values
                if (faces[f].vertices[i].x > maxCoords.x)
                {
                    maxCoords.x = faces[f].vertices[i].x;
                }
                if (faces[f].vertices[i].y > maxCoords.y)
                {
                    maxCoords.y = faces[f].vertices[i].y;
                }
                if (faces[f].vertices[i].z > maxCoords.z)
                {
                    maxCoords.z = faces[f].vertices[i].z;
                }
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
            float t = rayFaceIntersect(ray, closest_faces[i]);

            if (t > 0.f && t < hitInfo.hitDistance)
            {
                hitInfo.hit = true;
                hitInfo.hitDistance = t;
                hitInfo.hitPoint = ray.origin + ray.direction * t;

                // Getting the average normal
                hitInfo.normal = glm::normalize(closest_faces[i]->normals[0] + 
                    closest_faces[i]->normals[1] +
                    closest_faces[i]->normals[2]);
                
                hitInfo.face = closest_faces[i];
            }
        }

        return hitInfo;
    }

    float rayFaceIntersect(Ray ray, Face* face)
    {
        return rayTriangleIntersect(ray, face->vertices[0], face->vertices[1], face->vertices[2]);
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