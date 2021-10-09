//
//  SoftwareRenderer
//

#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "Vector.hpp"

namespace sr
{
    template <typename T> class Quaternion final
    {
    public:
        T v[4]{0};

        constexpr Quaternion() noexcept {}

        constexpr Quaternion(const T x, const T y, const T z, const T w) noexcept:
            v{{x, y, z, w}}
        {
        }

        T& operator[](std::size_t index) noexcept { return v[index]; }
        constexpr T operator[](std::size_t index) const noexcept { return v[index]; }

        T& x() noexcept { return v[0]; }
        constexpr T x() const noexcept { return v[0]; }

        T& y() noexcept { return v[1]; }
        constexpr T y() const noexcept { return v[1]; }

        T& z() noexcept { return v[2]; }
        constexpr T z() const noexcept { return v[2]; }

        T& w() noexcept { return v[3]; }
        constexpr T w() const noexcept { return v[3]; }

        static constexpr Quaternion identity() noexcept
        {
            return Quaternion{T(0), T(0), T(0), T(1)};
        }

        constexpr const Quaternion operator*(const Quaternion& q) const noexcept
        {
            return Quaternion{
                v[0] * q.v[3] + v[1] * q.v[2] - v[2] * q.v[1] + v[3] * q.v[0],
                -v[0] * q.v[2] + v[1] * q.v[3] + v[2] * q.v[0] + v[3] * q.v[1],
                v[0] * q.v[1] - v[1] * q.v[0] + v[2] * q.v[3] + v[3] * q.v[2],
                -v[0] * q.v[0] - v[1] * q.v[1] - v[2] * q.v[2] + v[3] * q.v[3]
            };
        }

        Quaternion& operator*=(const Quaternion& q) noexcept
        {
            constexpr T tempX = v[0] * q.v[3] + v[1] * q.v[2] - v[2] * q.v[1] + v[3] * q.v[0];
            constexpr T tempY = -v[0] * q.v[2] + v[1] * q.v[3] + v[2] * q.v[0] + v[3] * q.v[1];
            constexpr T tempZ = v[0] * q.v[1] - v[1] * q.v[0] + v[2] * q.v[3] + v[3] * q.v[2];
            constexpr T tempW = -v[0] * q.v[0] - v[1] * q.v[1] - v[2] * q.v[2] + v[3] * q.v[3];

            v[0] = tempX;
            v[1] = tempY;
            v[2] = tempZ;
            v[3] = tempW;

            return *this;
        }

        constexpr const Quaternion operator*(const T scalar) const noexcept
        {
            return Quaternion(v[0] * scalar,
                              v[1] * scalar,
                              v[2] * scalar,
                              v[3] * scalar);
        }

        Quaternion& operator*=(const T scalar) noexcept
        {
            v[0] *= scalar;
            v[1] *= scalar;
            v[2] *= scalar;
            v[3] *= scalar;

            return *this;
        }

        constexpr const Quaternion operator/(const T scalar) const noexcept
        {
            return Quaternion(v[0] / scalar,
                              v[1] / scalar,
                              v[2] / scalar,
                              v[3] / scalar);
        }

        Quaternion& operator/=(const T scalar) noexcept
        {
            v[0] /= scalar;
            v[1] /= scalar;
            v[2] /= scalar;
            v[3] /= scalar;

            return *this;
        }

        constexpr const Quaternion operator-() const noexcept
        {
            return Quaternion(-v[0], -v[1], -v[2], -v[3]);
        }

        constexpr const Quaternion operator+(const Quaternion& q) const noexcept
        {
            return Quaternion(v[0] + q.v[0],
                              v[1] + q.v[1],
                              v[2] + q.v[2],
                              v[3] + q.v[3]);
        }

        Quaternion& operator+=(const Quaternion& q) noexcept
        {
            v[0] += q.v[0];
            v[1] += q.v[1];
            v[2] += q.v[2];
            v[3] += q.v[3];

            return *this;
        }

        constexpr const Quaternion operator-(const Quaternion& q) const noexcept
        {
            return Quaternion(v[0] - q.v[0],
                              v[1] - q.v[1],
                              v[2] - q.v[2],
                              v[3] - q.v[3]);
        }

        Quaternion& operator-=(const Quaternion& q) noexcept
        {
            v[0] -= q.v[0];
            v[1] -= q.v[1];
            v[2] -= q.v[2];
            v[3] -= q.v[3];

            return *this;
        }

        constexpr bool operator==(const Quaternion& q) const noexcept
        {
            return v[0] == q.v[0] && v[1] == q.v[1] && v[2] == q.v[2] && v[3] == q.v[3];
        }

        constexpr bool operator!=(const Quaternion& q) const noexcept
        {
            return v[0] != q.v[0] || v[1] != q.v[1] || v[2] != q.v[2] || v[3] != q.v[3];
        }

        void negate() noexcept
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
            v[3] = -v[3];
        }

        void conjugate() noexcept
        {
            v[0] = -v[0];
            v[1] = -v[1];
            v[2] = -v[2];
        }

        void invert() noexcept
        {
            constexpr auto squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3]; // norm squared
            if (squared <= std::numeric_limits<T>::min())
                return;

            // conjugate divided by norm squared
            const auto multiplier = T(1) / squared;
            v[0] = -v[0] * multiplier;
            v[1] = -v[1] * multiplier;
            v[2] = -v[2] * multiplier;
            v[3] = v[3] * multiplier;
        }

        T getNorm() const noexcept
        {
            constexpr auto n = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (n == T(1)) // already normalized
                return 1;

            return std::sqrt(n);
        }

        void normalize() noexcept
        {
            constexpr auto squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (squared == T(1)) // already normalized
                return;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return;

            const auto multiplier = T(1) / length;
            v[0] *= multiplier;
            v[1] *= multiplier;
            v[2] *= multiplier;
            v[3] *= multiplier;
        }

        Quaternion normalized() const noexcept
        {
            constexpr auto squared = v[0] * v[0] + v[1] * v[1] + v[2] * v[2] + v[3] * v[3];
            if (squared == T(1)) // already normalized
                return *this;

            const auto length = std::sqrt(squared);
            if (length <= std::numeric_limits<T>::min()) // too close to zero
                return *this;

            const T multiplier = T(1) / length;
            return Quaternion(v[0] * multiplier,
                              v[1] * multiplier,
                              v[2] * multiplier,
                              v[3] * multiplier);
        }

        void rotate(const T angle, const Vector<T, 3>& axis) noexcept
        {
            const auto normalizedAxis = axis.normalized();

            const auto cosAngle = std::cos(angle / T(2));
            const auto sinAngle = std::sin(angle / T(2));

            v[0] = normalizedAxis.v[0] * sinAngle;
            v[1] = normalizedAxis.v[1] * sinAngle;
            v[2] = normalizedAxis.v[2] * sinAngle;
            v[3] = cosAngle;
        }

        void getRotation(T& angle, Vector<T, 3>& axis) const noexcept
        {
            angle = T(2) * std::acos(v[3]);
            const auto s = std::sqrt(T(1) - v[3] * v[3]);
            if (s <= std::numeric_limits<T>::min()) // too close to zero
            {
                axis.v[0] = v[0];
                axis.v[1] = v[1];
                axis.v[2] = v[2];
            }
            else
            {
                axis.v[0] = v[0] / s;
                axis.v[1] = v[1] / s;
                axis.v[2] = v[2] / s;
            }
        }

        Vector<T, 3> getEulerAngles() const noexcept
        {
            Vector<T, 3> result;
            result.v[0] = std::atan2(2 * (v[1] * v[2] + v[3] * v[0]), v[3] * v[3] - v[0] * v[0] - v[1] * v[1] + v[2] * v[2]);
            result.v[1] = std::asin(-2 * (v[0] * v[2] - v[3] * v[1]));
            result.v[2] = std::atan2(2 * (v[0] * v[1] + v[3] * v[2]), v[3] * v[3] + v[0] * v[0] - v[1] * v[1] - v[2] * v[2]);
            return result;
        }

        T getEulerAngleX() const noexcept
        {
            return std::atan2(T(2) * (v[1] * v[2] + v[3] * v[0]), v[3] * v[3] - v[0] * v[0] - v[1] * v[1] + v[2] * v[2]);
        }

        T getEulerAngleY() const noexcept
        {
            return std::asin(T(-2) * (v[0] * v[2] - v[3] * v[1]));
        }

        T getEulerAngleZ() const noexcept
        {
            return std::atan2(T(2) * (v[0] * v[1] + v[3] * v[2]), v[3] * v[3] + v[0] * v[0] - v[1] * v[1] - v[2] * v[2]);
        }

        void setEulerAngles(const Vector<T, 3>& angles) noexcept
        {
            const auto angleR = angles.v[0] / T(2);
            const auto sr = std::sin(angleR);
            const auto cr = std::cos(angleR);

            const auto angleP = angles.v[1] / T(2);
            const auto sp = std::sin(angleP);
            const auto cp = std::cos(angleP);

            const auto angleY = angles.v[2] / T(2);
            const auto sy = std::sin(angleY);
            const auto cy = std::cos(angleY);

            const auto cpcy = cp * cy;
            const auto spcy = sp * cy;
            const auto cpsy = cp * sy;
            const auto spsy = sp * sy;

            v[0] = sr * cpcy - cr * spsy;
            v[1] = cr * spcy + sr * cpsy;
            v[2] = cr * cpsy - sr * spcy;
            v[3] = cr * cpcy + sr * spsy;
        }

        const Vector<T, 3> operator*(const Vector<T, 3>& vector) const noexcept
        {
            return rotateVector(vector);
        }

        Vector<T, 3> rotateVector(const Vector<T, 3>& vector) const noexcept
        {
            const Vector<T, 3> q{v[0], v[1], v[2]};
            const auto t = T(2) * q.cross(vector);
            return vector + (v[3] * t) + q.cross(t);
        }

        Vector<T, 3> getRightVector() const noexcept
        {
            return rotateVector(Vector<T, 3>(T(1), T(0), T(0)));
        }

        Vector<T, 3> getUpVector() const noexcept
        {
            return rotateVector(Vector<T, 3>(T(0), T(1), T(0)));
        }

        Vector<T, 3> getForwardVector() const noexcept
        {
            return rotateVector(Vector<T, 3>(T(0), T(0), T(1)));
        }

        Quaternion& lerp(const Quaternion& q1, const Quaternion& q2, T t) noexcept
        {
            *this = (q1 * (T(1) - t)) + (q2 * t);
            return *this;
        }
    };
}

#endif
