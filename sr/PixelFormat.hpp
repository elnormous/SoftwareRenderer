//
//  SoftwareRenderer
//

#ifndef SR_PIXELFORMAT_HPP
#define SR_PIXELFORMAT_HPP

#include <cstdint>

namespace sr
{
    enum class PixelFormat
    {
        r8,
        a8,
        rgba8,
        float32
    };

    inline std::size_t getPixelSize(const PixelFormat pixelFormat) noexcept
    {
        switch (pixelFormat)
        {
        case PixelFormat::r8:
        case PixelFormat::a8:
            return sizeof(std::uint8_t) * 1;
        case PixelFormat::rgba8:
            return sizeof(std::uint8_t) * 4;
        case PixelFormat::float32:
            return sizeof(float);
        default:
            return 0;
        }
    }
}

#endif
