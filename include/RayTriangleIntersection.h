#pragma once

#include<glm.hpp>

#include "Ray.h"
#include "Face.h"

float rayTriangleIntersect(
        Ray ray,
        Face *face) {
    glm::vec3 v0v1 = face->vertices[1] - face->vertices[0];
    glm::vec3 v0v2 = face->vertices[2] - face->vertices[0];
    glm::vec3 pvec = glm::cross(glm::vec3(ray.direction), v0v2);
    float det = glm::dot(v0v1, pvec);

    // if the determinant is negative the triangle is backfacing
    // if the determinant is close to 0, the ray misses the triangle
    if (det < 0.00000000001) return -1.f;

    // ray and triangle are parallel if det is close to 0
    if (fabs(det) < 0.00000000001) return -1.f;

    float invDet = 1 / det;

    glm::vec3 tvec = glm::vec3(ray.origin) - face->vertices[0];
    float u = glm::dot(tvec, pvec) * invDet;
    if (u < 0 || u > 1) return -1.f;

    glm::vec3 qvec = glm::cross(tvec, v0v1);
    float v = glm::dot(glm::vec3(ray.direction), qvec) * invDet;
    if (v < 0 || u + v > 1) return -1.f;

    float t = glm::dot(v0v2, qvec) * invDet;

    return t;
}