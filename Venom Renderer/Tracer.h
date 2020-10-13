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
    Mesh* scene;

    // 2D Array of rays from the scene
    Ray** transRays;
    
    int IMAGE_WIDTH;
    int IMAGE_HEIGHT;
    int OBJECT_COUNT;

    std::default_random_engine generator;
    std::uniform_real_distribution<float> distribution{ 0, 1 };

public:
	Tracer(Mesh& scene, Ray** transRays, int IMAGE_WIDTH, int IMAGE_HEIGHT)
	{
		this->scene = &scene;
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

    glm::vec3 trace(Ray ray, int bounces=1, int samples=1)
    {
        HitInfo hitInfo;
        hitInfo.hit = false;

        // closest hit distance and id of closest object
        float closest_distance = 10000.f;

        // Getting closest hit info from the scene
        HitInfo tempHitInfo = this->scene->closestIntersection(ray);

        if (tempHitInfo.hit)
        {
            if (tempHitInfo.hitDistance > 0.f && tempHitInfo.hitDistance < closest_distance)
            {
                hitInfo = tempHitInfo;
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

        glm::vec3 Nt, Nb;
        createCoordinateSystem(hitInfo.normal, Nt, Nb);

        float lightImportanceSamplingWeight = 0.6f; // 50 percent biasness towards light sources
        // I.e 0.5 will be multiplied by the pdf taken from the light pdf list
        float uniformPDF = (1 / (2 * M_PI)) * (1-lightImportanceSamplingWeight); // Uniform pdf over semi-sphere
        glm::vec3 indirectLight(0.f, 0.f, 0.f); // Starting from darkness
        for (int i=0; i<samples; i++)
        {
            Ray diffuse_ray;
            diffuse_ray.origin = hitInfo.hitPoint + hitInfo.normal * 0.0000001f;

            //std::cout << brdf.x << " " << brdf.y << " " << brdf.z << "\n";

            float r = distribution(generator); // 0-1

            float nonUniPDF = NULL;
            if (r < lightImportanceSamplingWeight)
            {
                // Sampling a light source
                r = distribution(generator); // generating again to see which light source to sample
                for (int l=0; l<scene->lightImportanceCDF.size(); l++)
                {
                    if (r < scene->lightImportanceCDF[l])
                    {
                        // Getting the non uniform pdf of this light source
                        nonUniPDF = scene->lightImportancePDF[l] * lightImportanceSamplingWeight;

                        // Sample this light source and break
                        glm::vec3 centerLightCoord = (scene->emissives[l]->vertices[0] +
                            scene->emissives[l]->vertices[1] +
                            scene->emissives[l]->vertices[2]) / 3.f;

                        // Setting the ray direction towards the light source center
                        diffuse_ray.direction = glm::normalize(centerLightCoord - diffuse_ray.origin);
                        //std::cout << "Sampled a light source with pdf:  " << nonUniPDF << "\n";
                        break; // exit loop
                    }
                }
            }
            
            // Sampling indirect light if the probabilty says, or if the ray direction is going inside mesh
            if(r >= lightImportanceSamplingWeight || glm::dot(diffuse_ray.direction, hitInfo.normal) <= 0.f)
            {
                // Sample the random semi-sphere
                // Setting the diffuse ray direction from uniform semi-sphere sampling
                float r1 = distribution(generator);
                float r2 = distribution(generator);
                glm::vec3 sample = uniformSampleHemisphere(r1, r2);
                // Transforming sample to world (from a scratch a pixel)
                diffuse_ray.direction = glm::vec3(
                    sample.x * Nb.x + sample.y * hitInfo.normal.x + sample.z * Nt.x,
                    sample.x * Nb.y + sample.y * hitInfo.normal.y + sample.z * Nt.y,
                    sample.x * Nb.z + sample.y * hitInfo.normal.z + sample.z * Nt.z);

                //std::cout << "Sampled indirect ligth:  " << uniformPDF << "\n";
            }

            glm::vec3 specularColor(0);
            glm::vec3 diffuse(0);
            glm::vec3 brdf = hitInfo.face->mat->BRDF(diffuse_ray.direction, -ray.direction, hitInfo.normal, Nt, Nb,
                specularColor, diffuse);

            indirectLight += hitInfo.face->mat->baseColor * hitInfo.face->mat->emission;
            indirectLight += brdf * trace(diffuse_ray, bounces - 1, samples) *
                glm::dot(hitInfo.normal, diffuse_ray.direction) * lightImportanceSamplingWeight / uniformPDF;
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