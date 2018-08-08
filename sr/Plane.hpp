//
//  SoftwareRenderer
//

#ifndef PLANE_H
#define PLANE_H

#include <cmath>
#include "Vector.hpp"
#include "MathUtils.hpp"

namespace sr
{
    class Vector4;

    class Plane
    {
    public:
        float a = 0.0F;
        float b = 0.0F;
        float c = 0.0F;
        float d = 0.0F;

        Plane()
        {
        }

        Plane(float initA, float initB, float initC, float initD):
            a(initA), b(initB), c(initC), d(initD)
        {
        }

        void flip()
        {
            a = -a;
            b = -b;
            c = -c;
            d = -d;
        }

        float dot(const Vector4F& vec) const
        {
            return a * vec.v[0] + b * vec.v[1] + c * vec.v[2] + d;
        }

        void normalize()
        {
            float n = a * a + b * b + c * c + d * d;
            if (n == 1.0F) // already normalized
                return;

            n = sqrtf(n);
            if (n < EPSILON) // too close to zero
                return;

            n = 1.0F / n;
            a *= n;
            b *= n;
            c *= n;
            d *= n;
        }

        inline bool operator==(const Plane& plane) const
        {
            return a == plane.a && b == plane.b && c == plane.c && d == plane.d;
        }

        inline bool operator!=(const Plane& plane) const
        {
            return a != plane.a || b != plane.b || c != plane.c || d != plane.d;
        }

        static inline bool makeFrustumPlane(float a, float b, float c, float d, Plane& plane)
        {
            float n = sqrtf(a * a + b * b + c * c);
            if (n < EPSILON)
            {
                return false;
            }

            n = 1.0F / n;
            plane = Plane(a * n, b * n, c * n, d * n);

            return true;
        }
    };
}

#endif
