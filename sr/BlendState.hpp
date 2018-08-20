//
//  SoftwareRenderer
//

#ifndef BLENDSTATE_H
#define BLENDSTATE_H

#include "Color.hpp"

namespace sr
{
    class BlendState
    {
    public:
        enum class Factor
        {
            ZERO,
            ONE,
            SRC_COLOR,
            INV_SRC_COLOR,
            SRC_ALPHA,
            INV_SRC_ALPHA,
            DEST_ALPHA,
            INV_DEST_ALPHA,
            DEST_COLOR,
            INV_DEST_COLOR,
            SRC_ALPHA_SAT,
            BLEND_FACTOR,
            INV_BLEND_FACTOR
        };

        enum class Operation
        {
            ADD,
            SUBTRACT,
            REV_SUBTRACT,
            MIN,
            MAX
        };

        BlendState::Factor colorBlendSource = BlendState::Factor::ONE;
        BlendState::Factor colorBlendDest = BlendState::Factor::ZERO;
        BlendState::Operation colorOperation = BlendState::Operation::ADD;
        BlendState::Factor alphaBlendSource = BlendState::Factor::ONE;
        BlendState::Factor alphaBlendDest = BlendState::Factor::ZERO;
        BlendState::Operation alphaOperation = BlendState::Operation::ADD;
        bool enabled = false;
        Color blendFactor;
    };
}

#endif
