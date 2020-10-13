#pragma once

#include<iostream>
#include<vector>
#include<random>
#include<cmath>
#include<chrono>

#include "Ray.h"
#include "Mesh.h"
#include "HitInfo.h"

# define M_PI           3.1415926535f

class Tracer
{
    // Array of meshes
    std::vector<Mesh> meshes;

    // 2D Array of rays from the scene
    Ray** transRays;
    
    int IMAGE_WIDTH;
    int IMAGE_HEIGHT;
    int OBJECT_COUNT;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution{ 0, 1 };

public:
	Tracer(std::vector<Mesh> meshes, Ray** transRays, int IMAGE_WIDTH, int IMAGE_HEIGHT)
	{
		this->meshes = meshes;
        this->IMAGE_WIDTH = IMAGE_WIDTH;
        this->IMAGE_HEIGHT= IMAGE_HEIGHT;
        this->OBJECT_COUNT = OBJECT_COUNT;

        // Saving transRays
        this->transRays = transRays;
	}

    void createCoordinateSystem(const glm::vec3& N, glm::vec3& Nt, glm::vec3& Nb)
    {
        if (std::fabs(N.x) > std::fabs(N.y))
            Nt = glm::vec3(N.z, 0, -N.x) / sqrtf(N.x * N.x + N.z * N.z);
        else
            Nt = glm::vec3(0, -N.z, N.y) / sqrtf(N.y * N.y + N.z * N.z);
        Nb = glm::cross(N, Nt);
    }

    glm::vec3 uniformSampleHemisphere(const float& r1, const float& r2)
    {
        // cos(theta) = u1 = y
        // cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
        float sinTheta = sqrtf(1 - r1 * r1);
        float phi = 2 * M_PI * r2;
        float x = sinTheta * cosf(phi);
        float z = sinTheta * sinf(phi);
        return glm::vec3(x, r1, z);
    }

    glm::vec3 trace(Ray ray, int bounces=2, int samples=1)
    {
        HitInfo hitInfo;
        hitInfo.hit = false;

        // closest hit distance and id of closest object
        float closest_distance = 10000.f;

        for (int i=0; i<this->meshes.size(); i++)
        {
            HitInfo tempHitInfo = this->meshes[i].closestIntersection(ray);

            if (tempHitInfo.hit)
            {
                if (tempHitInfo.hitDistance > 0.f && tempHitInfo.hitDistance < closest_distance)
                {
                    hitInfo = tempHitInfo;
                }
            }
        }

        // Return sky color if hit nothing
        if (!hitInfo.hit)
        {
            return glm::vec3(1.f, 1.f, 1.f);
        }
        
        // return color*ambienLight if bounces are over
        if(bounces <= 0)
        {
            return glm::vec3(0.1f, 0.1f, 0.1f) * hitInfo.face->mat->baseColor;
        }

        // Find the reflected ray
        //float c1 = -glm::dot(hitInfo.normal, ray.direction);
        //glm::vec3 Rl(ray.direction + (2.f * hitInfo.normal * c1));

        glm::vec3 Nt, Nb;
        createCoordinateSystem(hitInfo.normal, Nt, Nb);

        glm::vec3 indirectLight(0.f, 0.f, 0.f); // Starting from darkness
        for (int i=0; i<samples; i++)
        {
            Ray diffuse_ray;
            diffuse_ray.origin = hitInfo.hitPoint + hitInfo.normal * 0.0000001f;

            float r1 = distribution(generator);
            float r2 = distribution(generator);
            glm::vec3 sample = uniformSampleHemisphere(r1, r2);
            // Transforming sample to world (from a scratch a pixel)
            diffuse_ray.direction = glm::vec3(
                sample.x * Nb.x + sample.y * hitInfo.normal.x + sample.z * Nt.x,
                sample.x * Nb.y + sample.y * hitInfo.normal.y + sample.z * Nt.y,
                sample.x * Nb.z + sample.y * hitInfo.normal.z + sample.z * Nt.z);

            float pdf = 1/(2*M_PI);

            indirectLight += hitInfo.face->mat->BRDF(diffuse_ray.direction, -ray.direction, hitInfo.normal, Nt, Nb) *
                trace(diffuse_ray, bounces - 1, 1) * glm::dot(hitInfo.normal, diffuse_ray.direction) / pdf;
        }

        return indirectLight/float(samples);
    }

    // Get color list from region
    void colorFromRegion(glm::vec4 region, glm::vec3* colors)
    {
        // Looping over the canvas pixels to fill the color information
        for (int y = 0; y < (region.w-region.z); y++)
        {
            for (int x = 0; x < (region.y-region.x); x++)
            {
                // Extract colors from the appropriate rays
                colors[x + y * (int)(region.y - region.x)] = this->trace(this->transRays[int(y+region.z)][int(x+region.x)]);
            }
        }
    }
};