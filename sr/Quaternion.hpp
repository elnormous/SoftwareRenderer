//
//  SoftwareRenderer
//

#pragma once

#include "Vector.hpp"

namespace sr
{
    class Quaternion
    {
    public:
        float x = 0.0F;
        float y = 0.0F;
        float z = 0.0F;
        float w = 0.0F;

        Quaternion()
        {
        }

        Quaternion(float initX, float initY, float initZ, float initW):
            x(initX), y(initY), z(initZ), w(initW)
        {
        }

        Quaternion(const Quaternion& copy):
            x(copy.x), y(copy.y), z(copy.z), w(copy.w)
        {
        }

        Quaternion operator*(const Quaternion& q) const
        {
            return Quaternion( x * q.w + y * q.z - z * q.y + w * q.x,
                              -x * q.z + y * q.w + z * q.x + w * q.y,
                               x * q.y - y * q.x + z * q.w + w * q.z,
                              -x * q.x - y * q.y - z * q.z + w * q.w);
        }

        const Quaternion& operator*=(const Quaternion& q)
        {
            float tempX =  x * q.w + y * q.z - z * q.y + w * q.x;
            float tempY = -x * q.z + y * q.w + z * q.x + w * q.y;
            float tempZ =  x * q.y - y * q.x + z * q.w + w * q.z;
            float tempW = -x * q.x - y * q.y - z * q.z + w * q.w;

            x = tempX;
            y = tempY;
            z = tempZ;
            w = tempW;

            return *this;
        }

        Quaternion operator*(float scalar) const
        {
            return Quaternion(x * scalar,
                              y * scalar,
                              z * scalar,
                              w * scalar);
        }

        const Quaternion& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;

            return *this;
        }

        Quaternion operator/(float scalar) const
        {
            return Quaternion(x / scalar,
                              y / scalar,
                              z / scalar,
                              w / scalar);
        }

        const Quaternion& operator/=(float scalar)
        {
            x /= scalar;
            y /= scalar;
            z /= scalar;
            w /= scalar;

            return *this;
        }

        inline Quaternion operator-() const
        {
            return Quaternion(-x, -y, -z, -w);
        }

        inline Quaternion operator+(const Quaternion& q) const
        {
            Quaternion result(*this);
            result.x += q.x;
            result.y += q.y;
            result.z += q.z;
            result.w += q.w;

            return result;
        }

        inline Quaternion& operator+=(const Quaternion& q)
        {
            x += q.x;
            y += q.y;
            z += q.z;
            w += q.w;

            return *this;
        }

        inline Quaternion operator-(const Quaternion& q) const
        {
            Quaternion result(*this);
            result.x -= q.x;
            result.y -= q.y;
            result.z -= q.z;
            result.w -= q.w;

            return result;
        }

        inline Quaternion& operator-=(const Quaternion& q)
        {
            x -= q.x;
            y -= q.y;
            z -= q.z;
            w -= q.w;

            return *this;
        }

        inline bool operator==(const Quaternion& q) const
        {
            return x == q.x && y == q.y && z == q.z && w == q.w;
        }

        inline bool operator!=(const Quaternion& q) const
        {
            return x != q.x || y != q.y || z != q.z || w != q.w;
        }

        inline void negate()
        {
            x = -x;
            y = -y;
            z = -z;
            w = -w;
        }

        inline void conjugate()
        {
            x = -x;
            y = -y;
            z = -z;
        }

        inline void invert()
        {
            float n2 = x * x + y * y + z * z + w * w; // norm squared

            if (n2 == 0.0F) return;

            // conjugate divided by norm squared
            x = -x / n2;
            y = -y / n2;
            z = -z / n2;
            w = w / n2;
        }

        float getNorm()
        {
            float n = x * x + y * y + z * z + w * w;
            if (n == 1.0F) // already normalized
                return 1.0F;

            return sqrtf(n);
        }

        void normalize()
        {
            float n = x * x + y * y + z * z + w * w;
            if (n == 1.0F) // already normalized
                return;

            n = sqrtf(n);
            if (n < EPSILON) // too close to zero
                return;

            n = 1.0F / n;
            x *= n;
            y *= n;
            z *= n;
            w *= n;
        }

        void rotate(float angle, Vector3F axis)
        {
            axis.normalize();

            float cosAngle = cosf(angle / 2.0F);
            float sinAngle = sinf(angle / 2.0F);

            x = axis.v[0] * sinAngle;
            y = axis.v[1] * sinAngle;
            z = axis.v[2] * sinAngle;
            w = cosAngle;
        }

        void getRotation(float& angle, Vector3F& axis)
        {
            angle = 2.0F * acosf(w);
            float s = sqrtf(1.0F - w * w);
            if (s < EPSILON) // too close to zero
            {
                axis.v[0] = x;
                axis.v[1] = y;
                axis.v[2] = z;
            }
            else
            {
                axis.v[0] = x / s;
                axis.v[1] = y / s;
                axis.v[2] = z / s;
            }
        }

        void setEulerAngles(const Vector3F& angles)
        {
            float angle;

            angle = angles.v[0] * 0.5F;
            const float sr = sinf(angle);
            const float cr = cosf(angle);

            angle = angles.v[1] * 0.5F;
            const float sp = sinf(angle);
            const float cp = cosf(angle);

            angle = angles.v[2] * 0.5F;
            const float sy = sinf(angle);
            const float cy = cosf(angle);

            const float cpcy = cp * cy;
            const float spcy = sp * cy;
            const float cpsy = cp * sy;
            const float spsy = sp * sy;

            x = sr * cpcy - cr * spsy;
            y = cr * spcy + sr * cpsy;
            z = cr * cpsy - sr * spcy;
            w = cr * cpcy + sr * spsy;
        }

        Vector3F getEulerAngles() const
        {
            Vector3F result;

            result.v[0] = atan2f(2.0F * (y * z + w * x), w * w - x * x - y * y + z * z);
            result.v[1] = asinf(-2.0F * (x * z - w * y));
            result.v[2] = atan2f(2.0F * (x * y + w * z), w * w + x * x - y * y - z * z);
            return result;
        }

        float getEulerAngleX() const
        {
            return atan2f(2.0F * (y * z + w * x), w * w - x * x - y * y + z * z);
        }

        float getEulerAngleY() const
        {
            return asinf(-2.0F * (x * z - w * y));
        }

        float getEulerAngleZ() const
        {
            return atan2f(2.0F * (x * y + w * z), w * w + x * x - y * y - z * z);
        }

        inline Vector3F operator*(const Vector3F& vector) const
        {
            return rotateVector(vector);
        }

        inline Vector3F rotateVector(const Vector3F& vector) const
        {
            Vector3F q(x, y, z);
            Vector3F t = 2.0F * Vector3F::cross(q, vector);
            Vector3F result = vector + (w * t) + Vector3F::cross(q, t);
            return result;
        }

        inline Vector3F getRightVector() const
        {
            return rotateVector(Vector3F(1.0F, 0.0F, 0.0F));
        }

        inline Vector3F getUpVector() const
        {
            return rotateVector(Vector3F(0.0F, 1.0F, 0.0F));
        }

        inline Vector3F getForwardVector() const
        {
            return rotateVector(Vector3F(0.0F, 0.0F, 1.0F));
        }

        Quaternion& lerp(const Quaternion& q1, const Quaternion& q2, float t)
        {
            const float scale = 1.0F - t;
            return (*this = (q1 * scale) + (q2 * t));
        }
    };
}
