//
//  SoftwareRenderer
//

#include <algorithm>
#include <cmath>
#include <cassert>
#include "Matrix4.hpp"
#include "Quaternion.hpp"
#include "MathUtils.hpp"

namespace sr
{
    const Matrix4 Matrix4::IDENTITY(1.0F, 0.0F, 0.0F, 0.0F,
                                    0.0F, 1.0F, 0.0F, 0.0F,
                                    0.0F, 0.0F, 1.0F, 0.0F,
                                    0.0F, 0.0F, 0.0F, 1.0F);

    const Matrix4 Matrix4::ZERO(0.0F, 0.0F, 0.0F, 0.0F,
                                0.0F, 0.0F, 0.0F, 0.0F,
                                0.0F, 0.0F, 0.0F, 0.0F,
                                0.0F, 0.0F, 0.0F, 0.0F);

    Matrix4::Matrix4(float pm11, float pm12, float pm13, float pm14,
                     float pm21, float pm22, float pm23, float pm24,
                     float pm31, float pm32, float pm33, float pm34,
                     float pm41, float pm42, float pm43, float pm44)
    {
        set(pm11, pm12, pm13, pm14,
            pm21, pm22, pm23, pm24,
            pm31, pm32, pm33, pm34,
            pm41, pm42, pm43, pm44);
    }

    Matrix4::Matrix4(const float* array)
    {
        set(array);
    }

    Matrix4::Matrix4(const Matrix4& copy)
    {
        std::copy(std::begin(copy.m), std::end(copy.m), m);
    }

    void Matrix4::createLookAt(const Vector3F& eyePosition,
                               const Vector3F& targetPosition,
                               const Vector3F& up,
                               Matrix4& dst)
    {
        createLookAt(eyePosition.v[0], eyePosition.v[1], eyePosition.v[2],
                     targetPosition.v[0], targetPosition.v[1], targetPosition.v[2],
                     up.v[0], up.v[1], up.v[2], dst);
    }

    void Matrix4::createLookAt(float eyePositionX, float eyePositionY, float eyePositionZ,
                               float targetPositionX, float targetPositionY, float targetPositionZ,
                               float upX, float upY, float upZ,
                               Matrix4& dst)
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

    void Matrix4::createPerspective(float fieldOfView, float aspectRatio,
                                    float zNearPlane, float zFarPlane,
                                    Matrix4& dst)
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

    void Matrix4::createOrthographicFromSize(float width, float height,
                                             float zNearPlane, float zFarPlane,
                                             Matrix4& dst)
    {
        float halfWidth = width / 2.0F;
        float halfHeight = height / 2.0F;
        createOrthographicOffCenter(-halfWidth, halfWidth,
                                    -halfHeight, halfHeight,
                                    zNearPlane, zFarPlane, dst);
    }

    void Matrix4::createOrthographicOffCenter(float left, float right, float bottom, float top,
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

    void Matrix4::createScale(const Vector3F& scale, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        dst.m[0] = scale.v[0];
        dst.m[5] = scale.v[1];
        dst.m[10] = scale.v[2];
    }

    void Matrix4::createScale(float xScale, float yScale, float zScale, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        dst.m[0] = xScale;
        dst.m[5] = yScale;
        dst.m[10] = zScale;
    }

    void Matrix4::createRotation(const Vector3F& axis, float angle, Matrix4& dst)
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

    void Matrix4::createRotationX(float angle, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        float c = cosf(angle);
        float s = sinf(angle);

        dst.m[5]  = c;
        dst.m[9]  = -s;
        dst.m[6]  = s;
        dst.m[10] = c;
    }

    void Matrix4::createRotationY(float angle, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        float c = cosf(angle);
        float s = sinf(angle);

        dst.m[0]  = c;
        dst.m[8]  = s;
        dst.m[2]  = -s;
        dst.m[10] = c;
    }

    void Matrix4::createRotationZ(float angle, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        float c = cosf(angle);
        float s = sinf(angle);

        dst.m[0] = c;
        dst.m[4] = -s;
        dst.m[1] = s;
        dst.m[5] = c;
    }

    void Matrix4::createTranslation(const Vector3F& translation, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        dst.m[12] = translation.v[0];
        dst.m[13] = translation.v[1];
        dst.m[14] = translation.v[2];
    }

    void Matrix4::createTranslation(float xTranslation, float yTranslation, float zTranslation, Matrix4& dst)
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), dst.m);

        dst.m[12] = xTranslation;
        dst.m[13] = yTranslation;
        dst.m[14] = zTranslation;
    }

    void Matrix4::add(float scalar)
    {
        add(scalar, *this);
    }

    void Matrix4::add(float scalar, Matrix4& dst)
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

    void Matrix4::add(const Matrix4& matrix)
    {
        add(*this, matrix, *this);
    }

    void Matrix4::add(const Matrix4& m1, const Matrix4& m2, Matrix4& dst)
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

    float Matrix4::determinant() const
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

    void Matrix4::getUpVector(Vector3F& dst) const
    {
        dst.v[0] = m[4];
        dst.v[1] = m[5];
        dst.v[2] = m[6];
    }

    void Matrix4::getDownVector(Vector3F& dst) const
    {
        dst.v[0] = -m[4];
        dst.v[1] = -m[5];
        dst.v[2] = -m[6];
    }

    void Matrix4::getLeftVector(Vector3F& dst) const
    {
        dst.v[0] = -m[0];
        dst.v[1] = -m[1];
        dst.v[2] = -m[2];
    }

    void Matrix4::getRightVector(Vector3F& dst) const
    {
        dst.v[0] = m[0];
        dst.v[1] = m[1];
        dst.v[2] = m[2];
    }

    void Matrix4::getForwardVector(Vector3F& dst) const
    {
        dst.v[0] = -m[8];
        dst.v[1] = -m[9];
        dst.v[2] = -m[10];
    }

    void Matrix4::getBackVector(Vector3F& dst) const
    {
        dst.v[0] = m[8];
        dst.v[1] = m[9];
        dst.v[2] = m[10];
    }

    bool Matrix4::invert()
    {
        return invert(*this);
    }

    bool Matrix4::invert(Matrix4& dst) const
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

    bool Matrix4::isIdentity() const
    {
        return std::equal(m, m + sizeof(m) / sizeof(float), IDENTITY.m);
    }

    void Matrix4::multiply(float scalar)
    {
        multiply(scalar, *this);
    }

    void Matrix4::multiply(float scalar, Matrix4& dst) const
    {
        multiply(*this, scalar, dst);
    }

    void Matrix4::multiply(const Matrix4& m, float scalar, Matrix4& dst)
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

    void Matrix4::multiply(const Matrix4& matrix)
    {
        multiply(*this, matrix, *this);
    }

    void Matrix4::multiply(const Matrix4& m1, const Matrix4& m2, Matrix4& dst)
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

    void Matrix4::negate()
    {
        negate(*this);
    }

    void Matrix4::negate(Matrix4& dst) const
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

    void Matrix4::rotate(const Vector3F& axis, float angle)
    {
        rotate(axis, angle, *this);
    }

    void Matrix4::rotate(const Vector3F& axis, float angle, Matrix4& dst) const
    {
        Matrix4 r;
        createRotation(axis, angle, r);
        multiply(*this, r, dst);
    }

    void Matrix4::rotateX(float angle)
    {
        rotateX(angle, *this);
    }

    void Matrix4::rotateX(float angle, Matrix4& dst) const
    {
        Matrix4 r;
        createRotationX(angle, r);
        multiply(*this, r, dst);
    }

    void Matrix4::rotateY(float angle)
    {
        rotateY(angle, *this);
    }

    void Matrix4::rotateY(float angle, Matrix4& dst) const
    {
        Matrix4 r;
        createRotationY(angle, r);
        multiply(*this, r, dst);
    }

    void Matrix4::rotateZ(float angle)
    {
        rotateZ(angle, *this);
    }

    void Matrix4::rotateZ(float angle, Matrix4& dst) const
    {
        Matrix4 r;
        createRotationZ(angle, r);
        multiply(*this, r, dst);
    }

    void Matrix4::scale(float value)
    {
        scale(value, *this);
    }

    void Matrix4::scale(float value, Matrix4& dst) const
    {
        scale(value, value, value, dst);
    }

    void Matrix4::scale(float xScale, float yScale, float zScale)
    {
        scale(xScale, yScale, zScale, *this);
    }

    void Matrix4::scale(float xScale, float yScale, float zScale, Matrix4& dst) const
    {
        Matrix4 s;
        createScale(xScale, yScale, zScale, s);
        multiply(*this, s, dst);
    }

    void Matrix4::scale(const Vector3F& s)
    {
        scale(s.v[0], s.v[1], s.v[2], *this);
    }

    void Matrix4::scale(const Vector3F& s, Matrix4& dst) const
    {
        scale(s.v[0], s.v[1], s.v[2], dst);
    }

    void Matrix4::set(float m11, float m12, float m13, float m14,
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

    void Matrix4::set(const float* array)
    {
        assert(array);
        std::copy(array, array + sizeof(m) / sizeof(float), m);
    }

    void Matrix4::setIdentity()
    {
        std::copy(std::begin(IDENTITY.m), std::end(IDENTITY.m), m);
    }

    void Matrix4::setZero()
    {
        std::fill(m, m + sizeof(m) / sizeof(float), 0.0F);
    }

    void Matrix4::subtract(const Matrix4& matrix)
    {
        subtract(*this, matrix, *this);
    }

    void Matrix4::subtract(const Matrix4& m1, const Matrix4& m2, Matrix4& dst)
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

    void Matrix4::transformVector(const Vector4F& vector, Vector4F& dst) const
    {
        assert(&vector != &dst);

        dst.v[0] = vector.v[0] * m[0] + vector.v[1] * m[4] + vector.v[2] * m[8] + vector.v[3] * m[12];
        dst.v[1] = vector.v[0] * m[1] + vector.v[1] * m[5] + vector.v[2] * m[9] + vector.v[3] * m[13];
        dst.v[2] = vector.v[0] * m[2] + vector.v[1] * m[6] + vector.v[2] * m[10] + vector.v[3] * m[14];
        dst.v[3] = vector.v[0] * m[3] + vector.v[1] * m[7] + vector.v[2] * m[11] + vector.v[3] * m[15];
    }

    void Matrix4::translate(float x, float y, float z)
    {
        translate(x, y, z, *this);
    }

    void Matrix4::translate(float x, float y, float z, Matrix4& dst) const
    {
        Matrix4 t;
        createTranslation(x, y, z, t);
        multiply(*this, t, dst);
    }

    void Matrix4::translate(const Vector3F& t)
    {
        translate(t.v[0], t.v[1], t.v[2], *this);
    }

    void Matrix4::translate(const Vector3F& t, Matrix4& dst) const
    {
        translate(t.v[0], t.v[1], t.v[2], dst);
    }

    void Matrix4::transpose()
    {
        transpose(*this);
    }

    void Matrix4::transpose(Matrix4& dst) const
    {
        float t[16] = {
            m[0], m[4], m[8], m[12],
            m[1], m[5], m[9], m[13],
            m[2], m[6], m[10], m[14],
            m[3], m[7], m[11], m[15]
        };
        std::copy(std::begin(t), std::end(t), dst.m);
    }

    Vector3F Matrix4::getTranslation() const
    {
        return Vector3F(m[12], m[13], m[14]);
    }

    Vector3F Matrix4::getScale() const
    {
        Vector3F scale;
        scale.v[0] = Vector3F(m[0], m[1], m[2]).length();
        scale.v[1] = Vector3F(m[4], m[5], m[6]).length();
        scale.v[2] = Vector3F(m[8], m[9], m[10]).length();

        return scale;
    }

    Quaternion Matrix4::getRotation() const
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
}
