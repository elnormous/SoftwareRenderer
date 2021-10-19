//
//  SoftwareRenderer
//

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <algorithm>
#include <array>
#include <cstddef>
#include <cmath>
#include <iterator>

namespace sr
{
    template <typename T, std::size_t N> class Vector final
    {
    public:
        std::array<T, N> v{0};

        constexpr Vector() noexcept {}

        template <typename ...A>
        explicit constexpr Vector(A... args) noexcept:
            v{args...}
        {
        }

        template <std::size_t X = N, std::size_t N2, typename std::enable_if<(X != N2)>::type* = nullptr>
        explicit Vector(const Vector<T, N2>& vec) noexcept
        {
            for (std::size_t i = 0; i < N && i < N2; ++i)
                v[i] = vec.v[i];
        }

        T& operator[](std::size_t index) noexcept { return v[index]; }
        constexpr T operator[](std::size_t index) const noexcept { return v[index]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 1)>::type* = nullptr>
        T& x() noexcept { return v[0]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 1)>::type* = nullptr>
        constexpr T x() const noexcept { return v[0]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 2)>::type* = nullptr>
        T& y() noexcept { return v[1]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 2)>::type* = nullptr>
        constexpr T y() const noexcept { return v[1]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 3)>::type* = nullptr>
        T& z() noexcept { return v[2]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 3)>::type* = nullptr>
        constexpr T z() const noexcept { return v[2]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 4)>::type* = nullptr>
        T& w() noexcept { return v[3]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 4)>::type* = nullptr>
        constexpr T w() const noexcept { return v[3]; }

        template <std::size_t X = N, typename std::enable_if<(X == 2)>::type* = nullptr>
        T getAngle() const noexcept
        {
            return std::atan2(v[1], v[0]);
        }

        template <std::size_t X = N, typename std::enable_if<(X == 3)>::type* = nullptr>
        T getAngle(const Vector& axis) const noexcept
        {
            constexpr T dx = axis.v[0] - v[0] - v[1] * axis.v[2] + v[2] * axis.v[1];
            constexpr T dy = axis.v[1] - v[1] - v[2] * axis.v[0] + v[0] * axis.v[2];
            constexpr T dz = axis.v[2] - v[2] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return std::atan2(std::sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        template <std::size_t X = N, typename std::enable_if<(X == 4)>::type* = nullptr>
        T getAngle(const Vector& axis) const noexcept
        {
            constexpr T dx = v[3] * axis.v[0] - v[0] * axis.v[3] - v[1] * axis.v[2] + v[2] * axis.v[1];
            constexpr T dy = v[3] * axis.v[1] - v[1] * axis.v[3] - v[2] * axis.v[0] + v[0] * axis.v[2];
            constexpr T dz = v[3] * axis.v[2] - v[2] * axis.v[3] - v[0] * axis.v[1] + v[1] * axis.v[0];

            return std::atan2(std::sqrt(dx * dx + dy * dy + dz * dz), dot(axis));
        }

        void clamp(const Vector& min, const Vector& max) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] < min.v[i]) v[i] = min.v[i];
                else if (v[i] > max.v[i]) v[i] = max.v[i];
        }

        template <std::size_t X = N, typename std::enable_if<(X == 3)>::type* = nullptr>
        constexpr Vector cross(const Vector& vec) const noexcept
        {
            return Vector{
                (v[1] * vec.v[2]) - (v[2] * vec.v[1]),
                (v[2] * vec.v[0]) - (v[0] * vec.v[2]),
                (v[0] * vec.v[1]) - (v[1] * vec.v[0])
            };
        }

        T distance(const Vector& vec) const noexcept
        {
            T d = 0;
            for (std::size_t i = 0; i < N; ++i)
                d += (vec.v[i] - v[i]) * (vec.v[i] - v[i]);
            return std::sqrt(d);
        }

        T distanceSquared(const Vector& vec) const noexcept
        {
            T d = 0;
            for (std::size_t i = 0; i < N; ++i)
                d += (vec.v[i] - v[i]) * (vec.v[i] - v[i]);
            return d;
        }

        T dot(const Vector& vec) const noexcept
        {
            T d = 0;
            for (std::size_t i = 0; i < N; ++i)
                d += v[i] * vec.v[i];
            return d;
        }

        T length() const noexcept
        {
            T l = 0;
            for (const auto& c : v)
                l += c * c;
            return std::sqrt(l);
        }

        T lengthSquared() const noexcept
        {
            T l = 0;
            for (const auto& c : v)
                l += c * c;
            return l;
        }

        void negate() noexcept
        {
            for (auto& c : v)
                c = -c;
        }

        bool isNormalized(const T tolerance = std::numeric_limits<T>::epsilon()) const noexcept
        {
            return std::abs(T(1) - lengthSquared()) <= tolerance;
        }

        void normalize() noexcept
        {
            T squared = T(0);
            for (const auto& c : v)
                squared += c * c;

            if (squared == T(1)) // already normalized
                return;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
                return;

            const auto multiplier = T(1) / length;
            for (auto& c : v)
                c *= multiplier;
        }

        Vector normalized() const noexcept
        {
            T squared = T(0);
            for (const auto& c : v)
                squared += c * c;

            if (squared == T(1)) // already normalized
                return *this;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::epsilon()) // too close to zero
                return *this;

            const auto multiplier = T(1) / length;
            return *this * multiplier;
        }

        void scale(const Vector& scale) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] *= scale.v[i];
        }

        void smooth(const Vector& target, const T elapsedTime, const T responseTime) noexcept
        {
            if (elapsedTime > T(0))
                *this += (target - *this) * (elapsedTime / (elapsedTime + responseTime));
        }

        T getMin() const noexcept
        {
            T result = v[0];

            for (std::size_t i = 1; i < N; ++i)
                if (v[i] < result)
                    result = v[i];

            return result;
        }

        T getMax() const noexcept
        {
            T result = v[0];

            for (std::size_t i = 1; i < N; ++i)
                if (v[i] > result)
                    result = v[i];

            return result;
        }

        const Vector operator+(const Vector& vec) const noexcept
        {
            Vector result = *this;
            for (std::size_t i = 0; i < N; ++i)
                result.v[i] += vec.v[i];
            return result;
        }

        Vector& operator+=(const Vector& vec) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] += vec.v[i];
            return *this;
        }

        const Vector operator-(const Vector& vec) const noexcept
        {
            Vector result = *this;
            for (std::size_t i = 0; i < N; ++i)
                result.v[i] -= vec.v[i];
            return result;
        }

        Vector& operator-=(const Vector& vec) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] -= vec.v[i];
            return *this;
        }

        const Vector operator-() const noexcept
        {
            Vector result = *this;
            for (auto& c : result.v)
                c = -c;
            return result;
        }

        const Vector operator*(const T scalar) const noexcept
        {
            Vector result(*this);
            for (auto& c : result.v)
                c *= scalar;
            return result;
        }

        Vector& operator*=(const T scalar) noexcept
        {
            for (auto& c : v)
                c *= scalar;
            return *this;
        }

        const Vector operator/(const T scalar) const noexcept
        {
            Vector result(*this);
            for (auto& c : result.v)
                c /= scalar;
            return result;
        }

        Vector& operator/=(const T scalar) noexcept
        {
            for (auto& c : v)
                c /= scalar;
            return *this;
        }

        bool operator<(const Vector& vec) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] < vec.v[i]) return true;
                else if (vec.v[i] < v[i]) return false;

            return false;
        }

        bool operator==(const Vector& vec) const noexcept
        {
            return std::equal(std::begin(v), std::end(v), std::begin(vec.v));
        }

        bool operator!=(const Vector& vec) const noexcept
        {
            return !std::equal(std::begin(v), std::end(v), std::begin(vec.v));
        }

        bool isZero() const noexcept
        {
            for (const auto& c : v)
                if (c != T(0)) return false;
            return true;
        }
    };

    template <typename T, std::size_t N>
    inline const Vector<T, N> operator*(const T scalar, const Vector<T, N>& vec) noexcept
    {
        Vector<T, N> result = vec;
        result *= scalar;
        return result;
    }
}

#endif
