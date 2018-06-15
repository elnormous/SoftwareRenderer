//
//  SoftwareRenderer
//

#pragma once

#include <cstdint>
#include "Vector.hpp"

namespace sr
{
    inline float lerp(float v0, float v1, float t)
    {
        return (1.0F - t) * v0 + t * v1;
    }

    inline float smoothStep(float a, float b, float t)
    {
        float remapSmoothStep = t * t * (3 - 2 * t);
        return lerp(a, b, remapSmoothStep);
    }

    inline bool isPOT(uint32_t x)
    {
        return (x != 0) && (((x - 1) & x) == 0);
    }

    inline uint32_t nextPOT(uint32_t x)
    {
        x = x - 1;
        x = x | (x >> 1);
        x = x | (x >> 2);
        x = x | (x >> 4);
        x = x | (x >> 8);
        x = x | (x >>16);
        return x + 1;
    }

    template<typename T> inline int sgn(T val)
    {
        return (T(0) < val) - (val < T(0));
    }

    inline float degToRad(float x)
    {
        return x * 0.0174532925F;
    }

    inline float radToDeg(float x)
    {
        return x * 57.29577951F;
    }

    template<typename T>
    inline T clamp(T x, T lo, T hi)
    {
        return (x < lo) ? lo : ((x > hi) ? hi : x);
    }

    static const uint64_t INITIAL_FNV = 2166136261U;
    static const uint64_t FNV_MULTIPLE = 16777619;

    // Fowler / Noll / Vo (FNV) hash
    inline uint64_t fnvHash(uint64_t s)
    {
        uint64_t hash = INITIAL_FNV;
        for (uint64_t i = 0; i < sizeof(uint64_t); i++)
        {
            hash = hash ^ (reinterpret_cast<uint8_t*>(&s)[i]); // xor the low 8 bits
            hash = hash * FNV_MULTIPLE; // multiply by the magic number
        }
        return hash;
    }

    inline Vector3F barycentric(Vector2F a, Vector2F b, Vector2F c, Vector2F p)
    {
        Vector3F s[2];

        s[0].v[0] = c.v[0] - a.v[0];
        s[0].v[1] = b.v[0] - a.v[0];
        s[0].v[2] = a.v[0] - p.v[0];

        s[1].v[0] = c.v[1] - a.v[1];
        s[1].v[1] = b.v[1] - a.v[1];
        s[1].v[2] = a.v[1] - p.v[1];

        Vector3F u = Vector3F::cross(s[0], s[1]);

        if (std::abs(u.v[2]) < EPSILON) // degenerate triangle (all three points in a line)
            return Vector3F(-1.0F, 1.0F, 1.0F);

        return Vector3F(1.0F - (u.v[0] + u.v[1]) / u.v[2], u.v[1] / u.v[2], u.v[0] / u.v[2]);
    }
}
