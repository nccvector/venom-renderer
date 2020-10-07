#include <iostream>
#include <algorithm>    // std::shuffle
#include <vector>       // std::vector
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <thread>

#include "opencv2/opencv.hpp"

#include "Camera.h"
#include "Ray.h"
#include "Primitives.h"
#include "Tracer.h"
#include "Octree.h"
#include "Voxel.h"

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution{ 0, 1 };

std::string NowToString()
{
    std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
    time_t t = std::chrono::system_clock::to_time_t(p);
    char str[26];
    ctime_s(str, sizeof str, &t);
    return std::string(str);
}

int main()
{
    std::string startTime = NowToString();
    replace(startTime.begin(), startTime.end(), ':', '_');
    replace(startTime.begin(), startTime.end(), '\n', '_');

    clock_t tStart = clock();

    // Fixing random object spawn seed
    generator.seed(1000);

    // Creating image canvas
    int ANTI_ALIASING = 1; // 2X
    int IMAGE_WIDTH = 640 * ANTI_ALIASING;
    int IMAGE_HEIGHT = 480 * ANTI_ALIASING;    // 16:9
    cv::Mat image(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat outImg;

    // Creating and preparing camera object
    Camera cam(IMAGE_WIDTH, IMAGE_HEIGHT);
    cam.translate(glm::vec3(0.f, 4.f, 3.f));
    cam.rotate(0.f, -35.f, 0.f);

    // Creating primitives (A sphere and a plane)
    int num_objects = 100;
    Primitive** prims = new Primitive * [num_objects];
    //prims[0] = new Plane();
    //prims[0]->mat.color = glm::vec3(0.99f, 0.99f, 0.99f);
    //prims[0]->mat.base_specular = 0.05f;
    //prims[0]->mat.specular_weight = 0.9f;
    //prims[0]->mat.roughness = 0.777f;

    prims[0] = new Sphere();
    prims[0]->mat.color = glm::vec3(0.6f, 0.6f, 0.6f);
    prims[0]->mat.base_specular = 0.1f;
    prims[0]->mat.specular_weight = 0.9f;
    prims[0]->mat.roughness = 0.15f;
    prims[0]->mat.metal = true;
    prims[0]->translate(glm::vec3(-3.f, 0.5f, 0.f)); // Elevating it along up axis

    prims[1] = new Sphere();
    prims[1]->mat.color = glm::vec3(203.f / 255.f, 148.f / 255.f, 145.f / 255.f);
    prims[1]->mat.base_specular = 0.1f;
    prims[1]->mat.specular_weight = 1.f;
    prims[1]->mat.roughness = 0.01f;
    prims[1]->mat.metal = true;
    prims[1]->translate(glm::vec3(-1.f, 0.5f, 0.f)); // Elevating it along up axis

    prims[2] = new Sphere();
    prims[2]->mat.color = glm::vec3(203.f / 255.f, 148.f / 255.f, 145.f / 255.f);
    prims[2]->mat.base_specular = 0.1f;
    prims[2]->mat.specular_weight = 1.f;
    prims[2]->mat.roughness = 0.25f;
    prims[2]->mat.metal = true;
    prims[2]->translate(glm::vec3(0.f, 0.5f, 0.f)); // Elevating it along up axis

    prims[3] = new Sphere();
    prims[3]->mat.color = glm::vec3(203.f / 255.f, 148.f / 255.f, 145.f / 255.f);
    prims[3]->mat.base_specular = 0.1f;
    prims[3]->mat.specular_weight = 1.f;
    prims[3]->mat.roughness = 0.5f;
    prims[3]->mat.metal = true;
    prims[3]->translate(glm::vec3(1.f, 0.5f, 0.f)); // Elevating it along up axis

    // Adding random object
#pragma omp parallel for
    for (int i = 4; i < num_objects; i++)
    {
        prims[i] = new Sphere();
        prims[i]->mat.color = glm::vec3(float(distribution(generator)), float(distribution(generator)), float(distribution(generator)));
        prims[i]->mat.base_specular = std::max(0.1f, float(distribution(generator)));
        prims[i]->mat.specular_weight = std::max(0.1f, float(distribution(generator)));
        prims[i]->mat.roughness = std::min(0.8f, float(distribution(generator)));
        prims[i]->translate(glm::vec3((float(distribution(generator)) - 0.5f) * 10.f,
            (float(distribution(generator)) - 0.5f) * 3.f, -float(distribution(generator) * 10.f))); // Elevating it along up axis
    }

    // Finding the min/max bounds of objects
    glm::vec3 minCoords(1000000, 1000000, 1000000);
    glm::vec3 maxCoords(-1000000, -1000000, -1000000);
    for (int i = 0; i < num_objects; i++)
    {
        // Assigning min values
        if (prims[i]->position.x < minCoords.x)
        {
            minCoords.x = prims[i]->position.x;
        }
        if (prims[i]->position.y < minCoords.y)
        {
            minCoords.y = prims[i]->position.y;
        }
        if (prims[i]->position.z < minCoords.z)
        {
            minCoords.z = prims[i]->position.z;
        }

        // Assigning max values
        if (prims[i]->position.x > maxCoords.x)
        {
            maxCoords.x = prims[i]->position.x;
        }
        if (prims[i]->position.y > maxCoords.y)
        {
            maxCoords.y = prims[i]->position.y;
        }
        if (prims[i]->position.z > maxCoords.z)
        {
            maxCoords.z = prims[i]->position.z;
        }
    }

    float minVal = std::min({ minCoords.x, minCoords.y, minCoords.z });
    minCoords.x = minVal;
    minCoords.y = minVal;
    minCoords.z = minVal;

    // Adding an additional amount of extra space
    float maxVal = std::max({ maxCoords.x, maxCoords.y, maxCoords.z });
    maxCoords.x = maxVal;
    maxCoords.y = maxVal;
    maxCoords.z = maxVal;

    minCoords -= glm::vec3(2.f, 2.f, 2.f);
    maxCoords += glm::vec3(2.f, 2.f, 2.f);

    std::cout << "\nMin-Coord: " << minCoords.x << ", " << minCoords.y << ", " << minCoords.z;
    std::cout << "\nMax-Coord: " << maxCoords.x << ", " << maxCoords.y << ", " << maxCoords.z;

    // Creating Octree
    Voxel boundary(minCoords, glm::abs(maxCoords - minCoords));
    Octree octree(boundary);

    // Adding objects to the octree
    for (int i = 0; i < num_objects; i++)
    {
        octree.insert(prims[i]);
    }

    std::cout << "\nSuccessfully Added Objects to the Octree";

    // Creating a ray pointer to iterate over the array of rays
    Ray** transRays;

    // Getting the rays transformed w.r.t world-frame
    transRays = cam.getTransformedRays();

    // Creating the tracer object and passing it all the objects in the scene and all the camera rays
    Tracer tracer(octree, transRays, IMAGE_WIDTH, IMAGE_HEIGHT);

    // DOING BUCKET RENDERING
    const int divisions = 32;

    // Computing buckets
    glm::vec2 bucketSize(IMAGE_WIDTH / divisions, IMAGE_HEIGHT / divisions);
    glm::vec4 bucketList[divisions * divisions];
    for (int y = 0; y < divisions; y++)
    {
        for (int x = 0; x < divisions; x++)
        {
            // xmin xmax ymin ymax
            bucketList[x + y * divisions] = glm::vec4(bucketSize.x * x, bucketSize.x * (x + 1), bucketSize.y * y, bucketSize.y * (y + 1));
        }
    }

    //// obtain a time-based seed:
    //unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    //std::default_random_engine e(seed);
    //// copy it into a vector and shuffle it
    //std::shuffle(bucketList.begin(), bucketList.end(), e);

    // Creating a thread for image display and detaching it
    std::thread image_thread(
        [&]()
        {
            while (true)
            {
                // Display the rendered bucket
                cv::resize(image, outImg, cv::Size(int(IMAGE_WIDTH / ANTI_ALIASING), int(IMAGE_HEIGHT / ANTI_ALIASING)), 0, 0, CV_INTER_AREA);
                cv::imshow("Image", outImg);
                cv::waitKey(1);
            }
        }
    );

    image_thread.detach(); // Let it run in parallel

    // Parallel Rendering and updating image
    #pragma omp parallel for num_threads(8)
    for (int i = 0; i < divisions * divisions; i++) {
        // Getting color list from the region
        glm::vec3* colors = tracer.colorFromRegion(bucketList[i]);

        // Coloring image using the colors obtained from the rendered region
        for (int y = 0; y < bucketSize.y; y++)
        {
            for (int x = 0; x < bucketSize.x; x++)
            {
                // Coloring the image pixel using the final color values
                // Getting the color value at this x,y
                cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(x + bucketList[i].x, IMAGE_HEIGHT - 1 - y - bucketList[i].z)); // Picking the image point at the bucket offset
                // Altering the color value with gamma-2 correction
                color[0] = sqrt(colors[(int)(x + y * bucketSize.x)].z) * 255.99;
                color[1] = sqrt(colors[(int)(x + y * bucketSize.x)].y) * 255.99;
                color[2] = sqrt(colors[(int)(x + y * bucketSize.x)].x) * 255.99;
                // set a pixel back to the image
                image.at<cv::Vec3b>(cv::Point(x + bucketList[i].x, IMAGE_HEIGHT - 1 - y - bucketList[i].z)) = color;
            }
        }
    }

    std::cout << "\nTime taken: " << (double)(clock() - tStart) / CLOCKS_PER_SEC;

    std::string endTime = NowToString();
    replace(endTime.begin(), endTime.end(), ':', '_');
    replace(endTime.begin(), endTime.end(), '\n', '_');

    cv::imwrite("./Renders/" + startTime + "_to_" + endTime + ".jpg", outImg);

    cv::imshow("Final Render", outImg);
    cv::waitKey();
}
