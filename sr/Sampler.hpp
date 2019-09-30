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
            CLAMP,
            REPEAT,
            MIRROR
        };

        enum class Filter
        {
            POINT,
            LINEAR
        };

        AddressMode addressModeX = AddressMode::CLAMP;
        AddressMode addressModeY = AddressMode::CLAMP;
        Filter filter = Filter::POINT;
    };
}

#endif
