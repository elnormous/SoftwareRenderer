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
            Zero,
            One,
            SrcColor,
            InvSrcColor,
            SrcAlpha,
            InvSrcAlpha,
            DestAlpha,
            InvDestAlpha,
            DestColor,
            InvDestColor,
            SrcAlphaSat,
            BlendFactor,
            InvBlendFactor
        };

        enum class Operation
        {
            Add,
            Subtract,
            RevSubtract,
            Min,
            Max
        };

        BlendState::Factor colorBlendSource = BlendState::Factor::One;
        BlendState::Factor colorBlendDest = BlendState::Factor::Zero;
        BlendState::Operation colorOperation = BlendState::Operation::Add;
        BlendState::Factor alphaBlendSource = BlendState::Factor::One;
        BlendState::Factor alphaBlendDest = BlendState::Factor::Zero;
        BlendState::Operation alphaOperation = BlendState::Operation::Add;
        bool enabled = false;
        Color blendFactor;
    };
}

#endif
