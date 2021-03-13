//
//  SoftwareRenderer
//

#ifndef MATRIX_H
#define MATRIX_H

#include <algorithm>
#include <cassert>
#include <cmath>
#include "Quaternion.hpp"
#include "Vector.hpp"

namespace sr
{
    template <typename T, std::size_t C, std::size_t R = C> class Matrix final
    {
    public:
        T m[C * R]{0};

        constexpr Matrix() noexcept {}

        template <typename ...A>
        explicit constexpr Matrix(A... args) noexcept:
            m{args...}
        {
        }

        T& operator[](std::size_t index) noexcept { return m[index]; }
        constexpr T operator[](std::size_t index) const noexcept { return m[index]; }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        static constexpr Matrix identity() noexcept
        {
            return Matrix{
                T(1), T(0), T(0), T(0),
                T(0), T(1), T(0), T(0),
                T(0), T(0), T(1), T(0),
                T(0), T(0), T(0), T(1)
            };
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setLookAt(const Vector<T, 3>& eyePosition,
                       const Vector<T, 3>& targetPosition,
                       const Vector<T, 3>& up) noexcept
        {
            setLookAt(eyePosition.v[0], eyePosition.v[1], eyePosition.v[2],
                      targetPosition.v[0], targetPosition.v[1], targetPosition.v[2],
                      up.v[0], up.v[1], up.v[2]);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setLookAt(const T eyePositionX, const T eyePositionY, const T eyePositionZ,
                       const T targetPositionX, const T targetPositionY, const T targetPositionZ,
                       const T upX, const T upY, const T upZ) noexcept
        {
            const Vector<T, 3> eye(eyePositionX, eyePositionY, eyePositionZ);
            const Vector<T, 3> target(targetPositionX, targetPositionY, targetPositionZ);
            Vector<T, 3> up(upX, upY, upZ);
            up.normalize();

            Vector<T, 3> zaxis = target - eye;
            zaxis.normalize();

            Vector<T, 3> xaxis = up.cross(zaxis);
            xaxis.normalize();

            Vector<T, 3> yaxis = zaxis.cross(xaxis);
            yaxis.normalize();

            m[0] = xaxis.v[0];
            m[1] = yaxis.v[0];
            m[2] = zaxis.v[0];
            m[3] = T(0);

            m[4] = xaxis.v[1];
            m[5] = yaxis.v[1];
            m[6] = zaxis.v[1];
            m[7] = T(0);

            m[8] = xaxis.v[2];
            m[9] = yaxis.v[2];
            m[10] = zaxis.v[2];
            m[11] = T(0);

            m[12] = xaxis.dot(-eye);
            m[13] = yaxis.dot(-eye);
            m[14] = zaxis.dot(-eye);
            m[15] = T(1);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setPerspective(const T fieldOfView, const T aspectRatio,
                            const T zNearPlane, const T zFarPlane) noexcept
        {
            assert(zFarPlane != zNearPlane);

            const auto theta = fieldOfView / T(2);
            if (std::fabs(std::fmod(theta, T(pi) / T(2))) <= std::numeric_limits<T>::min())
                return;

            const auto divisor = tan(theta);
            assert(divisor);
            const auto factor = T(1) / divisor;

            setZero();

            assert(aspectRatio);
            m[0] = (T(1) / aspectRatio) * factor;
            m[5] = factor;
            m[10] = zFarPlane / (zFarPlane - zNearPlane);
            m[11] = T(1);
            m[14] = -zNearPlane * zFarPlane / (zFarPlane - zNearPlane);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setOrthographic(const T width, const T height,
                             const T nearClip, const T farClip) noexcept
        {
            assert(width);
            assert(height);
            assert(farClip != nearClip);

            setZero();

            m[0] = T(2) / width;
            m[5] = T(2) / height;
            m[10] = T(1) / (farClip - nearClip);
            m[14] = nearClip / (nearClip - farClip);
            m[15] = T(1);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setOrthographic(const T left, const T right,
                             const T bottom, const T top,
                             const T near, const T far) noexcept
        {
            assert(right != left);
            assert(top != bottom);
            assert(far != near);

            setZero();

            m[0] = T(2) / (right - left);
            m[5] = T(2) / (top - bottom);
            m[10] = T(1) / (far - near);
            m[12] = (left + right) / (left - right);
            m[13] = (bottom + top) / (bottom - top);
            m[14] = near / (near - far);
            m[15] = T(1);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 3 && Y == 3)>::type* = nullptr>
        void setScale(const Vector<T, 2>& scale) noexcept
        {
            setIdentity();

            m[0] = scale.v[0];
            m[4] = scale.v[1];
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setScale(const Vector<T, 3>& scale) noexcept
        {
            setIdentity();

            m[0] = scale.v[0];
            m[5] = scale.v[1];
            m[10] = scale.v[2];
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 3 && Y == 3)>::type* = nullptr>
        void setScale(const T x, const T y) noexcept
        {
            setIdentity();

            m[0] = x;
            m[4] = y;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setScale(const T x, const T y, const T z) noexcept
        {
            setIdentity();

            m[0] = x;
            m[5] = y;
            m[10] = z;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 3 && Y == 3)>::type* = nullptr>
        void setRotation(const T angle) noexcept
        {
            setIdentity();

            const auto c = std::cos(angle);
            const auto s = std::sin(angle);

            m[0] = c;
            m[3] = -s;
            m[1] = s;
            m[4] = c;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setRotation(const Vector<T, 3>& axis, T angle) noexcept
        {
            const auto x = axis.v[0];
            const auto y = axis.v[1];
            const auto z = axis.v[2];

            const auto squared = x * x + y * y + z * z;
            if (squared != T(1))
            {
                const auto length = std::sqrt(squared);
                if (length > std::numeric_limits<T>::min())
                {
                    const auto multiplier = T(1) / length;
                    x *= multiplier;
                    y *= multiplier;
                    z *= multiplier;
                }
            }

            const auto c = std::cos(angle);
            const auto s = std::sin(angle);

            const auto t = T(1) - c;
            const auto tx = t * x;
            const auto ty = t * y;
            const auto tz = t * z;
            const auto txy = tx * y;
            const auto txz = tx * z;
            const auto tyz = ty * z;
            const auto sx = s * x;
            const auto sy = s * y;
            const auto sz = s * z;

            m[0] = c + tx * x;
            m[4] = txy - sz;
            m[8] = txz + sy;
            m[12] = T(0);

            m[1] = txy + sz;
            m[5] = c + ty * y;
            m[9] = tyz - sx;
            m[13] = T(0);

            m[2] = txz - sy;
            m[6] = tyz + sx;
            m[10] = c + tz * z;
            m[14] = T(0);

            m[3] = T(0);
            m[7] = T(0);
            m[11] = T(0);
            m[15] = T(1);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setRotation(const Quaternion<T>& rotation) noexcept
        {
            const auto wx = rotation.v[3] * rotation.v[0];
            const auto wy = rotation.v[3] * rotation.v[1];
            const auto wz = rotation.v[3] * rotation.v[2];

            const auto xx = rotation.v[0] * rotation.v[0];
            const auto xy = rotation.v[0] * rotation.v[1];
            const auto xz = rotation.v[0] * rotation.v[2];

            const auto yy = rotation.v[1] * rotation.v[1];
            const auto yz = rotation.v[1] * rotation.v[2];

            const auto zz = rotation.v[2] * rotation.v[2];

            m[0] = T(1) - T(2) * (yy + zz);
            m[4] = T(2) * (xy - wz);
            m[8] = T(2) * (xz + wy);
            m[12] = T(0);

            m[1] = T(2) * (xy + wz);
            m[5] = T(1) - T(2) * (xx + zz);
            m[9] = T(2) * (yz - wx);
            m[13] = T(0);

            m[2] = T(2) * (xz - wy);
            m[6] = T(2) * (yz + wx);
            m[10] = T(1) - T(2) * (xx + yy);
            m[14] = T(0);

            m[3] = T(0);
            m[7] = T(0);
            m[11] = T(0);
            m[15] = T(1);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setRotationX(const T angle) noexcept
        {
            setIdentity();

            const auto c = std::cos(angle);
            const auto s = std::sin(angle);

            m[5] = c;
            m[9] = -s;
            m[6] = s;
            m[10] = c;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setRotationY(const T angle) noexcept
        {
            setIdentity();

            const auto c = std::cos(angle);
            const auto s = std::sin(angle);

            m[0] = c;
            m[8] = s;
            m[2] = -s;
            m[10] = c;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setRotationZ(const T angle) noexcept
        {
            setIdentity();

            const auto c = std::cos(angle);
            const auto s = std::sin(angle);

            m[0] = c;
            m[4] = -s;
            m[1] = s;
            m[5] = c;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 3 && Y == 3)>::type* = nullptr>
        void setTranslation(const Vector<T, 3>& translation) noexcept
        {
            setIdentity();

            m[6] = translation.v[0];
            m[7] = translation.v[1];
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setTranslation(const Vector<T, 3>& translation) noexcept
        {
            setIdentity();

            m[12] = translation.v[0];
            m[13] = translation.v[1];
            m[14] = translation.v[2];
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 3 && Y == 3)>::type* = nullptr>
        void setTranslation(const T x, const T y) noexcept
        {
            setIdentity();

            m[6] = x;
            m[7] = y;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void setTranslation(const T x, const T y, const T z) noexcept
        {
            setIdentity();

            m[12] = x;
            m[13] = y;
            m[14] = z;
        }

        void add(const T scalar) noexcept
        {
            add(scalar, *this);
        }

        void add(const T scalar, Matrix& dst) const noexcept
        {
            dst.m[0] = m[0] + scalar;
            dst.m[1] = m[1] + scalar;
            dst.m[2] = m[2] + scalar;
            dst.m[3] = m[3] + scalar;
            dst.m[4] = m[4] + scalar;
            dst.m[5] = m[5] + scalar;
            dst.m[6] = m[6] + scalar;
            dst.m[7] = m[7] + scalar;
            dst.m[8] = m[8] + scalar;
            dst.m[9] = m[9] + scalar;
            dst.m[10] = m[10] + scalar;
            dst.m[11] = m[11] + scalar;
            dst.m[12] = m[12] + scalar;
            dst.m[13] = m[13] + scalar;
            dst.m[14] = m[14] + scalar;
            dst.m[15] = m[15] + scalar;
        }

        void add(const Matrix& matrix) noexcept
        {
            add(matrix, *this);
        }

        void add(const Matrix& matrix, Matrix& dst) const noexcept
        {
            dst.m[0] = m[0] + matrix.m[0];
            dst.m[1] = m[1] + matrix.m[1];
            dst.m[2] = m[2] + matrix.m[2];
            dst.m[3] = m[3] + matrix.m[3];
            dst.m[4] = m[4] + matrix.m[4];
            dst.m[5] = m[5] + matrix.m[5];
            dst.m[6] = m[6] + matrix.m[6];
            dst.m[7] = m[7] + matrix.m[7];
            dst.m[8] = m[8] + matrix.m[8];
            dst.m[9] = m[9] + matrix.m[9];
            dst.m[10] = m[10] + matrix.m[10];
            dst.m[11] = m[11] + matrix.m[11];
            dst.m[12] = m[12] + matrix.m[12];
            dst.m[13] = m[13] + matrix.m[13];
            dst.m[14] = m[14] + matrix.m[14];
            dst.m[15] = m[15] + matrix.m[15];
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 1 && Y == 1)>::type* = nullptr>
        T determinant() const noexcept
        {
            return m[0];
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 2 && Y == 2)>::type* = nullptr>
        T determinant() const noexcept
        {
            return m[0] * m[3] - m[1] * m[2];
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 3 && Y == 3)>::type* = nullptr>
        T determinant() const noexcept
        {
            const auto a0 = m[0] * (m[4] * m[8] - m[5] * m[7]);
            const auto a1 = m[1] * (m[3] * m[8] - m[5] * m[6]);
            const auto a2 = m[2] * (m[3] * m[7] - m[4] * m[6]);
            return a0 - a1 + a2;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        T determinant() const noexcept
        {
            const auto a0 = m[0] * m[5] - m[1] * m[4];
            const auto a1 = m[0] * m[6] - m[2] * m[4];
            const auto a2 = m[0] * m[7] - m[3] * m[4];
            const auto a3 = m[1] * m[6] - m[2] * m[5];
            const auto a4 = m[1] * m[7] - m[3] * m[5];
            const auto a5 = m[2] * m[7] - m[3] * m[6];
            const auto b0 = m[8] * m[13] - m[9] * m[12];
            const auto b1 = m[8] * m[14] - m[10] * m[12];
            const auto b2 = m[8] * m[15] - m[11] * m[12];
            const auto b3 = m[9] * m[14] - m[10] * m[13];
            const auto b4 = m[9] * m[15] - m[11] * m[13];
            const auto b5 = m[10] * m[15] - m[11] * m[14];
            return a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        Vector<T, 3> getUpVector() const noexcept
        {
            return Vector<T, 3>{m[4], m[5], m[6]};
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        Vector<T, 3> getDownVector() const noexcept
        {
            return Vector<T, 3>{-m[4], -m[5], -m[6]};
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        Vector<T, 3> getLeftVector() const noexcept
        {
            return Vector<T, 3>{-m[0], -m[1], -m[2]};
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        Vector<T, 3> getRightVector() const noexcept
        {
            return Vector<T, 3>{m[0], m[1], m[2]};
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        Vector<T, 3> getForwardVector() const noexcept
        {
            return Vector<T, 3>{-m[8], -m[9], -m[10]};
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        Vector<T, 3> getBackVector() const noexcept
        {
            return Vector<T, 3>{m[8], m[9], m[10]};
        }

        void invert() noexcept
        {
            invert(*this);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void invert(Matrix& dst) const noexcept
        {
            const auto a0 = m[0] * m[5] - m[1] * m[4];
            const auto a1 = m[0] * m[6] - m[2] * m[4];
            const auto a2 = m[0] * m[7] - m[3] * m[4];
            const auto a3 = m[1] * m[6] - m[2] * m[5];
            const auto a4 = m[1] * m[7] - m[3] * m[5];
            const auto a5 = m[2] * m[7] - m[3] * m[6];
            const auto b0 = m[8] * m[13] - m[9] * m[12];
            const auto b1 = m[8] * m[14] - m[10] * m[12];
            const auto b2 = m[8] * m[15] - m[11] * m[12];
            const auto b3 = m[9] * m[14] - m[10] * m[13];
            const auto b4 = m[9] * m[15] - m[11] * m[13];
            const auto b5 = m[10] * m[15] - m[11] * m[14];

            // Calculate the determinant
            const auto det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

            // Close to zero, can't invert
            if (std::fabs(det) <= std::numeric_limits<T>::min()) return;

            const Matrix adjugate{
                m[5] * b5 - m[6] * b4 + m[7] * b3,
                -m[1] * b5 + m[2] * b4 - m[3] * b3,
                m[13] * a5 - m[14] * a4 + m[15] * a3,
                -m[9] * a5 + m[10] * a4 - m[11] * a3,

                -m[4] * b5 + m[6] * b2 - m[7] * b1,
                m[0] * b5 - m[2] * b2 + m[3] * b1,
                -m[12] * a5 + m[14] * a2 - m[15] * a1,
                m[8] * a5 - m[10] * a2 + m[11] * a1,

                m[4] * b4 - m[5] * b2 + m[7] * b0,
                -m[0] * b4 + m[1] * b2 - m[3] * b0,
                m[12] * a4 - m[13] * a2 + m[15] * a0,
                -m[8] * a4 + m[9] * a2 - m[11] * a0,

                -m[4] * b3 + m[5] * b1 - m[6] * b0,
                m[0] * b3 - m[1] * b1 + m[2] * b0,
                -m[12] * a3 + m[13] * a1 - m[14] * a0,
                m[8] * a3 - m[9] * a1 + m[10] * a0
            };

            adjugate.multiply(T(1) / det, dst);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == Y)>::type* = nullptr>
        bool isIdentity() const noexcept
        {
            for (std::size_t r = 0; r < R; ++r)
                for (std::size_t c = 0; c < C; ++c)
                    if (m[r * C + c] != (r == c ? T(1) : T(0)))
                        return false;
            return true;
        }

        void multiply(const T scalar) noexcept
        {
            multiply(scalar, *this);
        }

        void multiply(const T scalar, Matrix& dst) const noexcept
        {
            dst.m[0] = m[0] * scalar;
            dst.m[1] = m[1] * scalar;
            dst.m[2] = m[2] * scalar;
            dst.m[3] = m[3] * scalar;
            dst.m[4] = m[4] * scalar;
            dst.m[5] = m[5] * scalar;
            dst.m[6] = m[6] * scalar;
            dst.m[7] = m[7] * scalar;
            dst.m[8] = m[8] * scalar;
            dst.m[9] = m[9] * scalar;
            dst.m[10] = m[10] * scalar;
            dst.m[11] = m[11] * scalar;
            dst.m[12] = m[12] * scalar;
            dst.m[13] = m[13] * scalar;
            dst.m[14] = m[14] * scalar;
            dst.m[15] = m[15] * scalar;
        }

        void multiply(const Matrix& matrix) noexcept
        {
            multiply(matrix, *this);
        }

        void multiply(const Matrix& matrix, Matrix& dst) const noexcept
        {
            const auto product = {
                m[0] * matrix.m[0] + m[4] * matrix.m[1] + m[8] * matrix.m[2] + m[12] * matrix.m[3],
                m[1] * matrix.m[0] + m[5] * matrix.m[1] + m[9] * matrix.m[2] + m[13] * matrix.m[3],
                m[2] * matrix.m[0] + m[6] * matrix.m[1] + m[10] * matrix.m[2] + m[14] * matrix.m[3],
                m[3] * matrix.m[0] + m[7] * matrix.m[1] + m[11] * matrix.m[2] + m[15] * matrix.m[3],

                m[0] * matrix.m[4] + m[4] * matrix.m[5] + m[8] * matrix.m[6] + m[12] * matrix.m[7],
                m[1] * matrix.m[4] + m[5] * matrix.m[5] + m[9] * matrix.m[6] + m[13] * matrix.m[7],
                m[2] * matrix.m[4] + m[6] * matrix.m[5] + m[10] * matrix.m[6] + m[14] * matrix.m[7],
                m[3] * matrix.m[4] + m[7] * matrix.m[5] + m[11] * matrix.m[6] + m[15] * matrix.m[7],

                m[0] * matrix.m[8] + m[4] * matrix.m[9] + m[8] * matrix.m[10] + m[12] * matrix.m[11],
                m[1] * matrix.m[8] + m[5] * matrix.m[9] + m[9] * matrix.m[10] + m[13] * matrix.m[11],
                m[2] * matrix.m[8] + m[6] * matrix.m[9] + m[10] * matrix.m[10] + m[14] * matrix.m[11],
                m[3] * matrix.m[8] + m[7] * matrix.m[9] + m[11] * matrix.m[10] + m[15] * matrix.m[11],

                m[0] * matrix.m[12] + m[4] * matrix.m[13] + m[8] * matrix.m[14] + m[12] * matrix.m[15],
                m[1] * matrix.m[12] + m[5] * matrix.m[13] + m[9] * matrix.m[14] + m[13] * matrix.m[15],
                m[2] * matrix.m[12] + m[6] * matrix.m[13] + m[10] * matrix.m[14] + m[14] * matrix.m[15],
                m[3] * matrix.m[12] + m[7] * matrix.m[13] + m[11] * matrix.m[14] + m[15] * matrix.m[15]
            };

            std::copy(std::begin(product), std::end(product), dst.m);
        }

        void negate() noexcept
        {
            negate(*this);
        }

        void negate(Matrix& dst) const noexcept
        {
            dst.m[0] = -m[0];
            dst.m[1] = -m[1];
            dst.m[2] = -m[2];
            dst.m[3] = -m[3];
            dst.m[4] = -m[4];
            dst.m[5] = -m[5];
            dst.m[6] = -m[6];
            dst.m[7] = -m[7];
            dst.m[8] = -m[8];
            dst.m[9] = -m[9];
            dst.m[10] = -m[10];
            dst.m[11] = -m[11];
            dst.m[12] = -m[12];
            dst.m[13] = -m[13];
            dst.m[14] = -m[14];
            dst.m[15] = -m[15];
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == Y)>::type* = nullptr>
        void setIdentity() noexcept
        {
            for (std::size_t r = 0; r < R; ++r)
                for (std::size_t c = 0; c < C; ++c)
                    m[r * C + c] = static_cast<T>(r == c ? T(1) : T(0));
        }

        void setZero() noexcept
        {
            for (auto& c : m)
                c = T(0);
        }

        void subtract(const Matrix& matrix) noexcept
        {
            subtract(matrix, *this);
        }

        void subtract(const Matrix& matrix, Matrix& dst) const noexcept
        {
            dst.m[0] = m[0] - matrix.m[0];
            dst.m[1] = m[1] - matrix.m[1];
            dst.m[2] = m[2] - matrix.m[2];
            dst.m[3] = m[3] - matrix.m[3];
            dst.m[4] = m[4] - matrix.m[4];
            dst.m[5] = m[5] - matrix.m[5];
            dst.m[6] = m[6] - matrix.m[6];
            dst.m[7] = m[7] - matrix.m[7];
            dst.m[8] = m[8] - matrix.m[8];
            dst.m[9] = m[9] - matrix.m[9];
            dst.m[10] = m[10] - matrix.m[10];
            dst.m[11] = m[11] - matrix.m[11];
            dst.m[12] = m[12] - matrix.m[12];
            dst.m[13] = m[13] - matrix.m[13];
            dst.m[14] = m[14] - matrix.m[14];
            dst.m[15] = m[15] - matrix.m[15];
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void transformPoint(Vector<T, 3>& point) const noexcept
        {
            transformVector(point.v[0], point.v[1], point.v[2], 1, point);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void transformPoint(const Vector<T, 3>& point, Vector<T, 3>& dst) const noexcept
        {
            transformVector(point.v[0], point.v[1], point.v[2], 1, dst);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void transformVector(Vector<T, 3>& v) const noexcept
        {
            Vector<T, 4> t;
            transformVector(Vector<T, 4>{v.v[0], v.v[1], v.v[2], T(0)}, t);
            v = Vector<T, 3>{t.v[0], t.v[1], t.v[2]};
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void transformVector(const Vector<T, 3>& v, Vector<T, 3>& dst) const noexcept
        {
            transformVector(v.v[0], v.v[1], v.v[2], 0, dst);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void transformVector(const T x, const T y, const T z, const T w,
                             Vector<T, 3>& dst) const noexcept
        {
            Vector<T, 4> t;
            transformVector(Vector<T, 4>{x, y, z, w}, t);
            dst = Vector<T, 3>{t.v[0], t.v[1], t.v[2]};
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        void transformVector(Vector<T, 4>& v) const noexcept
        {
            transformVector(v, v);
        }

        void transformVector(const Vector<T, 4>& v, Vector<T, 4>& dst) const noexcept
        {
            assert(&v != &dst);
            dst.v[0] = v.v[0] * m[0] + v.v[1] * m[4] + v.v[2] * m[8] + v.v[3] * m[12];
            dst.v[1] = v.v[0] * m[1] + v.v[1] * m[5] + v.v[2] * m[9] + v.v[3] * m[13];
            dst.v[2] = v.v[0] * m[2] + v.v[1] * m[6] + v.v[2] * m[10] + v.v[3] * m[14];
            dst.v[3] = v.v[0] * m[3] + v.v[1] * m[7] + v.v[2] * m[11] + v.v[3] * m[15];
        }

        void transpose() noexcept
        {
            transpose(*this);
        }

        void transpose(Matrix& dst) const noexcept
        {
            const auto t = {
                m[0], m[4], m[8], m[12],
                m[1], m[5], m[9], m[13],
                m[2], m[6], m[10], m[14],
                m[3], m[7], m[11], m[15]
            };
            std::copy(std::begin(t), std::end(t), dst.m);
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 3 && Y == 3)>::type* = nullptr>
        Vector<T, 2> getTranslation() const noexcept
        {
            return Vector<T, 2>{m[6], m[7]};
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        Vector<T, 3> getTranslation() const noexcept
        {
            return Vector<T, 3>{m[12], m[13], m[14]};
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 3 && Y == 3)>::type* = nullptr>
        Vector<T, 2> getScale() const noexcept
        {
            Vector<T, 2> scale;
            scale.v[0] = Vector<T, 2>{m[0], m[1]}.length();
            scale.v[1] = Vector<T, 2>{m[3], m[4]}.length();

            return scale;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        Vector<T, 3> getScale() const noexcept
        {
            Vector<T, 3> scale;
            scale.v[0] = Vector<T, 3>{m[0], m[1], m[2]}.length();
            scale.v[1] = Vector<T, 3>{m[4], m[5], m[6]}.length();
            scale.v[2] = Vector<T, 3>{m[8], m[9], m[10]}.length();

            return scale;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        Vector<T, 3> getRotation() const noexcept
        {
            const auto scale = getScale();

            const auto m11 = m[0] / scale.v[0];
            const auto m21 = m[1] / scale.v[0];
            const auto m31 = m[2] / scale.v[0];

            const auto m12 = m[4] / scale.v[1];
            const auto m22 = m[5] / scale.v[1];
            const auto m32 = m[6] / scale.v[1];

            const auto m13 = m[8] / scale.v[2];
            const auto m23 = m[9] / scale.v[2];
            const auto m33 = m[10] / scale.v[2];

            Quaternion<T> result;
            result.v[0] = std::sqrt(std::max(static_cast<T>(0), T(1) + m11 - m22 - m33)) / T(2);
            result.v[1] = std::sqrt(std::max(static_cast<T>(0), T(1) - m11 + m22 - m33)) / T(2);
            result.v[2] = std::sqrt(std::max(static_cast<T>(0), T(1) - m11 - m22 + m33)) / T(2);
            result.v[3] = std::sqrt(std::max(static_cast<T>(0), T(1) + m11 + m22 + m33)) / T(2);

            // The problem with using copysign: http://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/paul.htm
            result.v[0] = std::copysign(result.v[0], m32 - m23);
            result.v[1] = std::copysign(result.v[1], m13 - m31);
            result.v[2] = std::copysign(result.v[2], m21 - m12);

            result.normalize();

            return result;
        }

        const Matrix operator+(const Matrix& matrix) const noexcept
        {
            Matrix result(*this);
            result.add(matrix, result);
            return result;
        }

        Matrix& operator+=(const Matrix& matrix) noexcept
        {
            add(matrix);
            return *this;
        }

        const Matrix operator-(const Matrix& matrix) const noexcept
        {
            Matrix result(*this);
            result.subtract(matrix, result);
            return result;
        }

        Matrix& operator-=(const Matrix& matrix) noexcept
        {
            subtract(matrix);
            return *this;
        }

        const Matrix operator-() const noexcept
        {
            Matrix result(*this);
            negate(result);
            return result;
        }

        const Matrix operator*(const Matrix& matrix) const noexcept
        {
            Matrix result(*this);
            result.multiply(matrix, result);
            return result;
        }

        Matrix& operator*=(const Matrix& matrix) noexcept
        {
            multiply(matrix);
            return *this;
        }

        bool operator==(const Matrix& matrix) const noexcept
        {
            for (std::size_t i = 0; i < C * R; ++i)
                if (m[i] != matrix.m[i])
                    return false;

            return true;
        }

        bool operator!=(const Matrix& matrix) const noexcept
        {
            for (std::size_t i = 0; i < C * R; ++i)
                if (m[i] != matrix.m[i])
                    return true;

            return false;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        const Vector<T, 3> operator*(const Vector<T, 3>& v) const noexcept
        {
            Vector<T, 3> x;
            transformVector(v, x);
            return x;
        }

        template <std::size_t X = C, std::size_t Y = R, typename std::enable_if<(X == 4 && Y == 4)>::type* = nullptr>
        const Vector<T, 4> operator*(const Vector<T, 4>& v) const noexcept
        {
            Vector<T, 4> x;
            transformVector(v, x);
            return x;
        }
    };

    template <typename T>
    inline Vector<T, 3>& operator*=(Vector<T, 3>& v, const Matrix<T, 4, 4>& m) noexcept
    {
        m.transformVector(v);
        return v;
    }

    template <typename T>
    inline Vector<T, 4>& operator*=(Vector<T, 4>& v, const Matrix<T, 4, 4>& m) noexcept
    {
        m.transformVector(v);
        return v;
    }
}

#endif
