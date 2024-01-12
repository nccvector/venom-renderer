#pragma once

#include <glm.hpp>

//#include "Primitives.h"

class Voxel {
public:
    glm::vec3 coordinate;
    glm::vec3 size;

    Voxel() {

    }

    Voxel(glm::vec3 coord, glm::vec3 size) {
        this->coordinate = coord;
        this->size = size;
    }

    //bool contains(Primitive* prim)
    //{
    //	// check if the primitive touches this voxel
    //	return prim->touches(this);
    //}

    bool intersects(Ray ray) {
        float tmin = (this->coordinate.x - ray.origin.x) / ray.direction.x;
        float tmax = (this->coordinate.x + this->size.x - ray.origin.x) / ray.direction.x;

        if (tmin > tmax) std::swap(tmin, tmax);

        float tymin = (this->coordinate.y - ray.origin.y) / ray.direction.y;
        float tymax = (this->coordinate.y + this->size.y - ray.origin.y) / ray.direction.y;

        if (tymin > tymax) std::swap(tymin, tymax);

        if ((tmin > tymax) || (tymin > tmax))
            return false;

        if (tymin > tmin)
            tmin = tymin;

        if (tymax < tmax)
            tmax = tymax;

        float tzmin = (this->coordinate.z - ray.origin.z) / ray.direction.z;
        float tzmax = (this->coordinate.z + this->size.z - ray.origin.z) / ray.direction.z;

        if (tzmin > tzmax) std::swap(tzmin, tzmax);

        if ((tmin > tzmax) || (tzmin > tmax))
            return false;

        if (tzmin > tmin)
            tmin = tzmin;

        if (tzmax < tmax)
            tmax = tzmax;

        return true;
    }
};