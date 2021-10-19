//
//  SoftwareRenderer
//

#ifndef SR_COLOR_HPP
#define SR_COLOR_HPP

#include <cstddef>
#include <cstdint>
#include <array>
#include <type_traits>
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

        constexpr Color() noexcept
        {
        }

        explicit constexpr Color(std::uint32_t color) noexcept:
            r{static_cast<std::uint8_t>((color & 0xFF000000U) >> 24) / 255.0F},
            g{static_cast<std::uint8_t>((color & 0x00FF0000U) >> 16) / 255.0F},
            b{static_cast<std::uint8_t>((color & 0x0000FF00U) >> 8) / 255.0F},
            a{static_cast<std::uint8_t>(color & 0x000000FFU) / 255.0F}
        {
        }

        Color& operator=(std::uint32_t color) noexcept
        {
            r = static_cast<std::uint8_t>((color & 0xFF000000U) >> 24) / 255.0F;
            g = static_cast<std::uint8_t>((color & 0x00FF0000U) >> 16) / 255.0F;
            b = static_cast<std::uint8_t>((color & 0x0000FF00U) >> 8) / 255.0F;
            a = static_cast<std::uint8_t>(color & 0x000000FFU) / 255.0F;

            return *this;
        }

        template <class T, typename std::enable_if<std::is_integral<T>::value>::type* = nullptr>
        constexpr Color(T red, T green, T blue, T alpha = 0xFFU) noexcept:
            r{red / 255.0F},
            g{green / 255.0F},
            b{blue / 255.0F},
            a{alpha / 255.0F}
        {
        }

        constexpr Color(float red, float green, float blue, float alpha = 1.0F) noexcept:
            r{red},
            g{green},
            b{blue},
            a{alpha}
        {
        }

        explicit constexpr Color(const float color[4]) noexcept:
            r{color[0]},
            g{color[1]},
            b{color[2]},
            a{color[3]}
        {
        }

        constexpr std::uint32_t getIntValue() const noexcept
        {
            return (static_cast<std::uint32_t>(r * 255.0F) << 24) |
                   (static_cast<std::uint32_t>(g * 255.0F) << 16) |
                   (static_cast<std::uint32_t>(b * 255.0F) << 8) |
                   static_cast<std::uint32_t>(a * 255.0F);
        }

        std::uint32_t getIntValueRaw() const noexcept
        {
            const std::array<std::uint8_t, 4> result{
                static_cast<std::uint8_t>(r * 255.0F),
                static_cast<std::uint8_t>(g * 255.0F),
                static_cast<std::uint8_t>(b * 255.0F),
                static_cast<std::uint8_t>(a * 255.0F)
            };

            return *reinterpret_cast<const std::uint32_t*>(result.data());
        }
    };
}

#endif
