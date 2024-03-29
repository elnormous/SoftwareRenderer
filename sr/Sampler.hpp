//
//  SoftwareRenderer
//

#ifndef SR_SAMPLER_HPP
#define SR_SAMPLER_HPP

#include "Color.hpp"
#include "Vector.hpp"

namespace sr
{
    class Sampler final
    {
    public:
        enum class AddressMode
        {
            clamp,
            repeat,
            mirror
        };

        enum class Filter
        {
            point,
            linear
        };

        AddressMode addressModeX = AddressMode::clamp;
        AddressMode addressModeY = AddressMode::clamp;
        Filter filter = Filter::point;
    };
}

#endif
