#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include "Ray.h"

class Camera {
    glm::vec4 position, forward, right, up;
    float fov, aspectRatio, nearPlaneDistance, farPlaneDistance;
    glm::mat4 transform;

    // 2D array of rays
    Ray **rays;

    int IMAGE_WIDTH;
    int IMAGE_HEIGHT;

public:
    Camera(int IMAGE_WIDTH, int IMAGE_HEIGHT, float aspectRatio, float fov) {
        this->position = glm::vec4(0.f, 0.f, 0.f, 1.f);
        this->forward = glm::vec4(0.f, 0.f, -1.f, 1.f);
        this->right = glm::vec4(1.f, 0.f, 0.f, 1.f);
        this->up = glm::vec4(0.f, 1.f, 0.f, 1.f);
        this->fov = fov;
        this->aspectRatio = aspectRatio;        // 16:9
        this->nearPlaneDistance = 0.01f;
        this->farPlaneDistance = 1000.f;
        this->transform = glm::mat4(1.f);
        this->IMAGE_WIDTH = IMAGE_WIDTH;
        this->IMAGE_HEIGHT = IMAGE_HEIGHT;

        // Declaring rays array
        this->rays = new Ray *[this->IMAGE_HEIGHT];

        // Calculating the pre-requisites for making rays
        float hw = this->nearPlaneDistance * glm::tan(this->fov / 2.f);
        float vw = hw / this->aspectRatio;

        float hstep = (hw * 2) / this->IMAGE_WIDTH;
        float vstep = (vw * 2) / this->IMAGE_HEIGHT;

        // Initializing Rays
        float vOffset = -vw;
        for (int y = 0; y < this->IMAGE_HEIGHT; y++) {
            this->rays[y] = new Ray[IMAGE_WIDTH];
            float hOffset = -hw;
            for (int x = 0; x < this->IMAGE_WIDTH; x++) {
                // Getting the point on screen in camera space (x->right, y->up, z->negative-z into the screen)
                // The nearplane distance is taken negative so as to shoot rays towards the forward vector of the camera (-ve z by default)
                glm::vec4 screenPoint(hOffset, vOffset, -this->nearPlaneDistance, 1.f);

                Ray ray;
                ray.origin = this->position;
                ray.direction = glm::normalize(screenPoint - this->position);
                ray.max_distance = 1000.f;
                // Assert if the 4th element in the direction vector is non-zero

                this->rays[y][x] = ray;

                // Updating horizontal offset
                hOffset += hstep;
            }

            // Updating vertical offset
            vOffset += vstep;
        }
    }

    Ray **getRays() {
        // Returns the 2d array of rays
        return this->rays;
    }

    Ray **getTransformedRays() {
        // Returns the 2d array of transformed rays
        // Get the rays w.r.t to world origin (multiply with the model matrix)
        Ray **transformedRays = new Ray *[this->IMAGE_HEIGHT];

        // Transforming Rays
        for (int y = 0; y < this->IMAGE_HEIGHT; y++) {
            transformedRays[y] = new Ray[IMAGE_WIDTH];
            for (int x = 0; x < this->IMAGE_WIDTH; x++) {
                Ray transformedRay;
                transformedRay.origin = glm::vec3(this->transform * glm::vec4(this->rays[y][x].origin, 1.f));
                transformedRay.direction = glm::vec3(this->transform * glm::vec4(this->rays[y][x].direction, 0.f));

                transformedRays[y][x] = transformedRay;
            }
        }

        return transformedRays;
    }

    void translate(glm::vec3 translation) {
        // Get the new transform matrix after incorporating this translation
        this->transform = glm::translate(this->transform, translation);
    }

    void rotate(float yaw, float pitch, float roll) {
        // Get the new transform matrix after incorporating these rotations
        this->transform = glm::rotate(this->transform, glm::radians(yaw), glm::vec3(0.f, 1.f, 0.f));
        this->transform = glm::rotate(this->transform, glm::radians(pitch), glm::vec3(1.f, 0.f, 0.f));
        this->transform = glm::rotate(this->transform, glm::radians(roll), glm::vec3(0.f, 0.f, 1.f));
    }
};