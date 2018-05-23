//
//  SoftwareRenderer
//

#include <cstdint>
#include <limits>
#include "Vector.hpp"
#include "Size3.hpp"

#pragma once

namespace sr
{
    class Box2;

    class Box3
    {
    public:
        Vector3F min;
        Vector3F max;

        Box3():
            min(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
            max(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())
        {
        }

        Box3(const Vector3F& initMin, const Vector3F& initMax):
            min(initMin), max(initMax)
        {
        }

        Box3(const Box3& box):
            min(box.min), max(box.max)
        {
        }

        inline Vector3F getCenter()
        {
            return Vector3F(0.5F * (min.v[0] + max.v[0]),
                           0.5F * (min.v[1] + max.v[1]),
                           0.5F * (min.v[2] + max.v[2]));
        }

        bool intersects(const Box3& aabb) const
        {
            return !(aabb.min.v[0] > max.v[0] ||
                     aabb.max.v[0] < min.v[0] ||
                     aabb.min.v[1] > max.v[1] ||
                     aabb.max.v[1] < min.v[1] ||
                     aabb.min.v[2] > max.v[2] ||
                     aabb.max.v[2] < min.v[2]);
        }

        bool containsPoint(const Vector3F& point) const
        {
            if (point.v[0] < min.v[0]) return false;
            if (point.v[1] < min.v[1]) return false;
            if (point.v[2] < min.v[2]) return false;
            if (point.v[0] > max.v[0]) return false;
            if (point.v[1] > max.v[1]) return false;
            if (point.v[2] > max.v[2]) return false;
            return true;
        }

        void merge(const Box3& box)
        {
            // Calculate the new minimum point
            min.v[0] = std::min(min.v[0], box.min.v[0]);
            min.v[1] = std::min(min.v[1], box.min.v[1]);
            min.v[2] = std::min(min.v[2], box.min.v[2]);

            // Calculate the new maximum point
            max.v[0] = std::max(max.v[0], box.max.v[0]);
            max.v[1] = std::max(max.v[1], box.max.v[1]);
            max.v[2] = std::max(max.v[1], box.max.v[2]);
        }

        void set(const Vector3F& newMin, const Vector3F& newMax)
        {
            min = newMin;
            max = newMax;
        }

        void reset()
        {
            min.v[0] = std::numeric_limits<float>::max();
            min.v[1] = std::numeric_limits<float>::max();
            min.v[2] = std::numeric_limits<float>::max();

            max.v[0] = std::numeric_limits<float>::lowest();
            max.v[1] = std::numeric_limits<float>::lowest();
            max.v[2] = std::numeric_limits<float>::lowest();
        }

        bool isEmpty() const
        {
            return min.v[0] > max.v[0] || min.v[1] > max.v[1] || min.v[2] > max.v[2];
        }

        void insertPoint(const Vector3F& point)
        {
            if (point.v[0] < min.v[0]) min.v[0] = point.v[0];
            if (point.v[0] > max.v[0]) max.v[0] = point.v[0];
            if (point.v[1] < min.v[1]) min.v[1] = point.v[1];
            if (point.v[1] > max.v[1]) max.v[1] = point.v[1];
            if (point.v[2] < min.v[2]) min.v[2] = point.v[2];
            if (point.v[2] > max.v[2]) max.v[2] = point.v[2];
        }

        inline Box3 operator+(const Vector3F& v) const
        {
            Box3 result(*this);
            result += v;
            return result;
        }

        inline Box3& operator+=(const Vector3F& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline Box3 operator-(const Vector3F& v) const
        {
            Box3 result(*this);
            result -= v;
            return result;
        }

        inline Box3& operator-=(const Vector3F& v)
        {
            min -= v;
            max -= v;
            return *this;
        }

        inline Size3 getSize() const
        {
            return Size3(max.v[0] - min.v[0], max.v[1] - min.v[1], max.v[2] - min.v[2]);
        }
    };
}
