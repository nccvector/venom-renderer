#pragma once

#include <glm.hpp>

#include "Face.h"

struct HitInfo {
    // Logical properties
    bool hit;

    // Geometric properties
    float hitDistance;
    glm::vec3 normal;

    glm::vec3 hitPoint;
    glm::vec2 texcoord;

    // Material properties
    Face *face;
};