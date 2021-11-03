//
//  SoftwareRenderer
//

#ifndef SR_BLENDSTATE_HPP
#define SR_BLENDSTATE_HPP

#include "Color.hpp"
#include "RenderError.hpp"

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

    inline float getValue(const BlendState::Factor factor,
                          const float srcColor,
                          const float srcAlpha,
                          const float destColor,
                          const float destAlpha,
                          const float blendFactor)
    {
        switch (factor)
        {
            case BlendState::Factor::zero: return 0.0F;
            case BlendState::Factor::one: return 1.0F;
            case BlendState::Factor::srcColor: return srcColor;
            case BlendState::Factor::invSrcColor: return 1.0F - srcColor;
            case BlendState::Factor::srcAlpha: return srcAlpha;
            case BlendState::Factor::invSrcAlpha: return 1.0F - srcAlpha;
            case BlendState::Factor::destAlpha: return destAlpha;
            case BlendState::Factor::invDestAlpha: return 1.0F - destAlpha;
            case BlendState::Factor::destColor: return destColor;
            case BlendState::Factor::invDestColor: return 1.0F - destColor;
            case BlendState::Factor::srcAlphaSat: return std::min(srcAlpha, 1.0F - destAlpha);
            case BlendState::Factor::blendFactor: return blendFactor;
            case BlendState::Factor::invBlendFactor: return 1.0F - blendFactor;
            default: throw RenderError{"Invalid blend factor"};
        }
    }

    inline float getValue(const BlendState::Operation operation,
                          const float a,
                          const float b)
    {
        switch (operation)
        {
            case BlendState::Operation::add: return std::clamp(a + b, 0.0F, 1.0F);
            case BlendState::Operation::subtract: return std::clamp(a - b, 0.0F, 1.0F);
            case BlendState::Operation::reverseSubtract: return std::clamp(b - a, 0.0F, 1.0F);
            case BlendState::Operation::min: return std::min(a, b);
            case BlendState::Operation::max: return std::max(a, b);
            default: throw RenderError{"Invalid blend operation"};
        }
    }
}

#endif
