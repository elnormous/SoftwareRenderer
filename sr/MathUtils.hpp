//
//  SoftwareRenderer
//

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cstdint>
#include "Vector.hpp"

namespace sr
{
    constexpr float lerp(float v0, float v1, float t) noexcept
    {
        return (1.0F - t) * v0 + t * v1;
    }

    constexpr float smoothStep(float a, float b, float t) noexcept
    {
        return lerp(a, b, t * t * (3 - 2 * t));
    }

    constexpr float degToRad(float x) noexcept
    {
        return x * 0.0174532925F;
    }

    constexpr float radToDeg(float x) noexcept
    {
        return x * 57.29577951F;
    }

    template<typename T>
    constexpr T clamp(T x, T lo, T hi) noexcept
    {
        return (x < lo) ? lo : ((x > hi) ? hi : x);
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

        const Vector3F u = s[0].cross(s[1]);

        if (std::abs(u.v[2]) < std::numeric_limits<float>::min()) // degenerate triangle (all three points in a line)
            return Vector3F(-1.0F, 1.0F, 1.0F);

        return Vector3F(1.0F - (u.v[0] + u.v[1]) / u.v[2], u.v[1] / u.v[2], u.v[0] / u.v[2]);
    }
}

#endif
