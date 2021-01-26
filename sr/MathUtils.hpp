//
//  SoftwareRenderer
//

#ifndef MATHUTILS_H
#define MATHUTILS_H

#include <cstdint>
#include "Vector.hpp"

namespace sr
{
    template<typename T>
    constexpr T clamp(T x, T lo, T hi) noexcept
    {
        return x < lo ? lo : x > hi ? hi : x;
    }

    inline Vector3F barycentric(Vector2F a, Vector2F b, Vector2F c, Vector2F p) noexcept
    {
        Vector3F s[2];

        s[0].v[0] = c.v[0] - a.v[0];
        s[0].v[1] = b.v[0] - a.v[0];
        s[0].v[2] = a.v[0] - p.v[0];

        s[1].v[0] = c.v[1] - a.v[1];
        s[1].v[1] = b.v[1] - a.v[1];
        s[1].v[2] = a.v[1] - p.v[1];

        const auto u = s[0].cross(s[1]);

        if (std::abs(u.v[2]) < std::numeric_limits<float>::min()) // degenerate triangle (all three points in a line)
            return Vector3F(-1.0F, 1.0F, 1.0F);

        return Vector3F(1.0F - (u.v[0] + u.v[1]) / u.v[2], u.v[1] / u.v[2], u.v[0] / u.v[2]);
    }
}

#endif
