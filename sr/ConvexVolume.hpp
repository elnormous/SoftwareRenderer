//
//  SoftwareRenderer
//

#pragma once

#include <vector>
#include "Box3.hpp"
#include "Plane.hpp"
#include "Vector.hpp"

namespace sr
{
    class ConvexVolume
    {
    public:
        ConvexVolume()
        {
        }

        ConvexVolume(const std::vector<Plane>& initPlanes):
            planes(initPlanes)
        {
        }

        bool isPointInside(const Vector4F& position)
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(position) < 0.0F)
                {
                    return false;
                }
            }

            return true;
        }

        bool isSphereInside(const Vector4F& position, float radius)
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(position) < -radius)
                {
                    return false;
                }
            }

            return true;
        }

        bool isBoxInside(const Box3& box)
        {
            for (const Plane& plane : planes)
            {
                if (plane.dot(Vector4F(box.min.v[0], box.min.v[1], box.min.v[2], 1.0F)) >= 0.0F)
                {
                    continue;
                }

                if (plane.dot(Vector4F(box.max.v[0], box.min.v[1], box.min.v[2], 1.0F)) >= 0.0F)
                {
                    continue;
                }

                if (plane.dot(Vector4F(box.min.v[0], box.max.v[1], box.min.v[2], 1.0F)) >= 0.0F)
                {
                    continue;
                }

                if (plane.dot(Vector4F(box.min.v[0], box.min.v[1], box.max.v[2], 1.0F)) >= 0.0F)
                {
                    continue;
                }

                if (plane.dot(Vector4F(box.max.v[0], box.max.v[1], box.min.v[2], 1.0F)) >= 0.0F)
                {
                    continue;
                }

                if (plane.dot(Vector4F(box.max.v[0], box.min.v[1], box.max.v[2], 1.0F)) >= 0.0F)
                {
                    continue;
                }

                if (plane.dot(Vector4F(box.min.v[0], box.max.v[1], box.max.v[2], 1.0F)) >= 0.0F)
                {
                    continue;
                }

                if (plane.dot(Vector4F(box.max.v[0], box.max.v[1], box.max.v[2], 1.0F)) >= 0.0F)
                {
                    continue;
                }

                return false;
            }

            return true;
        }

        std::vector<Plane> planes;
    };
}
