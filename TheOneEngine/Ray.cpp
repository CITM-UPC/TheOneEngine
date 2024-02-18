#include "Ray.h"
#include "BBox.hpp"


Ray::Ray() : Origin(0.0f), Direction(0.0f) {}
Ray::Ray(const glm::vec3& origin, const glm::vec3& direction) : Origin(origin), Direction(direction) {}

bool Ray::Intersects(const AABBox& bb, float& t) const
{
    t = 0.0f;

    // Check for ray origin being inside the bb
    if (bb.min.x <= Origin.x && Origin.x <= bb.max.x &&
        bb.min.y <= Origin.y && Origin.y <= bb.max.y &&
        bb.min.z <= Origin.z && Origin.z <= bb.max.z)
    {
        return true;
    }

    // Compute intersection distances
    float tmin = (bb.min.x - Origin.x) / Direction.x;
    float tmax = (bb.max.x - Origin.x) / Direction.x;

    float tymin = (bb.min.y - Origin.y) / Direction.y;
    float tymax = (bb.max.y - Origin.y) / Direction.y;

    if ((tmin > tymax) || (tymin > tmax)) return false;
    if (tymin > tmin) tmin = tymin;
    if (tymax < tmax) tmax = tymax;

    float tzmin = (bb.min.z - Origin.z) / Direction.z;
    float tzmax = (bb.max.z - Origin.z) / Direction.z;

    if ((tmin > tzmax) || (tzmin > tmax)) return false;
    if (tzmin > tmin) tmin = tzmin;
    if (tzmax < tmax) tmax = tzmax;

    t = tmin;
    return true;
}

bool Ray::Intersects(const AABBox& bb) const
{
    float distance;
    return Intersects(bb, distance);
}

bool Ray::IntersectsTriangle(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c, float& t) const
{
    const glm::vec3 E1 = b - a;
    const glm::vec3 E2 = c - a;
    const glm::vec3 N = cross(E1, E2);
    const float det = -glm::dot(Direction, N);
    const float invdet = 1.f / det;
    const glm::vec3 AO = Origin - a;
    const glm::vec3 DAO = glm::cross(AO, Direction);
    const float u = glm::dot(E2, DAO) * invdet;
    const float v = -glm::dot(E1, DAO) * invdet;
    t = glm::dot(AO, N) * invdet;
    return (det >= 1e-6f && t >= 0.0f && u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f);
}