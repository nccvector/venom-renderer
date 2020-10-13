#include <iostream>
#include <algorithm>    // std::shuffle
#include <vector>       // std::vector
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include <thread>

#include "opencv2/opencv.hpp"

#include "Camera.h"
#include "Ray.h"
#include "Tracer.h"
#include "Voxel.h"
#include "OBJLoader.h"

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

    // Creating image canvas
    float ANTI_ALIASING = 1.f; // 2X
    int IMAGE_WIDTH = 640 * ANTI_ALIASING;
    int IMAGE_HEIGHT = 480 * ANTI_ALIASING;    // 16:9
    cv::Mat image(IMAGE_HEIGHT, IMAGE_WIDTH, CV_8UC3, cv::Scalar(0, 0, 0));
    cv::Mat outImg;

    // Creating and preparing camera object
    Camera cam(IMAGE_WIDTH, IMAGE_HEIGHT, 4.f/3.f, glm::radians(45.f));
    cam.rotate(0.f, 0.f, 0.f);

    //cam.translate(glm::vec3(0.f, 2.5f, 3.5f)); // for cornell box
    //cam.translate(glm::vec3(-0.02f, 0.09f, 0.135f)); // for stanford bunny
    //cam.translate(glm::vec3(0.f, 0.5f, 5.f)); // for mit sphere
    //cam.translate(glm::vec3(0.f, 1.f, 4.5f)); // for teapot
    cam.translate(glm::vec3(-0.6f, 0.9f, 7.5f)); // for venom sample scene

    Mesh mesh("./Assets/venom_sample_scene.obj");

    std::vector<Mesh> meshes;
    meshes.push_back(mesh);

    std::cout << "\nSuccessfully Loaded Meshes: " << meshes.size() << "\n";

    // Creating a ray pointer to iterate over the array of rays
    Ray** transRays;

    // Getting the rays transformed w.r.t world-frame
    transRays = cam.getTransformedRays();

    // Creating the tracer object and passing it all the objects in the scene and all the camera rays
    Tracer tracer(meshes, transRays, IMAGE_WIDTH, IMAGE_HEIGHT);

    // DOING BUCKET RENDERING
    const int divisions = 40;

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

    const int samples = 1024;
    // Parallel Rendering and updating image
    for (int s=0; s<samples; s++)
    {
        #pragma omp parallel for num_threads(8)
        for (int i = 0; i < divisions * divisions; i++) {
            // Creating a color array of bucket size in advance (Thanks to Imre Palik for finding a memory leak)
            glm::vec3* colors = new glm::vec3[int(bucketSize.x) * int(bucketSize.y)]; // "new" keyword now gets deleted at the end

            // Filling colors array
            tracer.colorFromRegion(bucketList[i], colors);

            // Coloring image using the colors obtained from the rendered region
            for (int y = 0; y < bucketSize.y; y++)
            {
                for (int x = 0; x < bucketSize.x; x++)
                {
                    // Coloring the image pixel using the final color values
                    // Getting the color value at this x,y
                    cv::Vec3b color = image.at<cv::Vec3b>(cv::Point(x + bucketList[i].x, IMAGE_HEIGHT - 1 - y - bucketList[i].z)); // Picking the image point at the bucket offset
                    // Altering the color value with gamma-2 correction
                    color[0] = (color[0] * float(s) + sqrt(colors[(int)(x + y * bucketSize.x)].z) * 255.99f) / float(s + 1);
                    color[1] = (color[1] * float(s) + sqrt(colors[(int)(x + y * bucketSize.x)].y) * 255.99f) / float(s + 1);
                    color[2] = (color[2] * float(s) + sqrt(colors[(int)(x + y * bucketSize.x)].x) * 255.99f) / float(s + 1);

                    image.at<cv::Vec3b>(cv::Point(x + bucketList[i].x, IMAGE_HEIGHT - 1 - y - bucketList[i].z)) = color;
                }
            }

            // Clearing the memory
            delete[] colors;
        }

        if(s%10 == 0)
            cv::imwrite("./Renders/" + startTime + std::to_string(s) + ".jpg", outImg);

        std::cout << "Render-pass  :" << s << "\n";
    }

    std::cout << "\nTime taken: " << (double)(clock() - tStart) / CLOCKS_PER_SEC;

    std::string endTime = NowToString();
    replace(endTime.begin(), endTime.end(), ':', '_');
    replace(endTime.begin(), endTime.end(), '\n', '_');

    cv::waitKey(10); // Time delay before saving to let the image complete
    cv::imwrite("./Renders/" + startTime + "_to_" + endTime + ".jpg", outImg);
    cv::waitKey();
}
