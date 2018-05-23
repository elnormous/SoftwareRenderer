//
//  SoftwareRenderer
//

#pragma once

#include "Vector.hpp"
#include "Size2.hpp"

namespace sr
{
    class Rect
    {
    public:
        Vector2F position;
        Size2 size;

        Rect()
        {
        }

        Rect(float width, float height):
            size(width, height)
        {
        }

        Rect(float initX, float initY, float width, float height):
            position(initX, initY), size(width, height)
        {
        }

        Rect(const Vector2F& initPosition, float width, float height):
            position(initPosition), size(width, height)
        {
        }

        Rect(const Vector2F& initPosition, const Size2& initSize):
            position(initPosition), size(initSize)
        {
        }

        Rect(const Rect& copy):
            position(copy.position), size(copy.size)
        {
        }

        inline bool isEmpty() const
        {
            return size.isZero();
        }

        void set(float newX, float newY, float newWidth, float newHeight)
        {
            position.v[0] = newX;
            position.v[1] = newY;
            size.width = newWidth;
            size.height = newHeight;
        }

        void set(const Vector2F& newPosition, float newWidth, float newHeight)
        {
            position = newPosition;
            size.width = newWidth;
            size.height = newHeight;
        }

        void setPosition(float newX, float newY)
        {
            position.v[0] = newX;
            position.v[1] = newY;
        }

        void setPosition(const Vector2F& newPosition)
        {
            position = newPosition;
        }

        float left() const
        {
            return position.v[0];
        }

        float bottom() const
        {
            return position.v[1];
        }

        float right() const
        {
            return position.v[0] + size.width;
        }

        float top() const
        {
            return position.v[1] + size.height;
        }

        Vector2F bottomLeft() const
        {
            return position;
        }

        Vector2F topRight() const
        {
            return Vector2F(position.v[0] + size.width, position.v[1] + size.height);
        }

        bool containsPoint(float x, float y) const
        {
            return x >= position.v[0] && x <= (position.v[0] + size.width) &&
                y >= position.v[1] && y <= (position.v[1] + size.height);
        }

        bool containsPoint(const Vector2F& point) const
        {
            return point.v[0] >= position.v[0] && point.v[0] <= (position.v[0] + size.width) &&
                point.v[1] >= position.v[1] && point.v[1] <= (position.v[1] + size.height);
        }

        bool contains(float x, float y, float width, float height) const
        {
            return containsPoint(x, y) && containsPoint(x + width, y + height);
        }

        bool contains(const Rect& r) const
        {
            return contains(r.position.v[0], r.position.v[1], r.size.width, r.size.height);
        }

        bool intersects(float x, float y, float width, float height) const
        {
            float t;
            if ((t = x - position.v[0]) > size.width || -t > width)
                return false;
            if ((t = y - position.v[1]) > size.height || -t > height)
                return false;
            return true;
        }

        bool intersects(const Rect& r) const
        {
            return intersects(r.position.v[0], r.position.v[1], r.size.width, r.size.height);
        }

        static bool intersect(const Rect& r1, const Rect& r2, Rect& dst)
        {
            float xmin = std::max(r1.position.v[0], r2.position.v[0]);
            float xmax = std::min(r1.right(), r2.right());
            if (xmax > xmin)
            {
                float ymin = std::max(r1.position.v[1], r2.position.v[1]);
                float ymax = std::min(r1.bottom(), r2.bottom());
                if (ymax > ymin)
                {
                    dst.set(xmin, ymin, xmax - xmin, ymax - ymin);
                    return true;
                }
            }

            dst.set(0, 0, 0, 0);
            return false;
        }

        static void combine(const Rect& r1, const Rect& r2, Rect& dst)
        {
            dst.position.v[0] = std::min(r1.position.v[0], r2.position.v[0]);
            dst.position.v[1] = std::min(r1.position.v[1], r2.position.v[1]);
            dst.size.width = std::max(r1.position.v[0] + r1.size.width, r2.position.v[0] + r2.size.width) - dst.position.v[0];
            dst.size.height = std::max(r1.position.v[1] + r1.size.height, r2.position.v[1] + r2.size.height) - dst.position.v[1];
        }

        void inflate(float horizontalAmount, float verticalAmount)
        {
            position.v[0] -= horizontalAmount;
            position.v[1] -= verticalAmount;
            size.width += horizontalAmount * 2;
            size.height += verticalAmount * 2;
        }

        Rect& operator=(const Rect& r)
        {
            position.v[0] = r.position.v[0];
            position.v[1] = r.position.v[1];
            size.width = r.size.width;
            size.height = r.size.height;
            return *this;
        }

        bool operator==(const Rect& r) const
        {
            return position.v[0] == r.position.v[0] && size.width == r.size.width &&
                position.v[1] == r.position.v[1] && size.height == r.size.height;
        }

        bool operator!=(const Rect& r) const
        {
            return position.v[0] != r.position.v[0] || size.width != r.size.width ||
                position.v[1] != r.position.v[1] || size.height != r.size.height;
        }

        inline Rect operator*(float scalar) const
        {
            return Rect(position.v[0] * scalar, position.v[1] * scalar,
                        size.width * scalar, size.height * scalar);
        }

        inline Rect& operator*=(float scalar)
        {
            position.v[0] *= scalar;
            position.v[1] *= scalar;
            size.width *= scalar;
            size.height *= scalar;
            return *this;
        }

        inline Rect operator/(float scalar) const
        {
            return Rect(position.v[0] / scalar, position.v[1] / scalar,
                        size.width / scalar, size.height / scalar);
        }

        inline Rect& operator/=(float scalar)
        {
            position.v[0] /= scalar;
            position.v[1] /= scalar;
            size.width /= scalar;
            size.height /= scalar;
            return *this;
        }
    };
}
