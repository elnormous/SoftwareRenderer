//
//  SoftwareRenderer
//

#pragma once

#include "Vector.hpp"

namespace sr
{
    class Size3
    {
    public:
        float width = 0.0F;
        float height = 0.0F;
        float depth = 0.0F;

        Size3()
        {
        }

        Size3(float initWidth, float initHeight, float initDepth):
            width(initWidth), height(initHeight), depth(initDepth)
        {
        }

        Size3(const Vector3F& point):
            width(point.v[0]), height(point.v[1]), depth(point.v[2])
        {
        }

        Size3& operator=(const Vector3F& point)
        {
            width = point.v[0];
            height = point.v[1];
            depth = point.v[2];
            return *this;
        }

        Size3 operator+(const Size3& size) const
        {
            return Size3(width + size.width, height + size.height, depth + size.depth);
        }

        inline Size3& operator+=(const Size3& size)
        {
            width += size.width;
            height += size.height;
            depth += size.depth;
            return *this;
        }

        Size3 operator-(const Size3& size) const
        {
            return Size3(width - size.width, height - size.height, depth - size.depth);
        }

        inline Size3& operator-=(const Size3& size)
        {
            width -= size.width;
            height -= size.height;
            depth -= size.depth;
            return *this;
        }

        Size3 operator*(float a) const
        {
            return Size3(width * a, height * a, depth * a);
        }

        inline Size3& operator*=(float scalar)
        {
            width *= scalar;
            height *= scalar;
            depth *= scalar;
            return *this;
        }

        Size3 operator/(float a) const
        {
            return Size3(width / a, height / a, depth / a);
        }

        inline Size3& operator/=(float scalar)
        {
            width /= scalar;
            height /= scalar;
            depth /= scalar;
            return *this;
        }

        inline bool operator==(const Size3& size) const
        {
            return width == size.width && height == size.height && depth == size.depth;
        }

        inline bool operator!=(const Size3& size) const
        {
            return width != size.width || height != size.height || depth != size.depth;
        }

        bool isZero() const
        {
            return width == 0.0F && height == 0.0F && depth == 0.0F;
        }

        bool isOne() const
        {
            return width == 1.0F && height == 1.0F && depth == 1.0F;
        }

        float volume() const
        {
            return width * height * depth;
        }

        void scale(float scalar)
        {
            width *= scalar;
            height *= scalar;
            depth *= scalar;
        }

        void scale(const Vector3F& scale)
        {
            width *= scale.v[0];
            height *= scale.v[1];
            depth *= scale.v[2];
        }
    };

    inline Size3 operator*(const Size3& size, const Vector3F& v)
    {
        return Size3(size.width * v.v[0], size.height * v.v[1], size.depth * v.v[2]);
    }

    inline Size3 operator/(const Size3& size, const Vector3F& v)
    {
        return Size3(size.width / v.v[0], size.height / v.v[1], size.depth / v.v[2]);
    }
}
