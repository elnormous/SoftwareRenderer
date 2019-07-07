//
//  SoftwareRenderer
//

#ifndef CONVEXVOLUME_H
#define CONVEXVOLUME_H

#include <vector>
#include "Box3.hpp"
#include "Plane.hpp"
#include "Vector.hpp"

namespace sr
{
    class ConvexVolume final
    {
    public:
        ConvexVolume()
        {
        }

        ConvexVolume(const std::vector<Plane>& initPlanes):
            planes(initPlanes)
        {
        }

        bool isPointInside(const Vector4F& position) const
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

        bool isSphereInside(const Vector4F& position, float radius) const
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

        bool isBoxInside(const Box3& box) const
        {
            Vector4F leftBottomBack(box.min.v[0], box.min.v[1], box.min.v[2], 1.0F);
            Vector4F leftBottomFront(box.min.v[0], box.min.v[1], box.max.v[2], 1.0F);
            Vector4F leftTopBack(box.min.v[0], box.max.v[1], box.min.v[2], 1.0F);
            Vector4F leftTopFront(box.min.v[0], box.max.v[1], box.max.v[2], 1.0F);
            Vector4F rightBottomBack(box.max.v[0], box.min.v[1], box.min.v[2], 1.0F);
            Vector4F rightBottomFront(box.max.v[0], box.min.v[1], box.max.v[2], 1.0F);
            Vector4F rightTopBack(box.max.v[0], box.max.v[1], box.min.v[2], 1.0F);
            Vector4F rightTopFront(box.max.v[0], box.max.v[1], box.max.v[2], 1.0F);


            for (const Plane& plane : planes)
            {
                if (plane.dot(leftBottomBack) < 0.0F &&
                    plane.dot(leftBottomFront) < 0.0F &&
                    plane.dot(leftTopBack) < 0.0F &&
                    plane.dot(leftTopFront) < 0.0F &&
                    plane.dot(rightBottomBack) < 0.0F &&
                    plane.dot(rightBottomFront) < 0.0F &&
                    plane.dot(rightTopBack) < 0.0F &&
                    plane.dot(rightTopFront) < 0.0F)
                    return false;
            }

            return true;
        }

        std::vector<Plane> planes;
    };
}

#endif
