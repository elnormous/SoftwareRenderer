//
//  SoftwareRenderer
//

#ifndef SAMPLER_H
#define SAMPLER_H

#include "Color.hpp"
#include "Vector.hpp"

namespace sr
{
    class Sampler final
    {
    public:
        enum class AddressMode
        {
            Clamp,
            Repeat,
            Mirror
        };

        enum class Filter
        {
            Point,
            Linear
        };

        AddressMode addressModeX = AddressMode::Clamp;
        AddressMode addressModeY = AddressMode::Clamp;
        Filter filter = Filter::Point;
    };
}

#endif
