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

class Mesh {
public:
    // List of faces in this scene
    std::vector <Face> faces; // Required for octree building and intersection queries

    // For Importance sampling
    std::vector<Face *> emissives;
    std::vector<float> lightImportancePDF;
    std::vector<float> lightImportanceCDF;

    bool recalculated_normals = false; // Required for shading purposes

    // List of materials required by this scene
    std::vector <Material> materials;
    Material defaultMaterial;

    Octree *octree;

    // Constructor
    Mesh(std::string infile) {
        loadOBJ(infile, this->faces, this->materials);
        std::cout << "Loaded scene successfully\n";

        // Apply a default material to any face that has no material
        for (int i = 0; i < faces.size(); i++) {
            if (faces[i].mat == NULL) {
                faces[i].mat = &defaultMaterial;
            }
        }

        // Finding out the emissive faces for importance sampling
        float sum = 0;
        for (int i = 0; i < faces.size(); i++) {
            if (faces[i].mat->emission > 0.f) {
                emissives.push_back(&faces[i]);
                sum += faces[i].mat->emission;
            }
        }
        std::cout << "Emission Sum:         " << sum << "\n";
        std::cout << "Emissives Count:      " << emissives.size() << "\n";

        // Contructing Propotional PDF
        for (int i = 0; i < emissives.size(); i++) {
            lightImportancePDF.push_back(emissives[i]->mat->emission / sum);
        }

        // Constructing CDF
        float runningSum = 0;
        for (int i = 0; i < emissives.size(); i++) {
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

    glm::vec3 getFaceNormal(Face face) {

        glm::vec3 A = glm::normalize(face.vertices[1] - face.vertices[0]);
        glm::vec3 B = glm::normalize(face.vertices[2] - face.vertices[1]);

        return glm::normalize(glm::cross(A, B));
    }

    void computeOctree() {
        // Finding the min/max bounds of objects
        glm::vec3 minCoords(1000000, 1000000, 1000000);
        glm::vec3 maxCoords(-1000000, -1000000, -1000000);
        for (int f = 0; f < faces.size(); f++) {
            // Looping over vertices of this face
            for (int i = 0; i < 3; i++) {
                // Assigning min values
                if (faces[f].vertices[i].x < minCoords.x) {
                    minCoords.x = faces[f].vertices[i].x;
                }
                if (faces[f].vertices[i].y < minCoords.y) {
                    minCoords.y = faces[f].vertices[i].y;
                }
                if (faces[f].vertices[i].z < minCoords.z) {
                    minCoords.z = faces[f].vertices[i].z;
                }

                // Assigning max values
                if (faces[f].vertices[i].x > maxCoords.x) {
                    maxCoords.x = faces[f].vertices[i].x;
                }
                if (faces[f].vertices[i].y > maxCoords.y) {
                    maxCoords.y = faces[f].vertices[i].y;
                }
                if (faces[f].vertices[i].z > maxCoords.z) {
                    maxCoords.z = faces[f].vertices[i].z;
                }
            }
        }

        Voxel boundary(minCoords, glm::abs(maxCoords - minCoords));
        this->octree = new Octree(boundary);

        // Adding faces to octree
        for (int i = 0; i < faces.size(); i++) {
            this->octree->insert(&this->faces[i]);
        }
    }

    HitInfo closestIntersection(Ray ray) {
        HitInfo hitInfo;
        hitInfo.hit = false;
        hitInfo.texcoord = glm::vec2(0);
        hitInfo.hitDistance = 1000000.f;

        octree->query(ray, &hitInfo);

        // Filling necessary information only if hit something
        if (hitInfo.hit) {
            // Calculating remaining params of hitInfo
            hitInfo.hitPoint = ray.origin + ray.direction * hitInfo.hitDistance;

            // Getting the average normal
            hitInfo.normal = glm::normalize(hitInfo.face->normals[0] +
                                            hitInfo.face->normals[1] +
                                            hitInfo.face->normals[2]);

            // Calculating bary coordinates
            float b0, b1, b2;
            GetBaryCoords(hitInfo.face->vertices[0],
                          hitInfo.face->vertices[1], hitInfo.face->vertices[2],
                          hitInfo.hitPoint, &b1, &b2);

            b0 = 1.f - b1 - b2;

            // Texcoord at hitPoint is
            hitInfo.texcoord = b0 * hitInfo.face->uvs[0] +
                               b1 * hitInfo.face->uvs[1] + b2 * hitInfo.face->uvs[2];
        }

        return hitInfo;
    }

    bool GetBaryCoords(glm::vec3 &p0, glm::vec3 &p1, glm::vec3 &p2,
                       glm::vec3 &hitPoint, float *b1, float *b2) {
        glm::vec3 u = p1 - p0;
        glm::vec3 v = p2 - p0;
        glm::vec3 w = hitPoint - p0;

        glm::vec3 vCrossW = glm::cross(v, w);
        glm::vec3 vCrossU = glm::cross(v, u);

        if (glm::dot(vCrossW, vCrossU) < 0.f)
            return false;

        glm::vec3 uCrossW = glm::cross(u, w);
        glm::vec3 uCrossV = glm::cross(u, v);

        if (glm::dot(uCrossW, uCrossV) < 0.f)
            return false;

        const float denom = glm::length(uCrossV);
        const float r = glm::length(vCrossW) / denom;
        const float t = glm::length(uCrossW) / denom;

        *b1 = r;
        *b2 = t;

        return ((r <= 1.f) && (t <= 1.f) && (r + t <= 1.f));
    }
};