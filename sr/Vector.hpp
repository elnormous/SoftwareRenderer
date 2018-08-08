//
//  SoftwareRenderer
//

#ifndef VECTOR_H
#define VECTOR_H

#include <cstddef>
#include <cmath>
#include <algorithm>
#include <iterator>
#include "Constants.hpp"

namespace sr
{
    template<size_t N, typename T>
    class Vector
    {
    public:
        T v[N];

        Vector()
        {
            std::fill(std::begin(v), std::end(v), static_cast<T>(0.0));
        }

        Vector(T x)
        {
            static_assert(N == 1, "Incorrect parameter count");
            v[0] = x;
        }

        Vector(T x, T y)
        {
            static_assert(N == 2, "Incorrect parameter count");
            v[0] = x;
            v[1] = y;
        }

        Vector(T x, T y, T z)
        {
            static_assert(N == 3, "Incorrect parameter count");
            v[0] = x;
            v[1] = y;
            v[2] = z;
        }

        Vector(T x, T y, T z, T w)
        {
            static_assert(N == 4, "Incorrect parameter count");
            v[0] = x;
            v[1] = y;
            v[2] = z;
            v[3] = w;
        }

        inline Vector<N, T> operator+(const Vector<N, T>& vec) const
        {
            Vector<N, T> result = *this;
            for (size_t i = 0; i < N; ++i) result.v[i] += vec.v[i];
            return result;
        }

        inline Vector<N, T>& operator+=(const Vector<N, T>& vec)
        {
            for (size_t i = 0; i < N; ++i) v[i] += vec.v[i];
            return *this;
        }

        inline Vector<N, T> operator-(const Vector<N, T>& vec) const
        {
            Vector<N, T> result = *this;
            for (size_t i = 0; i < N; ++i) result.v[i] -= vec.v[i];
            return result;
        }

        inline Vector<N, T>& operator-=(const Vector<N, T>& vec)
        {
            for (size_t i = 0; i < N; ++i) v[i] -= vec.v[i];
            return *this;
        }

        inline Vector<N, T> operator-() const
        {
            Vector<N, T> result = *this;
            for (size_t i = 0; i < N; ++i) result.v[i] = -result.v[i];
            return result;
        }

        inline Vector<N, T> operator*(T scalar) const
        {
            Vector<N, T> result = *this;
            for (size_t i = 0; i < N; ++i) result.v[i] *= scalar;
            return result;
        }

        inline Vector<N, T>& operator*=(T scalar)
        {
            for (size_t i = 0; i < N; ++i) v[i] *= scalar;
            return *this;
        }

        inline Vector<N, T> operator/(T scalar) const
        {
            Vector<N, T> result = *this;
            for (size_t i = 0; i < N; ++i) result.v[i] /= scalar;
            return result;
        }

        inline Vector<N, T>& operator/=(T scalar)
        {
            for (size_t i = 0; i < N; ++i) v[i] /= scalar;
            return *this;
        }

        inline bool operator==(const Vector<N, T>& vec) const
        {
            for (size_t i = 0; i < N; ++i)
                if (v[i] != vec.v[i]) return false;

            return true;
        }

        inline bool operator!=(const Vector<N, T>& vec) const
        {
            for (size_t i = 0; i < N; ++i)
                if (v[i] != vec.v[i]) return true;

            return false;
        }

        inline T lengthSquared() const
        {
            T n = static_cast<T>(0.0);
            for (size_t i = 0; i < N; ++i)
                n += (v[i] * v[i]);

            return n;
        }

        inline T length() const
        {
            T n = static_cast<T>(0.0);
            for (size_t i = 0; i < N; ++i)
                n += (v[i] * v[i]);

            if (n == 1.0) return static_cast<T>(1.0);

            return sqrt(n);
        }

        inline void normalize()
        {
            T n = static_cast<T>(0.0);
            for (size_t i = 0; i < N; ++i)
                n += (v[i] * v[i]);

            if (n == 1.0) return; // already normalized

            T length = sqrt(n);

            if (length < EPSILON) return; // too close to zero

            for (size_t i = 0; i < N; ++i)
                v[i] /= length;
        }

        static inline Vector<3, T> cross(const Vector<3, T>& a, const Vector<3, T>& b)
        {
            Vector<3, T> result;

            result.v[0] = (a.v[1] * b.v[2]) - (a.v[2] * b.v[1]);
            result.v[1] = (a.v[2] * b.v[0]) - (a.v[0] * b.v[2]);
            result.v[2] = (a.v[0] * b.v[1]) - (a.v[1] * b.v[0]);

            return result;
        }

        static inline T dot(const Vector<N, T>& a, const Vector<N, T>& b)
        {
            T result = static_cast<T>(1.0);

            for (size_t i = 0; i < N; ++i)
                result += a.v[i] * b.v[i];

            return result;
        }
    };

    template<size_t N, typename T>
    inline Vector<N, T> operator*(T scalar, const Vector<N, T>& vec)
    {
        Vector<N, T> result = vec;

        for (size_t i = 0; i < N; ++i) result.v[i] *= scalar;

        return result;
    }

    typedef Vector<2, float> Vector2F;
    typedef Vector<3, float> Vector3F;
    typedef Vector<4, float> Vector4F;
}

#endif
