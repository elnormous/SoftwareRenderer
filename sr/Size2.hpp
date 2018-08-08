//
//  SoftwareRenderer
//

#ifndef SIZE2_H
#define SIZE2_H

#include "Vector.hpp"

namespace sr
{
    class Size2
    {
    public:
        float width = 0.0F;
        float height = 0.0F;

        Size2()
        {
        }

        Size2(float initWidth, float initHeight):
            width(initWidth), height(initHeight)
        {
        }

        Size2(const Vector2F& point):
            width(point.v[0]), height(point.v[1])
        {
        }

        Size2& operator=(const Vector2F& point)
        {
            width = point.v[0];
            height = point.v[1];
            return *this;
        }

        Size2 operator+(const Size2& size) const
        {
            return Size2(width + size.width, height + size.height);
        }

        inline Size2& operator+=(const Size2& size)
        {
            width += size.width;
            height += size.height;
            return *this;
        }

        Size2 operator-(const Size2& size) const
        {
            return Size2(width - size.width, height - size.height);
        }

        inline Size2& operator-=(const Size2& size)
        {
            width -= size.width;
            height -= size.height;
            return *this;
        }

        Size2 operator*(float a) const
        {
            return Size2(width * a, height * a);
        }

        inline Size2& operator*=(float scalar)
        {
            width *= scalar;
            height *= scalar;
            return *this;
        }

        Size2 operator/(float a) const
        {
            return Size2(width / a, height / a);
        }

        inline Size2& operator/=(float scalar)
        {
            width /= scalar;
            height /= scalar;
            return *this;
        }

        inline bool operator==(const Size2& size) const
        {
            return width == size.width && height == size.height;
        }

        inline bool operator!=(const Size2& size) const
        {
            return width != size.width || height != size.height;
        }

        bool isZero() const
        {
            return width == 0.0F && height == 0.0F;
        }

        bool isOne() const
        {
            return width == 1.0F && height == 1.0F;
        }

        float area() const
        {
            return width * height;
        }

        void scale(float scalar)
        {
            width *= scalar;
            height *= scalar;
        }

        void scale(const Vector2F& scale)
        {
            width *= scale.v[0];
            height *= scale.v[1];
        }
    };

    inline Size2 operator*(const Size2& size, const Vector2F& v)
    {
        return Size2(size.width * v.v[0], size.height * v.v[1]);
    }

    inline Size2 operator/(const Size2& size, const Vector2F& v)
    {
        return Size2(size.width / v.v[0], size.height / v.v[1]);
    }
}

#endif
