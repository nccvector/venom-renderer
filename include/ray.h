#pragma once

#include <glm.hpp>

struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;
    float max_distance;
};