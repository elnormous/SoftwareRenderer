//
//  SoftwareRenderer
//

#pragma once

#include "Color.hpp"
#include "Vector.hpp"

namespace sr
{
    class Sampler
    {
    public:
        enum AddressMode
        {
            CLAMP,
            REPEAT
        };

        enum class Filter
        {
            POINT,
            LINEAR
        };

        Sampler()
        {
        }

        inline AddressMode getAddressModeX() const
        {
            return addressModeX;
        }

        inline void setAddressModeX(AddressMode addressMode)
        {
            addressModeX = addressMode;
        }

        inline AddressMode getAddressModeY() const
        {
            return addressModeY;
        }

        inline void setAddressModeY(AddressMode addressMode)
        {
            addressModeY = addressMode;
        }

        inline Filter getFilter() const
        {
            return filter;
        }

        inline void setFilter(Filter newFilter)
        {
            filter = newFilter;
        }

    private:
        AddressMode addressModeX = AddressMode::CLAMP;
        AddressMode addressModeY = AddressMode::CLAMP;
        Filter filter = Filter::POINT;
    };
}
