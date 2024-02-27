#pragma once

#include <array>
#include <limits>
#include "Defs.h"
#undef max
#undef min

struct AABBox
{
    vec3 minPoint{ (std::numeric_limits<double>::max()) };
    vec3 maxPoint{ (std::numeric_limits<double>::min()) };

    inline vec3 center() const { return (minPoint + maxPoint) * 0.5; }
    inline vec3 sizes() const { return maxPoint - minPoint; }

    inline vec3 a() const { return vec3(minPoint.x, minPoint.y, maxPoint.z); }
    inline vec3 b() const { return vec3(maxPoint.x, minPoint.y, maxPoint.z); }
    inline vec3 c() const { return vec3(maxPoint.x, maxPoint.y, maxPoint.z); }
    inline vec3 d() const { return vec3(minPoint.x, maxPoint.y, maxPoint.z); }
    inline vec3 e() const { return vec3(minPoint.x, minPoint.y, minPoint.z); }
    inline vec3 f() const { return vec3(maxPoint.x, minPoint.y, minPoint.z); }
    inline vec3 g() const { return vec3(maxPoint.x, maxPoint.y, minPoint.z); }
    inline vec3 h() const { return vec3(minPoint.x, maxPoint.y, minPoint.z); }

    inline std::array<vec3, 8> verts() const { return { a(), b(), c(), d(), e(), f(), g(), h() }; }
};

struct OBBox
{
    union
    {
        struct { vec3 a, b, c, d, e, f, g, h; };
        std::array<vec3, 8> verts;
    };

    inline AABBox AABB() const
    {
        AABBox aabb = { a,a };

        for (const auto& v : verts) {
            aabb.minPoint = (glm::min)(aabb.minPoint, v);
            aabb.maxPoint = (glm::max)(aabb.maxPoint, v);
        }
        return aabb;
    }
};

inline OBBox operator*(const mat4& transform, const AABBox& aabb)
{
    OBBox bbox;
    bbox.a = transform * vec4(aabb.a(), 1.0);
    bbox.b = transform * vec4(aabb.b(), 1.0);
    bbox.c = transform * vec4(aabb.c(), 1.0);
    bbox.d = transform * vec4(aabb.d(), 1.0);
    bbox.e = transform * vec4(aabb.e(), 1.0);
    bbox.f = transform * vec4(aabb.f(), 1.0);
    bbox.g = transform * vec4(aabb.g(), 1.0);
    bbox.h = transform * vec4(aabb.h(), 1.0);
    return bbox;
}