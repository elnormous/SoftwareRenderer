//
//  SoftwareRenderer
//

#ifndef COLOR_H
#define COLOR_H

#include <cstddef>
#include <cstdint>
#include "Vector.hpp"

namespace sr
{
    class Color final
    {
    public:
        float r = 0.0F;
        float g = 0.0F;
        float b = 0.0F;
        float a = 0.0F;

        Color() noexcept
        {
        }

        explicit Color(std::uint32_t color) noexcept:
            r(static_cast<std::uint8_t>((color & 0xFF000000) >> 24) / 255.0F),
            g(static_cast<std::uint8_t>((color & 0x00FF0000) >> 16) / 255.0F),
            b(static_cast<std::uint8_t>((color & 0x0000FF00) >> 8) / 255.0F),
            a(static_cast<std::uint8_t>(color & 0x000000FF) / 255.0F)
        {
        }

        Color& operator=(std::uint32_t color) noexcept
        {
            r = static_cast<std::uint8_t>((color & 0xFF000000) >> 24) / 255.0F;
            g = static_cast<std::uint8_t>((color & 0x00FF0000) >> 16) / 255.0F;
            b = static_cast<std::uint8_t>((color & 0x0000FF00) >> 8) / 255.0F;
            a = static_cast<std::uint8_t>(color & 0x000000FF) / 255.0F;

            return *this;
        }

        Color(std::uint8_t red, std::uint8_t green, std::uint8_t blue, std::uint8_t alpha = 0xFF) noexcept:
            r(red / 255.0F), g(green / 255.0F), b(blue / 255.0F), a(alpha / 255.0F)
        {
        }

        explicit Color(const float color[4]) noexcept:
            r(color[0]),
            g(color[1]),
            b(color[2]),
            a(color[3])
        {
        }

        std::uint32_t getIntValue() const noexcept
        {
            return (static_cast<std::uint32_t>(r * 255.0F) << 24) |
                   (static_cast<std::uint32_t>(g * 255.0F) << 16) |
                   (static_cast<std::uint32_t>(b * 255.0F) << 8) |
                   static_cast<std::uint32_t>(a * 255.0F);
        }

        std::uint32_t getIntValueRaw() const noexcept
        {
            std::uint32_t result;
            std::uint8_t* bytes = reinterpret_cast<std::uint8_t*>(&result);
            bytes[0] = static_cast<std::uint8_t>(r * 255.0F);
            bytes[1] = static_cast<std::uint8_t>(g * 255.0F);
            bytes[2] = static_cast<std::uint8_t>(b * 255.0F);
            bytes[3] = static_cast<std::uint8_t>(a * 255.0F);
            return result;
        }
    };
} // namespace sr

#endif
