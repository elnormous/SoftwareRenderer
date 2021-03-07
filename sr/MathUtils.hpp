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

    inline Vector<float, 3> barycentric(Vector<float, 2> a, Vector<float, 2> b, Vector<float, 2> c, Vector<float, 2> p) noexcept
    {
        const Vector<float, 3> side1{c.v[0] - a.v[0], b.v[0] - a.v[0], a.v[0] - p.v[0]};
        const Vector<float, 3> side2{c.v[1] - a.v[1], b.v[1] - a.v[1], a.v[1] - p.v[1]};
        const auto u = side1.cross(side2);

        if (std::abs(u.v[2]) < std::numeric_limits<float>::min()) // degenerate triangle (all three points in a line)
            return Vector<float, 3>{-1.0F, 1.0F, 1.0F};

        return Vector<float, 3>{1.0F - (u.v[0] + u.v[1]) / u.v[2], u.v[1] / u.v[2], u.v[0] / u.v[2]};
    }
}

#endif
