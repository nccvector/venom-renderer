#pragma once

#include <cmath>
#include<glm.hpp>

#include "Voxel.h"
#include "Ray.h"
#include "Face.h"
#include "HitInfo.h"
#include "TriangleBoxIntersection.h"
#include "RayTriangleIntersection.h"

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

inline void findMinMax(float x0, float x1, float x2, float &min, float &max) {
    min = max = x0;
    if (x1 < min)
        min = x1;
    if (x1 > max)
        max = x1;
    if (x2 < min)
        min = x2;
    if (x2 > max)
        max = x2;
}

inline bool planeBoxOverlap(glm::vec3 normal, glm::vec3 vert, glm::vec3 maxbox) {
    glm::vec3 vmin, vmax;
    float v;
    for (size_t q = 0; q < 3; q++) {
        v = vert[q];
        if (normal[q] > 0.0f) {
            vmin[q] = -maxbox[q] - v;
            vmax[q] = maxbox[q] - v;
        } else {
            vmin[q] = maxbox[q] - v;
            vmax[q] = -maxbox[q] - v;
        }
    }
    if (glm::dot(normal, vmin) > 0.0f)
        return false;
    if (glm::dot(normal, vmax) >= 0.0f)
        return true;

    return false;
}

/*======================== X-tests ========================*/

inline bool axisTestX01(float a, float b, float fa, float fb, const glm::vec3 &v0,
                        const glm::vec3 &v2, const glm::vec3 &boxhalfsize, float &rad, float &min,
                        float &max, float &p0, float &p2) {
    p0 = a * v0.y - b * v0.z;
    p2 = a * v2.y - b * v2.z;
    if (p0 < p2) {
        min = p0;
        max = p2;
    } else {
        min = p2;
        max = p0;
    }
    rad = fa * boxhalfsize.y + fb * boxhalfsize.z;
    if (min > rad || max < -rad)
        return false;
    return true;
}

inline bool axisTestX2(float a, float b, float fa, float fb, const glm::vec3 &v0,
                       const glm::vec3 &v1, const glm::vec3 &boxhalfsize, float &rad, float &min,
                       float &max, float &p0, float &p1) {
    p0 = a * v0.y - b * v0.z;
    p1 = a * v1.y - b * v1.z;
    if (p0 < p1) {
        min = p0;
        max = p1;
    } else {
        min = p1;
        max = p0;
    }
    rad = fa * boxhalfsize.y + fb * boxhalfsize.z;
    if (min > rad || max < -rad)
        return false;
    return true;
}

/*======================== Y-tests ========================*/

inline bool axisTestY02(float a, float b, float fa, float fb, const glm::vec3 &v0,
                        const glm::vec3 &v2, const glm::vec3 &boxhalfsize, float &rad, float &min,
                        float &max, float &p0, float &p2) {
    p0 = -a * v0.x + b * v0.z;
    p2 = -a * v2.x + b * v2.z;
    if (p0 < p2) {
        min = p0;
        max = p2;
    } else {
        min = p2;
        max = p0;
    }
    rad = fa * boxhalfsize.x + fb * boxhalfsize.z;
    if (min > rad || max < -rad)
        return false;
    return true;
}

inline bool axisTestY1(float a, float b, float fa, float fb, const glm::vec3 &v0,
                       const glm::vec3 &v1, const glm::vec3 &boxhalfsize, float &rad, float &min,
                       float &max, float &p0, float &p1) {
    p0 = -a * v0.x + b * v0.z;
    p1 = -a * v1.x + b * v1.z;
    if (p0 < p1) {
        min = p0;
        max = p1;
    } else {
        min = p1;
        max = p0;
    }
    rad = fa * boxhalfsize.x + fb * boxhalfsize.z;
    if (min > rad || max < -rad)
        return false;
    return true;
}

/*======================== Z-tests ========================*/
inline bool axisTestZ12(float a, float b, float fa, float fb, const glm::vec3 &v1,
                        const glm::vec3 &v2, const glm::vec3 &boxhalfsize, float &rad, float &min,
                        float &max, float &p1, float &p2) {
    p1 = a * v1.x - b * v1.y;
    p2 = a * v2.x - b * v2.y;
    if (p1 < p2) {
        min = p1;
        max = p2;
    } else {
        min = p2;
        max = p1;
    }
    rad = fa * boxhalfsize.x + fb * boxhalfsize.y;
    if (min > rad || max < -rad)
        return false;
    return true;
}

inline bool axisTestZ0(float a, float b, float fa, float fb, const glm::vec3 &v0,
                       const glm::vec3 &v1, const glm::vec3 &boxhalfsize, float &rad, float &min,
                       float &max, float &p0, float &p1) {
    p0 = a * v0.x - b * v0.y;
    p1 = a * v1.x - b * v1.y;
    if (p0 < p1) {
        min = p0;
        max = p1;
    } else {
        min = p1;
        max = p0;
    }
    rad = fa * boxhalfsize.x + fb * boxhalfsize.y;
    if (min > rad || max < -rad)
        return false;
    return true;
}

bool triBoxOverlap(glm::vec3 boxcenter, glm::vec3 boxhalfsize, glm::vec3 tv0, glm::vec3 tv1,
                   glm::vec3 tv2) {
    /*    use separating axis theorem to test overlap between triangle and box */
    /*    need to test for overlap in these directions: */
    /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
    /*       we do not even need to test these) */
    /*    2) normal of the triangle */
    /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
    /*       this gives 3x3=9 more tests */
    glm::vec3 v0, v1, v2;
    float min, max, p0, p1, p2, rad, fex, fey, fez;
    glm::vec3 normal, e0, e1, e2;

    /* This is the fastest branch on Sun */
    /* move everything so that the boxcenter is in (0,0,0) */
    v0 = tv0 - boxcenter;
    v1 = tv1 - boxcenter;
    v2 = tv2 - boxcenter;

    /* compute triangle edges */
    e0 = v1 - v0;
    e1 = v2 - v1;
    e2 = v0 - v2;

    /* Bullet 3:  */
    /*  test the 9 tests first (this was faster) */
    fex = fabsf(e0.x);
    fey = fabsf(e0.y);
    fez = fabsf(e0.z);

    if (!axisTestX01(e0.z, e0.y, fez, fey, v0, v2, boxhalfsize, rad, min, max, p0, p2))
        return false;
    if (!axisTestY02(e0.z, e0.x, fez, fex, v0, v2, boxhalfsize, rad, min, max, p0, p2))
        return false;
    if (!axisTestZ12(e0.y, e0.x, fey, fex, v1, v2, boxhalfsize, rad, min, max, p1, p2))
        return false;

    fex = fabsf(e1.x);
    fey = fabsf(e1.y);
    fez = fabsf(e1.z);

    if (!axisTestX01(e1.z, e1.y, fez, fey, v0, v2, boxhalfsize, rad, min, max, p0, p2))
        return false;
    if (!axisTestY02(e1.z, e1.x, fez, fex, v0, v2, boxhalfsize, rad, min, max, p0, p2))
        return false;
    if (!axisTestZ0(e1.y, e1.x, fey, fex, v0, v1, boxhalfsize, rad, min, max, p0, p1))
        return false;

    fex = fabsf(e2.x);
    fey = fabsf(e2.y);
    fez = fabsf(e2.z);
    if (!axisTestX2(e2.z, e2.y, fez, fey, v0, v1, boxhalfsize, rad, min, max, p0, p1))
        return false;
    if (!axisTestY1(e2.z, e2.x, fez, fex, v0, v1, boxhalfsize, rad, min, max, p0, p1))
        return false;
    if (!axisTestZ12(e2.y, e2.x, fey, fex, v1, v2, boxhalfsize, rad, min, max, p1, p2))
        return false;

    /* Bullet 1: */
    /*  first test overlap in the {x,y,z}-directions */
    /*  find min, max of the triangle each direction, and test for overlap in */
    /*  that direction -- this is equivalent to testing a minimal AABB around */
    /*  the triangle against the AABB */

    /* test in X-direction */
    findMinMax(v0.x, v1.x, v2.x, min, max);
    if (min > boxhalfsize.x || max < -boxhalfsize.x)
        return false;

    /* test in Y-direction */
    findMinMax(v0.y, v1.y, v2.y, min, max);
    if (min > boxhalfsize.y || max < -boxhalfsize.y)
        return false;

    /* test in Z-direction */
    findMinMax(v0.z, v1.z, v2.z, min, max);
    if (min > boxhalfsize.z || max < -boxhalfsize.z)
        return false;

    /* Bullet 2: */
    /*  test if the box intersects the plane of the triangle */
    /*  compute plane equation of triangle: normal*x+d=0 */
    normal = glm::cross(e0, e1);
    if (!planeBoxOverlap(normal, v0, boxhalfsize))
        return false;

    return true; /* box and triangle overlaps */
}

class Octree {
    Voxel boundary;
    int voxel_capacity;
    std::vector<Face *> face_list;
    bool divided = false;

    Octree *bottomNorthWest;
    Octree *bottomNorthEast;
    Octree *bottomSouthWest;
    Octree *bottomSouthEast;
    Octree *topNorthWest;
    Octree *topNorthEast;
    Octree *topSouthWest;
    Octree *topSouthEast;

    int depth = 0;
    int max_depth = 1000;

public:
    Octree() {

    }

    // Constructor
    Octree(Voxel boundary, int voxel_capacity = 1, int depth = 0) {
        this->boundary = boundary;
        this->voxel_capacity = voxel_capacity;
        this->depth = depth;
    }

    void subdivide() {
        // Bottom Voxels
        Voxel bottomNorthWestBound(this->boundary.coordinate,
                                   this->boundary.size / 2.f);
        Voxel bottomNorthEastBound(this->boundary.coordinate + glm::vec3(this->boundary.size.x / 2, 0.f, 0.f),
                                   this->boundary.size / 2.f);
        Voxel bottomSouthWestBound(this->boundary.coordinate + glm::vec3(0.f, 0.f, this->boundary.size.z / 2),
                                   this->boundary.size / 2.f);
        Voxel bottomSouthEastBound(
                this->boundary.coordinate + glm::vec3(this->boundary.size.x / 2, 0.f, this->boundary.size.z / 2),
                this->boundary.size / 2.f);

        // Top Voxels
        Voxel topNorthWestBound(this->boundary.coordinate + glm::vec3(0.f, this->boundary.size.y / 2, 0.f),
                                this->boundary.size / 2.f);
        Voxel topNorthEastBound(
                this->boundary.coordinate + glm::vec3(this->boundary.size.x / 2, this->boundary.size.y / 2, 0.f),
                this->boundary.size / 2.f);
        Voxel topSouthWestBound(
                this->boundary.coordinate + glm::vec3(0.f, this->boundary.size.y / 2, this->boundary.size.z / 2),
                this->boundary.size / 2.f);
        Voxel topSouthEastBound(this->boundary.coordinate +
                                glm::vec3(this->boundary.size.x / 2, this->boundary.size.y / 2,
                                          this->boundary.size.z / 2),
                                this->boundary.size / 2.f);

        // Making Quadtrees
        this->bottomNorthWest = new Octree(bottomNorthWestBound, this->voxel_capacity, this->depth + 1);
        this->bottomNorthEast = new Octree(bottomNorthEastBound, this->voxel_capacity, this->depth + 1);
        this->bottomSouthWest = new Octree(bottomSouthWestBound, this->voxel_capacity, this->depth + 1);
        this->bottomSouthEast = new Octree(bottomSouthEastBound, this->voxel_capacity, this->depth + 1);

        this->topNorthWest = new Octree(topNorthWestBound, this->voxel_capacity, this->depth + 1);
        this->topNorthEast = new Octree(topNorthEastBound, this->voxel_capacity, this->depth + 1);
        this->topSouthWest = new Octree(topSouthWestBound, this->voxel_capacity, this->depth + 1);
        this->topSouthEast = new Octree(topSouthEastBound, this->voxel_capacity, this->depth + 1);

        // Set subdivided true
        this->divided = true;
    }

    void insert(Face *face) {
        if (!triBoxOverlap(
                // Box center
                glm::vec3(this->boundary.coordinate.x + this->boundary.size.x / 2,
                          this->boundary.coordinate.y + this->boundary.size.y / 2,
                          this->boundary.coordinate.z + this->boundary.size.z / 2),

                // Box half-size
                glm::vec3(this->boundary.size.x / 2,
                          this->boundary.size.y / 2,
                          this->boundary.size.z / 2),

                // Triangle vertices
                face->vertices[0], face->vertices[1], face->vertices[2]
        )) {
            return;
        }

        if (face_list.size() < voxel_capacity || this->depth > this->max_depth) {
            // Ignoring the voxel capacity if the min size is reached
            this->face_list.push_back(face);
        } else {
            // Divide if not divided
            if (!this->divided) {
                this->subdivide();
            }

            // Try inserting into all the child octrees
            this->bottomNorthWest->insert(face);
            this->bottomNorthEast->insert(face);
            this->bottomSouthWest->insert(face);
            this->bottomSouthEast->insert(face);

            this->topNorthWest->insert(face);
            this->topNorthEast->insert(face);
            this->topSouthWest->insert(face);
            this->topSouthEast->insert(face);
        }
    }

    void query(Ray ray, HitInfo *hitInfo) {

        // Lets just get out if ray does not intersects this boundary
        if (!this->boundary.intersects(ray)) {
            return;
        }

        // Check for intersection with faces inside this voxel
        for (int i = 0; i < this->face_list.size(); i++) {
            float t = rayTriangleIntersect(ray, face_list[i]);

            if (t > 0.f && t < hitInfo->hitDistance) {
                hitInfo->hit = true;
                hitInfo->face = face_list[i];
                hitInfo->hitDistance = t;
            }
        }

        // Adding the objects of children too if they exist
        if (this->divided) {
            this->bottomNorthWest->query(ray, hitInfo);
            this->bottomNorthEast->query(ray, hitInfo);
            this->bottomSouthWest->query(ray, hitInfo);
            this->bottomSouthEast->query(ray, hitInfo);

            this->topNorthWest->query(ray, hitInfo);
            this->topNorthEast->query(ray, hitInfo);
            this->topSouthWest->query(ray, hitInfo);
            this->topSouthEast->query(ray, hitInfo);
        }

        // Returning the filled list
        return;
    }
};