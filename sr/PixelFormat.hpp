//
//  SoftwareRenderer
//

#ifndef PIXELFORMAT_HPP
#define PIXELFORMAT_HPP

namespace sr
{
    enum class PixelFormat
    {
        r8,
        a8,
        rgba8,
        float32
    };

    inline std::size_t getPixelSize(PixelFormat pixelFormat) noexcept
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
