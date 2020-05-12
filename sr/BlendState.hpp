//
//  SoftwareRenderer
//

#ifndef BLENDSTATE_H
#define BLENDSTATE_H

#include "Color.hpp"

namespace sr
{
    class BlendState final
    {
    public:
        enum class Factor
        {
            zero,
            one,
            srcColor,
            invSrcColor,
            srcAlpha,
            invSrcAlpha,
            destAlpha,
            invDestAlpha,
            destColor,
            invDestColor,
            srcAlphaSat,
            blendFactor,
            invBlendFactor
        };

        enum class Operation
        {
            add,
            subtract,
            reverseSubtract,
            min,
            max
        };

        BlendState::Factor colorBlendSource = BlendState::Factor::one;
        BlendState::Factor colorBlendDest = BlendState::Factor::zero;
        BlendState::Operation colorOperation = BlendState::Operation::add;
        BlendState::Factor alphaBlendSource = BlendState::Factor::one;
        BlendState::Factor alphaBlendDest = BlendState::Factor::zero;
        BlendState::Operation alphaOperation = BlendState::Operation::add;
        bool enabled = false;
        Color blendFactor;
    };
}

#endif
