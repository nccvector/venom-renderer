#include <iostream>
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock

#include "Camera.h"
#include "Ray.h"
#include "Tracer.h"
#include "Voxel.h"
#include "OBJLoader.h"

std::default_random_engine generator;
std::uniform_real_distribution<float> distribution{0, 1};

std::string NowToString() {
    std::chrono::system_clock::time_point p = std::chrono::system_clock::now();
    time_t t = std::chrono::system_clock::to_time_t(p);
    char str[26];
    ctime_s(str, sizeof str, &t);
    return std::string(str);
}

int main() {
    std::string startTime = NowToString();
    replace(startTime.begin(), startTime.end(), ':', '_');
    replace(startTime.begin(), startTime.end(), '\n', '_');

    clock_t tStart = clock();

    // Creating image canvas
    float ANTI_ALIASING = 1.0f; // 2X
    int IMAGE_WIDTH = 640 * ANTI_ALIASING;
    int IMAGE_HEIGHT = 480 * ANTI_ALIASING;    // 16:9

    // Creating and preparing camera object
    Camera cam(IMAGE_WIDTH, IMAGE_HEIGHT, float(IMAGE_WIDTH) / float(IMAGE_HEIGHT), glm::radians(45.f));
    cam.rotate(0.f, -10.f, 0.f);

    //cam.translate(glm::vec3(0.f, 2.5f, 3.5f)); // for cornell box
    //cam.translate(glm::vec3(-0.02f, 0.09f, 0.135f)); // for stanford bunny
    //cam.translate(glm::vec3(0.f, 0.5f, 5.f)); // for mit sphere
    //cam.translate(glm::vec3(0.f, 1.f, 4.5f)); // for teapot
    cam.translate(glm::vec3(-0.5f, 1.f, 12.f)); // for venom sample scene

    // Whole scene is a single mesh with many mant faces
    Mesh scene("./Assets/sample-scenes/venom_sample_scene.obj");
    std::cout << "\nSuccessfully Loaded Scene " << "\n";

    // Creating a ray pointer to iterate over the array of rays
    Ray **transRays;

    // Getting the rays transformed w.r.t world-frame
    transRays = cam.getTransformedRays();

    // Creating the tracer object and passing it all the objects in the scene and all the camera rays
    Tracer tracer(scene, transRays, IMAGE_WIDTH, IMAGE_HEIGHT);

    const int samples = 1000;
    // Parallel Rendering and updating image
    for (int s = 0; s < samples; s++) {
        for (int i = 0; i < divisions * divisions; i++) {
            // Creating a color array of bucket size in advance (Thanks to Imre Palik for finding a memory leak)
            glm::vec3 *colors = new glm::vec3[int(bucketSize.x) *
                                              int(bucketSize.y)]; // "new" keyword now gets deleted at the end

            // Filling colors array
            tracer.colorFromRegion({0, 0, IMAGE_WIDTH, IMAGE_HEIGHT}, colors);
        }

        std::cout << "Render-pass  :" << s << "\n";
    }

    std::cout << "\nTime taken: " << (double) (clock() - tStart) / CLOCKS_PER_SEC;

    std::string endTime = NowToString();
    replace(endTime.begin(), endTime.end(), ':', '_');
    replace(endTime.begin(), endTime.end(), '\n', '_');

    cv::waitKey(10); // Time delay before saving to let the image complete
    cv::imwrite("./Renders/" + startTime + "_to_" + endTime + ".jpg", outImg);
    cv::waitKey();
}
