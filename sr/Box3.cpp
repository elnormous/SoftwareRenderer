//
//  SoftwareRenderer
//

#include <algorithm>
#include "Box3.hpp"
#include "Box2.hpp"

namespace sr
{
    void Box3::merge(const Box3& box)
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
}
