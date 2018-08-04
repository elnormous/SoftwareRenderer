//
//  SoftwareRenderer
//

#pragma once

#include <algorithm>
#include <cassert>
#include "ConvexVolume.hpp"
#include "Plane.hpp"
#include "Quaternion.hpp"
#include "Vector.hpp"

namespace sr
{
    class Matrix4
    {
    public:
        float m[16];

        Matrix4()
        {
            std::fill(std::begin(m), std::end(m), 0.0F);
        }

        Matrix4(float m11, float m12, float m13, float m14,
                float m21, float m22, float m23, float m24,
                float m31, float m32, float m33, float m34,
                float m41, float m42, float m43, float m44)
        {
            set(m11, m12, m13, m14,
                m21, m22, m23, m24,
                m31, m32, m33, m34,
                m41, m42, m43, m44);
        }

        Matrix4(const float* array)
        {
            set(array);
        }

        Matrix4(const Matrix4& copy)
        {
            std::copy(std::begin(copy.m), std::end(copy.m), m);
        }

        float& operator[](size_t index) { return m[index]; }
        float operator[](size_t index) const { return m[index]; }

        static Matrix4 identity()
        {
            return Matrix4(1.0F, 0.0F, 0.0F, 0.0F,
                           0.0F, 1.0F, 0.0F, 0.0F,
                           0.0F, 0.0F, 1.0F, 0.0F,
                           0.0F, 0.0F, 0.0F, 1.0F);
        }

        static void createLookAt(const Vector3F& eyePosition, const Vector3F& targetPosition, const Vector3F& up, Matrix4& dst)
        {
            createLookAt(eyePosition.v[0], eyePosition.v[1], eyePosition.v[2],
                         targetPosition.v[0], targetPosition.v[1], targetPosition.v[2],
                         up.v[0], up.v[1], up.v[2], dst);
        }

        static void createLookAt(float eyePositionX, float eyePositionY, float eyePositionZ,
                                 float targetPositionX, float targetPositionY, float targetPositionZ,
                                 float upX, float upY, float upZ, Matrix4& dst)
        {
            Vector3F eye(eyePositionX, eyePositionY, eyePositionZ);
            Vector3F target(targetPositionX, targetPositionY, targetPositionZ);
            Vector3F up(upX, upY, upZ);
            up.normalize();

            Vector3F zaxis = target - eye;
            zaxis.normalize();

            Vector3F xaxis = Vector3F::cross(up, zaxis);
            xaxis.normalize();

            Vector3F yaxis = Vector3F::cross(zaxis, xaxis);
            yaxis.normalize();

            dst.m[0] = xaxis.v[0];
            dst.m[1] = yaxis.v[0];
            dst.m[2] = zaxis.v[0];
            dst.m[3] = 0.0F;

            dst.m[4] = xaxis.v[1];
            dst.m[5] = yaxis.v[1];
            dst.m[6] = zaxis.v[1];
            dst.m[7] = 0.0F;

            dst.m[8] = xaxis.v[2];
            dst.m[9] = yaxis.v[2];
            dst.m[10] = zaxis.v[2];
            dst.m[11] = 0.0F;

            dst.m[12] = Vector3F::dot(xaxis, -eye);
            dst.m[13] = Vector3F::dot(yaxis, -eye);
            dst.m[14] = Vector3F::dot(zaxis, -eye);
            dst.m[15] = 1.0F;
        }

        static void createPerspective(float fieldOfView, float aspectRatio, float zNearPlane, float zFarPlane, Matrix4& dst)
        {
            assert(zFarPlane != zNearPlane);

            float theta = fieldOfView * 0.5F;
            if (fabsf(fmodf(theta, PI / 2.0F)) < EPSILON)
            {
                // invalid field of view value
                return;
            }
            float divisor = tanf(theta);
            assert(divisor);
            float factor = 1.0F / divisor;

            dst.setZero();

            assert(aspectRatio);
            dst.m[0] = (1.0F / aspectRatio) * factor;
            dst.m[5] = factor;
            dst.m[10] = zFarPlane / (zFarPlane - zNearPlane);
            dst.m[11] = 1.0F;
            dst.m[14] = -zNearPlane * zFarPlane / (zFarPlane - zNearPlane);
        }

        static void createOrthographicFromSize(float width, float height, float zNearPlane, float zFarPlane, Matrix4& dst)
        {
            float halfWidth = width / 2.0F;
            float halfHeight = height / 2.0F;
            createOrthographicOffCenter(-halfWidth, halfWidth,
                                        -halfHeight, halfHeight,
                                        zNearPlane, zFarPlane, dst);
        }

        static void createOrthographicOffCenter(float left, float right, float bottom, float top,
                                                float zNearPlane, float zFarPlane, Matrix4& dst)
        {
            assert(right != left);
            assert(top != bottom);
            assert(zFarPlane != zNearPlane);

            dst.setZero();

            dst.m[0] = 2.0F / (right - left);
            dst.m[5] = 2.0F / (top - bottom);
            dst.m[10] = 1.0F / (zFarPlane - zNearPlane);
            dst.m[12] = (left + right) / (left - right);
            dst.m[13] = (bottom + top) / (bottom - top);
            dst.m[14] = zNearPlane / (zNearPlane - zFarPlane);
            dst.m[15] = 1.0F;
        }

        static void createScale(const Vector3F& scale, Matrix4& dst)
        {
            dst.setIdentity();

            dst.m[0] = scale.v[0];
            dst.m[5] = scale.v[1];
            dst.m[10] = scale.v[2];
        }

        static void createScale(float xScale, float yScale, float zScale, Matrix4& dst)
        {
            dst.setIdentity();

            dst.m[0] = xScale;
            dst.m[5] = yScale;
            dst.m[10] = zScale;
        }

        static void createRotation(const Vector3F& axis, float angle, Matrix4& dst)
        {
            float x = axis.v[0];
            float y = axis.v[1];
            float z = axis.v[2];

            // Make sure the input axis is normalized
            float n = x * x + y * y + z * z;
            if (n != 1.0F)
            {
                // Not normalized
                n = sqrtf(n);
                // Prevent divide too close to zero
                if (n >= EPSILON)
                {
                    n = 1.0F / n;
                    x *= n;
                    y *= n;
                    z *= n;
                }
            }

            float c = cosf(angle);
            float s = sinf(angle);

            float t = 1.0F - c;
            float tx = t * x;
            float ty = t * y;
            float tz = t * z;
            float txy = tx * y;
            float txz = tx * z;
            float tyz = ty * z;
            float sx = s * x;
            float sy = s * y;
            float sz = s * z;

            dst.m[0] = c + tx * x;
            dst.m[4] = txy - sz;
            dst.m[8] = txz + sy;
            dst.m[12] = 0.0F;

            dst.m[1] = txy + sz;
            dst.m[5] = c + ty * y;
            dst.m[9] = tyz - sx;
            dst.m[13] = 0.0F;

            dst.m[2] = txz - sy;
            dst.m[6] = tyz + sx;
            dst.m[10] = c + tz * z;
            dst.m[14] = 0.0F;

            dst.m[3] = 0.0F;
            dst.m[7] = 0.0F;
            dst.m[11] = 0.0F;
            dst.m[15] = 1.0F;
        }

        static void createRotationX(float angle, Matrix4& dst)
        {
            dst.setIdentity();

            float c = cosf(angle);
            float s = sinf(angle);

            dst.m[5]  = c;
            dst.m[9]  = -s;
            dst.m[6]  = s;
            dst.m[10] = c;
        }

        static void createRotationY(float angle, Matrix4& dst)
        {
            dst.setIdentity();

            float c = cosf(angle);
            float s = sinf(angle);

            dst.m[0]  = c;
            dst.m[8]  = s;
            dst.m[2]  = -s;
            dst.m[10] = c;
        }

        static void createRotationZ(float angle, Matrix4& dst)
        {
            dst.setIdentity();

            float c = cosf(angle);
            float s = sinf(angle);

            dst.m[0] = c;
            dst.m[4] = -s;
            dst.m[1] = s;
            dst.m[5] = c;
        }

        static void createTranslation(const Vector3F& translation, Matrix4& dst)
        {
            dst.setIdentity();

            dst.m[12] = translation.v[0];
            dst.m[13] = translation.v[1];
            dst.m[14] = translation.v[2];
        }

        static void createTranslation(float xTranslation, float yTranslation, float zTranslation, Matrix4& dst)
        {
            dst.setIdentity();

            dst.m[12] = xTranslation;
            dst.m[13] = yTranslation;
            dst.m[14] = zTranslation;
        }

        bool getFrustumLeftPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] + m[0], m[7] + m[4], m[11] + m[8], m[15] + m[12], plane);
        }

        bool getFrustumRightPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] - m[0], m[7] - m[4], m[11] - m[8], m[15] - m[12], plane);
        }

        bool getFrustumBottomPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] + m[1], m[7] + m[5], m[11] + m[9], m[15] + m[13], plane);
        }

        bool getFrustumTopPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] - m[1], m[7] - m[5], m[11] - m[9], m[15] - m[13], plane);
        }

        bool getFrustumNearPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] + m[2], m[7] + m[6], m[11] + m[10], m[15] + m[14], plane);
        }

        bool getFrustumFarPlane(Plane& plane) const
        {
            return Plane::makeFrustumPlane(m[3] - m[2], m[7] - m[6], m[11] - m[10], m[15] - m[14], plane);
        }

        bool getFrustum(ConvexVolume& frustum) const
        {
            frustum.planes.clear();
            Plane plane;

            if (!getFrustumLeftPlane(plane)) return false;
            frustum.planes.push_back(plane);

            if (!getFrustumRightPlane(plane)) return false;
            frustum.planes.push_back(plane);

            if (!getFrustumBottomPlane(plane)) return false;
            frustum.planes.push_back(plane);

            if (!getFrustumTopPlane(plane)) return false;
            frustum.planes.push_back(plane);

            if (!getFrustumNearPlane(plane)) return false;
            frustum.planes.push_back(plane);

            if (!getFrustumFarPlane(plane)) return false;
            frustum.planes.push_back(plane);

            return true;
        }

        void add(float scalar)
        {
            add(scalar, *this);
        }

        void add(float scalar, Matrix4& dst)
        {
            dst.m[0]  = m[0]  + scalar;
            dst.m[1]  = m[1]  + scalar;
            dst.m[2]  = m[2]  + scalar;
            dst.m[3]  = m[3]  + scalar;
            dst.m[4]  = m[4]  + scalar;
            dst.m[5]  = m[5]  + scalar;
            dst.m[6]  = m[6]  + scalar;
            dst.m[7]  = m[7]  + scalar;
            dst.m[8]  = m[8]  + scalar;
            dst.m[9]  = m[9]  + scalar;
            dst.m[10] = m[10] + scalar;
            dst.m[11] = m[11] + scalar;
            dst.m[12] = m[12] + scalar;
            dst.m[13] = m[13] + scalar;
            dst.m[14] = m[14] + scalar;
            dst.m[15] = m[15] + scalar;
        }

        void add(const Matrix4& matrix)
        {
            add(*this, matrix, *this);
        }

        static void add(const Matrix4& m1, const Matrix4& m2, Matrix4& dst)
        {
            dst.m[0]  = m1.m[0]  + m2.m[0];
            dst.m[1]  = m1.m[1]  + m2.m[1];
            dst.m[2]  = m1.m[2]  + m2.m[2];
            dst.m[3]  = m1.m[3]  + m2.m[3];
            dst.m[4]  = m1.m[4]  + m2.m[4];
            dst.m[5]  = m1.m[5]  + m2.m[5];
            dst.m[6]  = m1.m[6]  + m2.m[6];
            dst.m[7]  = m1.m[7]  + m2.m[7];
            dst.m[8]  = m1.m[8]  + m2.m[8];
            dst.m[9]  = m1.m[9]  + m2.m[9];
            dst.m[10] = m1.m[10] + m2.m[10];
            dst.m[11] = m1.m[11] + m2.m[11];
            dst.m[12] = m1.m[12] + m2.m[12];
            dst.m[13] = m1.m[13] + m2.m[13];
            dst.m[14] = m1.m[14] + m2.m[14];
            dst.m[15] = m1.m[15] + m2.m[15];
        }

        float determinant() const
        {
            float a0 = m[0] * m[5] - m[1] * m[4];
            float a1 = m[0] * m[6] - m[2] * m[4];
            float a2 = m[0] * m[7] - m[3] * m[4];
            float a3 = m[1] * m[6] - m[2] * m[5];
            float a4 = m[1] * m[7] - m[3] * m[5];
            float a5 = m[2] * m[7] - m[3] * m[6];
            float b0 = m[8] * m[13] - m[9] * m[12];
            float b1 = m[8] * m[14] - m[10] * m[12];
            float b2 = m[8] * m[15] - m[11] * m[12];
            float b3 = m[9] * m[14] - m[10] * m[13];
            float b4 = m[9] * m[15] - m[11] * m[13];
            float b5 = m[10] * m[15] - m[11] * m[14];

            // Calculate the determinant
            return a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;
        }

        void getUpVector(Vector3F& dst) const
        {
            dst.v[0] = m[4];
            dst.v[1] = m[5];
            dst.v[2] = m[6];
        }

        void getDownVector(Vector3F& dst) const
        {
            dst.v[0] = -m[4];
            dst.v[1] = -m[5];
            dst.v[2] = -m[6];
        }

        void getLeftVector(Vector3F& dst) const
        {
            dst.v[0] = -m[0];
            dst.v[1] = -m[1];
            dst.v[2] = -m[2];
        }

        void getRightVector(Vector3F& dst) const
        {
            dst.v[0] = m[0];
            dst.v[1] = m[1];
            dst.v[2] = m[2];
        }

        void getForwardVector(Vector3F& dst) const
        {
            dst.v[0] = -m[8];
            dst.v[1] = -m[9];
            dst.v[2] = -m[10];
        }

        void getBackVector(Vector3F& dst) const
        {
            dst.v[0] = m[8];
            dst.v[1] = m[9];
            dst.v[2] = m[10];
        }

        bool invert()
        {
            return invert(*this);
        }

        bool invert(Matrix4& dst) const
        {
            float a0 = m[0] * m[5] - m[1] * m[4];
            float a1 = m[0] * m[6] - m[2] * m[4];
            float a2 = m[0] * m[7] - m[3] * m[4];
            float a3 = m[1] * m[6] - m[2] * m[5];
            float a4 = m[1] * m[7] - m[3] * m[5];
            float a5 = m[2] * m[7] - m[3] * m[6];
            float b0 = m[8] * m[13] - m[9] * m[12];
            float b1 = m[8] * m[14] - m[10] * m[12];
            float b2 = m[8] * m[15] - m[11] * m[12];
            float b3 = m[9] * m[14] - m[10] * m[13];
            float b4 = m[9] * m[15] - m[11] * m[13];
            float b5 = m[10] * m[15] - m[11] * m[14];

            // Calculate the determinant
            float det = a0 * b5 - a1 * b4 + a2 * b3 + a3 * b2 - a4 * b1 + a5 * b0;

            // Close to zero, can't invert
            if (fabs(det) < EPSILON)
                return false;

            Matrix4 inverse;
            inverse.m[0]  = m[5] * b5 - m[6] * b4 + m[7] * b3;
            inverse.m[1]  = -m[1] * b5 + m[2] * b4 - m[3] * b3;
            inverse.m[2]  = m[13] * a5 - m[14] * a4 + m[15] * a3;
            inverse.m[3]  = -m[9] * a5 + m[10] * a4 - m[11] * a3;

            inverse.m[4]  = -m[4] * b5 + m[6] * b2 - m[7] * b1;
            inverse.m[5]  = m[0] * b5 - m[2] * b2 + m[3] * b1;
            inverse.m[6]  = -m[12] * a5 + m[14] * a2 - m[15] * a1;
            inverse.m[7]  = m[8] * a5 - m[10] * a2 + m[11] * a1;

            inverse.m[8]  = m[4] * b4 - m[5] * b2 + m[7] * b0;
            inverse.m[9]  = -m[0] * b4 + m[1] * b2 - m[3] * b0;
            inverse.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
            inverse.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;

            inverse.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
            inverse.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
            inverse.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
            inverse.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;

            multiply(inverse, 1.0F / det, dst);

            return true;
        }

        inline bool isIdentity() const
        {
            return m[0]  == 1.0F && m[1]  == 0.0F && m[2]  == 0.0F && m[3]  == 0.0F &&
                   m[4]  == 0.0F && m[5]  == 1.0F && m[6]  == 0.0F && m[7]  == 0.0F &&
                   m[8]  == 0.0F && m[9]  == 0.0F && m[10] == 1.0F && m[11] == 0.0F &&
                   m[12] == 0.0F && m[13] == 0.0F && m[14] == 0.0F && m[15] == 1.0F;
        }

        void multiply(float scalar)
        {
            multiply(scalar, *this);
        }

        void multiply(float scalar, Matrix4& dst) const
        {
            multiply(*this, scalar, dst);
        }

        static void multiply(const Matrix4& m, float scalar, Matrix4& dst)
        {
            dst.m[0]  = m.m[0]  * scalar;
            dst.m[1]  = m.m[1]  * scalar;
            dst.m[2]  = m.m[2]  * scalar;
            dst.m[3]  = m.m[3]  * scalar;
            dst.m[4]  = m.m[4]  * scalar;
            dst.m[5]  = m.m[5]  * scalar;
            dst.m[6]  = m.m[6]  * scalar;
            dst.m[7]  = m.m[7]  * scalar;
            dst.m[8]  = m.m[8]  * scalar;
            dst.m[9]  = m.m[9]  * scalar;
            dst.m[10] = m.m[10] * scalar;
            dst.m[11] = m.m[11] * scalar;
            dst.m[12] = m.m[12] * scalar;
            dst.m[13] = m.m[13] * scalar;
            dst.m[14] = m.m[14] * scalar;
            dst.m[15] = m.m[15] * scalar;
        }

        void multiply(const Matrix4& matrix)
        {
            multiply(*this, matrix, *this);
        }

        static void multiply(const Matrix4& m1, const Matrix4& m2, Matrix4& dst)
        {
            float product[16];

            product[0]  = m1.m[0] * m2.m[0]  + m1.m[4] * m2.m[1] + m1.m[8]   * m2.m[2]  + m1.m[12] * m2.m[3];
            product[1]  = m1.m[1] * m2.m[0]  + m1.m[5] * m2.m[1] + m1.m[9]   * m2.m[2]  + m1.m[13] * m2.m[3];
            product[2]  = m1.m[2] * m2.m[0]  + m1.m[6] * m2.m[1] + m1.m[10]  * m2.m[2]  + m1.m[14] * m2.m[3];
            product[3]  = m1.m[3] * m2.m[0]  + m1.m[7] * m2.m[1] + m1.m[11]  * m2.m[2]  + m1.m[15] * m2.m[3];

            product[4]  = m1.m[0] * m2.m[4]  + m1.m[4] * m2.m[5] + m1.m[8]   * m2.m[6]  + m1.m[12] * m2.m[7];
            product[5]  = m1.m[1] * m2.m[4]  + m1.m[5] * m2.m[5] + m1.m[9]   * m2.m[6]  + m1.m[13] * m2.m[7];
            product[6]  = m1.m[2] * m2.m[4]  + m1.m[6] * m2.m[5] + m1.m[10]  * m2.m[6]  + m1.m[14] * m2.m[7];
            product[7]  = m1.m[3] * m2.m[4]  + m1.m[7] * m2.m[5] + m1.m[11]  * m2.m[6]  + m1.m[15] * m2.m[7];

            product[8]  = m1.m[0] * m2.m[8]  + m1.m[4] * m2.m[9] + m1.m[8]   * m2.m[10] + m1.m[12] * m2.m[11];
            product[9]  = m1.m[1] * m2.m[8]  + m1.m[5] * m2.m[9] + m1.m[9]   * m2.m[10] + m1.m[13] * m2.m[11];
            product[10] = m1.m[2] * m2.m[8]  + m1.m[6] * m2.m[9] + m1.m[10]  * m2.m[10] + m1.m[14] * m2.m[11];
            product[11] = m1.m[3] * m2.m[8]  + m1.m[7] * m2.m[9] + m1.m[11]  * m2.m[10] + m1.m[15] * m2.m[11];

            product[12] = m1.m[0] * m2.m[12] + m1.m[4] * m2.m[13] + m1.m[8]  * m2.m[14] + m1.m[12] * m2.m[15];
            product[13] = m1.m[1] * m2.m[12] + m1.m[5] * m2.m[13] + m1.m[9]  * m2.m[14] + m1.m[13] * m2.m[15];
            product[14] = m1.m[2] * m2.m[12] + m1.m[6] * m2.m[13] + m1.m[10] * m2.m[14] + m1.m[14] * m2.m[15];
            product[15] = m1.m[3] * m2.m[12] + m1.m[7] * m2.m[13] + m1.m[11] * m2.m[14] + m1.m[15] * m2.m[15];

            std::copy(std::begin(product), std::end(product), dst.m);
        }

        void negate()
        {
            negate(*this);
        }

        void negate(Matrix4& dst) const
        {
            dst.m[0]  = -m[0];
            dst.m[1]  = -m[1];
            dst.m[2]  = -m[2];
            dst.m[3]  = -m[3];
            dst.m[4]  = -m[4];
            dst.m[5]  = -m[5];
            dst.m[6]  = -m[6];
            dst.m[7]  = -m[7];
            dst.m[8]  = -m[8];
            dst.m[9]  = -m[9];
            dst.m[10] = -m[10];
            dst.m[11] = -m[11];
            dst.m[12] = -m[12];
            dst.m[13] = -m[13];
            dst.m[14] = -m[14];
            dst.m[15] = -m[15];
        }

        void rotate(const Vector3F& axis, float angle)
        {
            rotate(axis, angle, *this);
        }

        void rotate(const Vector3F& axis, float angle, Matrix4& dst) const
        {
            Matrix4 r;
            createRotation(axis, angle, r);
            multiply(*this, r, dst);
        }

        void rotateX(float angle)
        {
            rotateX(angle, *this);
        }

        void rotateX(float angle, Matrix4& dst) const
        {
            Matrix4 r;
            createRotationX(angle, r);
            multiply(*this, r, dst);
        }

        void rotateY(float angle)
        {
            rotateY(angle, *this);
        }

        void rotateY(float angle, Matrix4& dst) const
        {
            Matrix4 r;
            createRotationY(angle, r);
            multiply(*this, r, dst);
        }

        void rotateZ(float angle)
        {
            rotateZ(angle, *this);
        }

        void rotateZ(float angle, Matrix4& dst) const
        {
            Matrix4 r;
            createRotationZ(angle, r);
            multiply(*this, r, dst);
        }

        void scale(float value)
        {
            scale(value, *this);
        }

        void scale(float value, Matrix4& dst) const
        {
            scale(value, value, value, dst);
        }

        void scale(float xScale, float yScale, float zScale)
        {
            scale(xScale, yScale, zScale, *this);
        }

        void scale(float xScale, float yScale, float zScale, Matrix4& dst) const
        {
            Matrix4 s;
            createScale(xScale, yScale, zScale, s);
            multiply(*this, s, dst);
        }

        void scale(const Vector3F& s)
        {
            scale(s.v[0], s.v[1], s.v[2], *this);
        }

        void scale(const Vector3F& s, Matrix4& dst) const
        {
            scale(s.v[0], s.v[1], s.v[2], dst);
        }

        void set(float m11, float m12, float m13, float m14,
                 float m21, float m22, float m23, float m24,
                 float m31, float m32, float m33, float m34,
                 float m41, float m42, float m43, float m44)
        {
            m[0]  = m11;
            m[1]  = m21;
            m[2]  = m31;
            m[3]  = m41;
            m[4]  = m12;
            m[5]  = m22;
            m[6]  = m32;
            m[7]  = m42;
            m[8]  = m13;
            m[9]  = m23;
            m[10] = m33;
            m[11] = m43;
            m[12] = m14;
            m[13] = m24;
            m[14] = m34;
            m[15] = m44;
        }

        void set(const float* array)
        {
            assert(array);
            std::copy(array, array + sizeof(m) / sizeof(float), m);
        }

        inline void setIdentity()
        {
            m[0]  = 1.0F; m[1] = 0.0F;  m[2]  = 0.0F; m[3]  = 0.0F;
            m[4]  = 0.0F; m[5] = 1.0F;  m[6]  = 0.0F; m[7]  = 0.0F;
            m[8]  = 0.0F; m[9] = 0.0F;  m[10] = 1.0F; m[11] = 0.0F;
            m[12] = 0.0F; m[13] = 0.0F; m[14] = 0.0F; m[15] = 1.0F;
        }

        inline void setZero()
        {
            std::fill(m, m + sizeof(m) / sizeof(float), 0.0F);
        }

        void subtract(const Matrix4& matrix)
        {
            subtract(*this, matrix, *this);
        }

        static void subtract(const Matrix4& m1, const Matrix4& m2, Matrix4& dst)
        {
            dst.m[0]  = m1.m[0]  - m2.m[0];
            dst.m[1]  = m1.m[1]  - m2.m[1];
            dst.m[2]  = m1.m[2]  - m2.m[2];
            dst.m[3]  = m1.m[3]  - m2.m[3];
            dst.m[4]  = m1.m[4]  - m2.m[4];
            dst.m[5]  = m1.m[5]  - m2.m[5];
            dst.m[6]  = m1.m[6]  - m2.m[6];
            dst.m[7]  = m1.m[7]  - m2.m[7];
            dst.m[8]  = m1.m[8]  - m2.m[8];
            dst.m[9]  = m1.m[9]  - m2.m[9];
            dst.m[10] = m1.m[10] - m2.m[10];
            dst.m[11] = m1.m[11] - m2.m[11];
            dst.m[12] = m1.m[12] - m2.m[12];
            dst.m[13] = m1.m[13] - m2.m[13];
            dst.m[14] = m1.m[14] - m2.m[14];
            dst.m[15] = m1.m[15] - m2.m[15];
        }

        void transformPoint(Vector3F& point) const
        {
            transformVector(point.v[0], point.v[1], point.v[2], 1.0F, point);
        }

        void transformPoint(const Vector3F& point, Vector3F& dst) const
        {
            transformVector(point.v[0], point.v[1], point.v[2], 1.0F, dst);
        }

        void transformVector(Vector3F& vector) const
        {
            Vector4F t;
            transformVector(Vector4F(vector.v[0], vector.v[1], vector.v[2], 0.0F), t);
            vector = Vector3F(t.v[0], t.v[1], t.v[2]);
        }

        void transformVector(const Vector3F& vector, Vector3F& dst) const
        {
            transformVector(vector.v[0], vector.v[1], vector.v[2], 0.0F, dst);
        }

        void transformVector(float x, float y, float z, float w, Vector3F& dst) const
        {
            Vector4F t;
            transformVector(Vector4F(x, y, z, w), t);
            dst = Vector3F(t.v[0], t.v[1], t.v[2]);
        }

        void transformVector(Vector4F& vector) const
        {
            transformVector(vector, vector);
        }

        void transformVector(const Vector4F& vector, Vector4F& dst) const
        {
            assert(&vector != &dst);

            dst.v[0] = vector.v[0] * m[0] + vector.v[1] * m[4] + vector.v[2] * m[8] + vector.v[3] * m[12];
            dst.v[1] = vector.v[0] * m[1] + vector.v[1] * m[5] + vector.v[2] * m[9] + vector.v[3] * m[13];
            dst.v[2] = vector.v[0] * m[2] + vector.v[1] * m[6] + vector.v[2] * m[10] + vector.v[3] * m[14];
            dst.v[3] = vector.v[0] * m[3] + vector.v[1] * m[7] + vector.v[2] * m[11] + vector.v[3] * m[15];
        }

        void translate(float x, float y, float z)
        {
            translate(x, y, z, *this);
        }

        void translate(float x, float y, float z, Matrix4& dst) const
        {
            Matrix4 t;
            createTranslation(x, y, z, t);
            multiply(*this, t, dst);
        }

        void translate(const Vector3F& t)
        {
            translate(t.v[0], t.v[1], t.v[2], *this);
        }

        void translate(const Vector3F& t, Matrix4& dst) const
        {
            translate(t.v[0], t.v[1], t.v[2], dst);
        }

        void transpose()
        {
            transpose(*this);
        }

        void transpose(Matrix4& dst) const
        {
            float t[16] = {
                m[0], m[4], m[8], m[12],
                m[1], m[5], m[9], m[13],
                m[2], m[6], m[10], m[14],
                m[3], m[7], m[11], m[15]
            };
            std::copy(std::begin(t), std::end(t), dst.m);
        }

        Vector3F getTranslation() const
        {
            return Vector3F(m[12], m[13], m[14]);
        }

        Vector3F getScale() const
        {
            Vector3F scale;
            scale.v[0] = Vector3F(m[0], m[1], m[2]).length();
            scale.v[1] = Vector3F(m[4], m[5], m[6]).length();
            scale.v[2] = Vector3F(m[8], m[9], m[10]).length();

            return scale;
        }

        Quaternion getRotation() const
        {
            Vector3F scale = getScale();

            float m11 = m[0] / scale.v[0];
            float m21 = m[1] / scale.v[0];
            float m31 = m[2] / scale.v[0];

            float m12 = m[4] / scale.v[1];
            float m22 = m[5] / scale.v[1];
            float m32 = m[6] / scale.v[1];

            float m13 = m[8] / scale.v[2];
            float m23 = m[9] / scale.v[2];
            float m33 = m[10] / scale.v[2];

            Quaternion result;
            result.x = sqrtf(std::max(0.0F, 1 + m11 - m22 - m33)) / 2.0F;
            result.y = sqrtf(std::max(0.0F, 1 - m11 + m22 - m33)) / 2.0F;
            result.z = sqrtf(std::max(0.0F, 1 - m11 - m22 + m33)) / 2.0F;
            result.w = sqrtf(std::max(0.0F, 1 + m11 + m22 + m33)) / 2.0F;

            result.x *= sgn(result.x * (m32 - m23));
            result.y *= sgn(result.y * (m13 - m31));
            result.z *= sgn(result.z * (m21 - m12));

            result.normalize();

            return result;
        }

        void setRotation(const Quaternion& rotation)
        {
            Matrix4 result;

            float wx = rotation.w * rotation.x;
            float wy = rotation.w * rotation.y;
            float wz = rotation.w * rotation.z;

            float xx = rotation.x * rotation.x;
            float xy = rotation.x * rotation.y;
            float xz = rotation.x * rotation.z;

            float yy = rotation.y * rotation.y;
            float yz = rotation.y * rotation.z;

            float zz = rotation.z * rotation.z;

            m[0] = 1.0F - 2.0F * (yy + zz);
            m[4] = 2.0F * (xy - wz);
            m[8] = 2.0F * (xz + wy);
            m[12] = 0.0F;

            m[1] = 2.0F * (xy + wz);
            m[5] = 1.0F - 2.0F * (xx + zz);
            m[9] = 2.0F * (yz - wx);
            m[13] = 0.0F;

            m[2] = 2.0F * (xz - wy);
            m[6] = 2.0F * (yz + wx);
            m[10] = 1.0F - 2.0F * (xx + yy);
            m[14] = 0.0F;

            m[3] = 0.0F;
            m[7] = 0.0F;
            m[11] = 0.0F;
            m[15] = 1.0F;
        }

        inline Matrix4 operator+(const Matrix4& matrix) const
        {
            Matrix4 result(*this);
            add(result, matrix, result);
            return result;
        }

        inline Matrix4& operator+=(const Matrix4& matrix)
        {
            add(matrix);
            return *this;
        }

        inline Matrix4 operator-(const Matrix4& matrix) const
        {
            Matrix4 result(*this);
            subtract(result, matrix, result);
            return result;
        }

        inline Matrix4& operator-=(const Matrix4& matrix)
        {
            subtract(matrix);
            return *this;
        }

        inline Matrix4 operator-() const
        {
            Matrix4 result(*this);
            negate(result);
            return result;
        }

        inline Matrix4 operator*(const Matrix4& matrix) const
        {
            Matrix4 result(*this);
            multiply(result, matrix, result);
            return result;
        }

        inline Matrix4& operator*=(const Matrix4& matrix)
        {
            multiply(matrix);
            return *this;
        }

        inline bool operator==(const Matrix4& matrix)
        {
            return m[0] == matrix.m[0] &&
                   m[1] == matrix.m[1] &&
                   m[2] == matrix.m[2] &&
                   m[3] == matrix.m[3] &&
                   m[4] == matrix.m[4] &&
                   m[5] == matrix.m[5] &&
                   m[6] == matrix.m[6] &&
                   m[7] == matrix.m[7] &&
                   m[8] == matrix.m[8] &&
                   m[9] == matrix.m[9] &&
                   m[10] == matrix.m[10] &&
                   m[11] == matrix.m[11] &&
                   m[12] == matrix.m[12] &&
                   m[13] == matrix.m[13] &&
                   m[14] == matrix.m[14] &&
                   m[15] == matrix.m[15];
        }

        inline bool operator!=(const Matrix4& matrix)
        {
            return m[0] != matrix.m[0] ||
                   m[1] != matrix.m[1] ||
                   m[2] != matrix.m[2] ||
                   m[3] != matrix.m[3] ||
                   m[4] != matrix.m[4] ||
                   m[5] != matrix.m[5] ||
                   m[6] != matrix.m[6] ||
                   m[7] != matrix.m[7] ||
                   m[8] != matrix.m[8] ||
                   m[9] != matrix.m[9] ||
                   m[10] != matrix.m[10] ||
                   m[11] != matrix.m[11] ||
                   m[12] != matrix.m[12] ||
                   m[13] != matrix.m[13] ||
                   m[14] != matrix.m[14] ||
                   m[15] != matrix.m[15];
        }
    };

    inline Vector3F& operator*=(Vector3F& v, const Matrix4& m)
    {
        m.transformVector(v);
        return v;
    }

    inline Vector3F operator*(const Matrix4& m, const Vector3F& v)
    {
        Vector3F x;
        m.transformVector(v, x);
        return x;
    }

    inline Vector4F& operator*=(Vector4F& v, const Matrix4& m)
    {
        m.transformVector(v);
        return v;
    }

    inline Vector4F operator*(const Matrix4& m, const Vector4F& v)
    {
        Vector4F x;
        m.transformVector(v, x);
        return x;
    }
}
