#pragma once

#include<random>
#include<cmath>
#include<chrono>

#include "Ray.h"
#include "Primitives.h"
#include "Octree.h"

# define M_PI           3.14159265358979323846

class Tracer
{
    // Scene Octree
    Octree octree;

    // 2D Array of rays from the scene
    Ray** transRays;
    
    int IMAGE_WIDTH;
    int IMAGE_HEIGHT;
    int OBJECT_COUNT;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution{ 0, 1 };

public:
	Tracer(Octree octree, Ray** transRays, int IMAGE_WIDTH, int IMAGE_HEIGHT)
	{
		this->octree = octree;
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

    glm::vec3 trace(Ray ray, int bounces=2, int samples=32)
    {
        // closest hit distance and id of closest object
        float closest_distance = 10000.f;
        Primitive* closest_object = NULL;

        // Getting a list of potential hitables
        std::vector<Primitive*> prims;
        this->octree.query(ray, &prims);

        //std::cout << prims.size() << std::endl;

        // Checking the intersection of ray with the potentially hitable objects
        std::vector<Primitive*>::iterator k;
        for (k = prims.begin(); k != prims.end(); ++k) {
            float distance = (*k)->intersection(ray);

            if (distance > 0.f) {
                // Hit the primitive
                // store in closest distance and id if distance is less than closest distance
                if (distance < closest_distance)
                {
                    closest_distance = distance;
                    closest_object = (*k);
                }
            }
        }

        if (closest_object == NULL || bounces <= 0)
            return glm::vec3(1.f, 1.f, 1.f);

        // Find the reflected ray
        glm::vec4 hitPoint = ray.origin + ray.direction * closest_distance;
        glm::vec4 normal = closest_object->getNormal(hitPoint);

        float c1 = -glm::dot(normal, ray.direction);
        glm::vec4 Rl(ray.direction + (2.f * normal * c1));

        // simple reflection case
        //return prims[closest_id]->mat.color * trace(reflected_ray, bounces-1);

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
            glm::vec3 sampleSpecular = closest_object->mat.roughness * sampleWorld + (1- closest_object->mat.roughness) * glm::vec3(Rl);

            Ray diffuse_ray;
            diffuse_ray.origin = hitPoint + normal * 0.000001f;
            diffuse_ray.direction = glm::vec4(sampleWorld, 0.f);

            Ray specular_ray;
            specular_ray.origin = hitPoint + normal * 0.000001f;
            specular_ray.direction = glm::vec4(sampleSpecular, 0.f);

            float diffusion_dot = glm::dot(diffuse_ray.direction, normal);
            float specular_dot = glm::dot(specular_ray.direction, Rl);
            float fresnel_dot = std::min((1.f - glm::dot(normal, -ray.direction)) + closest_object->mat.base_specular, 1.f);

            indirectLigthing += (1- closest_object->mat.specular_weight * fresnel_dot) * r1 * trace(diffuse_ray, bounces - 1, 1) * diffusion_dot / pdf;
            indirectSpecular += closest_object->mat.specular_weight * r1 * trace(specular_ray, bounces - 1, 1) * specular_dot * fresnel_dot / pdf;
        }

        if (closest_object->mat.metal)
        {
            return (indirectLigthing / float(samples)) * closest_object->mat.color / float(2.f * M_PI) +
                (indirectSpecular / float(samples)) * closest_object->mat.color / float(2.f * M_PI);
        }

        return (indirectLigthing/float(samples)) * closest_object->mat.color/float(2.f * M_PI) +
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