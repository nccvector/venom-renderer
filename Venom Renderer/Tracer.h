#pragma once

#include<iostream>
#include<vector>
#include<random>
#include<cmath>
#include<chrono>

#include "Ray.h"
#include "Mesh.h"
#include "SurfaceProperties.h"

# define M_PI           3.14159265358979323846

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
        SurfaceProperties sp;
        sp.hit = false;

        // closest hit distance and id of closest object
        float closest_distance = 10000.f;

        for (int i=0; i<this->meshes.size(); i++)
        {
            SurfaceProperties tempSp = this->meshes[i].closestIntersection(ray);

            if (tempSp.hit)
            {
                if (tempSp.hitDistance > 0.f && tempSp.hitDistance < closest_distance)
                {
                    sp = tempSp;
                }
            }
        }

        if (!sp.hit || bounces <= 0)
            return glm::vec3(1.f, 1.f, 1.f);

        // Find the reflected ray
        glm::vec3 hitPoint = sp.hitPoint;
        glm::vec3 normal = sp.normal;

        float c1 = -glm::dot(normal, ray.direction);
        glm::vec3 Rl(ray.direction + (2.f * normal * c1));

        glm::vec3 indirectLigthing(0.f, 0.f, 0.f);
        glm::vec3 indirectSpecular(0.f,0.f,0.f);

        glm::vec3 Nt, Nb;
        createCoordinateSystem(normal, Nt, Nb);
        float pdf = 1 / (2 * M_PI);

        for (int i=0; i<samples; i++)
        {
            float r1 = distribution(generator);
            float r2 = distribution(generator);
            glm::vec3 sample = uniformSampleHemisphere(r1, r2);
            // Transforming sample to world (from a scratch a pixel)
            glm::vec3 sampleWorld(
                sample.x * Nb.x + sample.y * normal.x + sample.z * Nt.x,
                sample.x * Nb.y + sample.y * normal.y + sample.z * Nt.y,
                sample.x * Nb.z + sample.y * normal.z + sample.z * Nt.z);

            // Specular sample biasing
            glm::vec3 sampleSpecular = sp.roughness * sampleWorld + (1- sp.roughness) * glm::vec3(Rl);

            Ray diffuse_ray;
            diffuse_ray.origin = hitPoint + normal * 0.000001f;
            diffuse_ray.direction = glm::vec4(sampleWorld, 0.f);

            Ray specular_ray;
            specular_ray.origin = hitPoint + normal * 0.000001f;
            specular_ray.direction = sampleSpecular;

            float diffusion_dot = glm::dot(diffuse_ray.direction, normal);
            float specular_dot = glm::dot(specular_ray.direction, Rl);
            float fresnel_dot = std::min((1.f - glm::dot(normal, -ray.direction)) + sp.baseSpecular, 1.f);

            indirectLigthing += (1- sp.specularWeight * fresnel_dot) * r1 * trace(diffuse_ray, bounces - 1, 1) * diffusion_dot / pdf;
            indirectSpecular += sp.specularWeight * r1 * trace(specular_ray, bounces - 1, 1) * specular_dot * fresnel_dot / pdf;
        }

        return (indirectLigthing/float(samples)) * sp.color/float(2.f * M_PI) +
            (indirectSpecular / float(samples)) / float(2.f * M_PI);
    }

    // Get color list from region
    glm::vec3* colorFromRegion(glm::vec4 region)
    {
        glm::vec3* colors = new glm::vec3[int(region.y-region.x) * int(region.w-region.z)];

        // Looping over the canvas pixels to fill the color information
        for (int y = 0; y < (region.w-region.z); y++)
        {
            for (int x = 0; x < (region.y-region.x); x++)
            {
                // Extract colors from the appropriate rays
                colors[x + y * (int)(region.y - region.x)] = this->trace(this->transRays[int(y+region.z)][int(x+region.x)]);
            }
        }

        return colors;
    }
};