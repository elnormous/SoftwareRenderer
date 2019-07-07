//
//  SoftwareRenderer
//

#ifndef BOX2_H
#define BOX2_H

#include <cstdint>
#include <limits>
#include "Vector.hpp"
#include "Size2.hpp"

namespace sr
{
    class Box2 final
    {
    public:
        Vector2F min;
        Vector2F max;

        Box2():
            min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
            max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())
        {
        }

        Box2(const Vector2F& initMin, const Vector2F& initMax):
            min(initMin), max(initMax)
        {
        }

        Box2(const Box2& box):
            min(box.min), max(box.max)
        {
        }

        inline Vector2F getCenter()
        {
            return Vector2F(0.5F * (min.v[0] + max.v[0]),
                            0.5F * (min.v[1] + max.v[1]));
        }

        bool intersects(const Box2& aabb) const
        {
            return !(aabb.min.v[0] > max.v[0] ||
                     aabb.max.v[0] < min.v[0] ||
                     aabb.min.v[1] > max.v[1] ||
                     aabb.max.v[1] < min.v[1]);
        }

        bool containsPoint(const Vector2F& point) const
        {
            if (point.v[0] < min.v[0]) return false;
            if (point.v[1] < min.v[1]) return false;
            if (point.v[0] > max.v[0]) return false;
            if (point.v[1] > max.v[1]) return false;
            return true;
        }

        void merge(const Box2& box)
        {
            // Calculate the new minimum point
            min.v[0] = std::min(min.v[0], box.min.v[0]);
            min.v[1] = std::min(min.v[1], box.min.v[1]);

            // Calculate the new maximum point
            max.v[0] = std::max(max.v[0], box.max.v[0]);
            max.v[1] = std::max(max.v[1], box.max.v[1]);
        }

        void set(const Vector2F& newMin, const Vector2F& newMax)
        {
            min = newMin;
            max = newMax;
        }

        void reset()
        {
            min.v[0] = std::numeric_limits<float>::max();
            min.v[1] = std::numeric_limits<float>::max();

            max.v[0] = std::numeric_limits<float>::lowest();
            max.v[1] = std::numeric_limits<float>::lowest();
        }

        bool isEmpty() const
        {
            return min.v[0] > max.v[0] || min.v[1] > max.v[1];
        }

        void insertPoint(const Vector2F& point)
        {
            if (point.v[0] < min.v[0]) min.v[0] = point.v[0];
            if (point.v[0] > max.v[0]) max.v[0] = point.v[0];
            if (point.v[1] < min.v[1]) min.v[1] = point.v[1];
            if (point.v[1] > max.v[1]) max.v[1] = point.v[1];
        }

        inline Box2 operator+(const Vector2F& v) const
        {
            Box2 result(*this);
            result += v;
            return result;
        }

        inline Box2& operator+=(const Vector2F& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline Box2 operator-(const Vector2F& v) const
        {
            Box2 result(*this);
            result -= v;
            return result;
        }

        inline Box2& operator-=(const Vector2F& v)
        {
            min -= v;
            max -= v;
            return *this;
        }

        inline Size2 getSize() const
        {
            return Size2(max.v[0] - min.v[0], max.v[1] - min.v[1]);
        }
    };
}

#endif
