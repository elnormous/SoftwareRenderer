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

    template <class T>
    inline Vector<T, 3> barycentric(const Vector<T, 2>& a, const Vector<T, 2>& b, const Vector<T, 2>& c, const Vector<T, 2>& p) noexcept
    {
        const Vector<T, 3> side1{c.v[0] - a.v[0], b.v[0] - a.v[0], a.v[0] - p.v[0]};
        const Vector<T, 3> side2{c.v[1] - a.v[1], b.v[1] - a.v[1], a.v[1] - p.v[1]};
        const auto u = side1.cross(side2);

        return (std::abs(u.v[2]) < std::numeric_limits<T>::min()) // degenerate triangle (all three points in a line)
            ? Vector<T, 3>{T(-1), T(1), T(1)}
            : Vector<T, 3>{T(1) - (u.v[0] + u.v[1]) / u.v[2], u.v[1] / u.v[2], u.v[0] / u.v[2]};
    }
}

#endif
