//
//  SoftwareRenderer
//

#ifndef TEXTURE_H
#define TEXTURE_H

#include <climits>
#include <stdexcept>
#include <vector>
#include "Sampler.hpp"

namespace sr
{
    class Texture final
    {
    public:
        enum class PixelFormat
        {
            r8,
            a8,
            rgba8,
            float32
        };

        static std::size_t getPixelSize(PixelFormat pixelFormat) noexcept
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

        Texture(PixelFormat initPixelFormat = PixelFormat::rgba8,
                std::size_t initWidth = 0,
                std::size_t initHeight = 0,
                bool initMipMaps = false):
            pixelFormat{initPixelFormat},
            width{initWidth},
            height{initHeight},
            mipMaps{initMipMaps}
        {
            const auto pixelSize = getPixelSize(pixelFormat);

            if (pixelSize <= 0 || width <= 0 || height <= 0)
                return;

            levels.push_back(std::vector<std::uint8_t>(width * height * pixelSize));

            if (!mipMaps)
                return;
            
            std::size_t newWidth = width;
            std::size_t newHeight = height;

            while (newWidth > 1 || newHeight > 1)
            {
                newWidth >>= 1;
                newHeight >>= 1;

                if (newWidth < 1) newWidth = 1;
                if (newHeight < 1) newHeight = 1;

                levels.push_back(std::vector<std::uint8_t>(newWidth * newHeight * pixelSize));
            }
        }

        void resize(std::size_t newWidth, std::size_t newHeight)
        {
            width = newWidth;
            height = newHeight;

            const auto pixelSize = getPixelSize(pixelFormat);
            if (pixelSize == 0)
                throw std::runtime_error("Invalid pixel format");

            levels.clear();
            levels.push_back(std::vector<std::uint8_t>(width * height * pixelSize));

            if (mipMaps)
                return;

            newWidth = width;
            newHeight = height;

            while (newWidth > 1 || newHeight > 1)
            {
                newWidth >>= 1;
                newHeight >>= 1;

                if (newWidth < 1) newWidth = 1;
                if (newHeight < 1) newHeight = 1;

                levels.push_back(std::vector<std::uint8_t>(newWidth * newHeight * pixelSize));
            }
        }

        PixelFormat getPixelFormat() const noexcept { return pixelFormat; }
        std::size_t getWidth() const noexcept { return width; }
        std::size_t getHeight() const noexcept { return height; }

        std::size_t getLevelCount() const noexcept
        {
            return levels.size();
        }

        std::vector<std::uint8_t>& getData(std::uint32_t level = 0)
        {
            return levels[level];
        }

        const std::vector<std::uint8_t>& getData(std::uint32_t level = 0) const
        {
            return levels[level];
        }

        void setData(const std::vector<std::uint8_t>& buffer, std::uint32_t level = 0)
        {
            const auto pixelSize = getPixelSize(pixelFormat);
            if (pixelSize == 0)
                throw std::runtime_error("Invalid pixel format");

            if (buffer.size() != width * height * pixelSize)
                throw std::runtime_error("Invalid buffer size");

            if (level >= levels.size()) levels.resize(level + 1);
            levels[level] = buffer;
        }

        Color getPixel(std::size_t x, std::size_t y, std::uint32_t level) const
        {
            const auto& buffer = levels[level];

            switch (pixelFormat)
            {
                case Texture::PixelFormat::r8:
                {
                    const auto* r = &buffer[(y * width + x) * 1];
                    return Color{*r, *r, *r, std::uint8_t(255U)};
                }
                case Texture::PixelFormat::a8:
                {
                    const auto* a = &buffer[(y * width + x) * 1];
                    return Color{std::uint8_t(0U), std::uint8_t(0U), std::uint8_t(0U), *a};
                }
                case Texture::PixelFormat::rgba8:
                {
                    const auto* rgba = &buffer[(y * width + x) * 4];
                    return Color{rgba[0], rgba[1], rgba[2], rgba[3]};
                }
                case Texture::PixelFormat::float32:
                {
                    const float f = reinterpret_cast<const float*>(buffer.data())[y * width + x];
                    return Color{f, f, f, 1.0F};
                }
                default:
                    throw std::runtime_error("Invalid pixel format");
            }
        }

        Color sample(const Sampler* sampler, const Vector<float, 2>& coord) const
        {
            if (sampler && !levels.empty())
            {
                const auto u =
                    (sampler->addressModeX == Sampler::AddressMode::clamp) ? clamp(coord.v[0], 0.0F, 1.0F) * (width - 1) :
                    (sampler->addressModeX == Sampler::AddressMode::repeat) ? std::fmod(coord.v[0], 1.0F) * (width - 1) :
                    (sampler->addressModeX == Sampler::AddressMode::mirror) ? 1.0F - 2.0F * std::fabs(std::fmod(coord.v[0] / 2.0F, 1.0F) - 0.5F) * (width - 1) :
                    0.0F;

                const auto v =
                    (sampler->addressModeY == Sampler::AddressMode::clamp) ? clamp(coord.v[1], 0.0F, 1.0F) * (height - 1) :
                    (sampler->addressModeY == Sampler::AddressMode::repeat) ? std::fmod(coord.v[1], 1.0F) * (height - 1) :
                    (sampler->addressModeY == Sampler::AddressMode::mirror) ? 1.0F - 2.0F * std::fabs(std::fmod(coord.v[1] / 2.0F, 1.0F) - 0.5F) * (height - 1) :
                    0.0F;

                if (sampler->filter == Sampler::Filter::point)
                {
                    const auto textureX = static_cast<std::size_t>(std::round(u));
                    const auto textureY = static_cast<std::size_t>(std::round(v));
                    return getPixel(textureX, textureY, 0);
                }
                
                auto textureX0 = static_cast<std::size_t>(u - 0.5F);
                auto textureX1 = textureX0 + 1;
                auto textureY0 = static_cast<std::size_t>(v - 0.5F);
                auto textureY1 = textureY0 + 1;

                textureX0 = clamp(textureX0, static_cast<std::size_t>(0U), width - 1);
                textureX1 = clamp(textureX1, static_cast<std::size_t>(0U), width - 1);
                textureY0 = clamp(textureY0, static_cast<std::size_t>(0U), height - 1);
                textureY1 = clamp(textureY1, static_cast<std::size_t>(0U), height - 1);

                // TODO: calculate mip level
                const Color color[4] = {
                    getPixel(textureX0, textureY0, 0),
                    getPixel(textureX1, textureY0, 0),
                    getPixel(textureX0, textureY1, 0),
                    getPixel(textureX1, textureY1, 0)
                };

                const auto x0 = u - (textureX0 + 0.5F);
                const auto y0 = v - (textureY0 + 0.5F);
                const auto x1 = (textureX0 + 1.5F) - u;
                const auto y1 = (textureY0 + 1.5F) - v;
                
                return Color{
                    color[0].r * x1 * y1 + color[1].r * x0 * y1 + color[2].r * x1 * y0 + color[3].r * x0 * y0,
                    color[0].g * x1 * y1 + color[1].g * x0 * y1 + color[2].g * x1 * y0 + color[3].g * x0 * y0,
                    color[0].b * x1 * y1 + color[1].b * x0 * y1 + color[2].b * x1 * y0 + color[3].b * x0 * y0,
                    color[0].a * x1 * y1 + color[1].a * x0 * y1 + color[2].a * x1 * y0 + color[3].a * x0 * y0
                };
            }

            return Color();
        }

    private:
        PixelFormat pixelFormat;
        std::size_t width = 0;
        std::size_t height = 0;
        bool mipMaps = false;
        std::vector<std::vector<std::uint8_t>> levels;
        std::uint32_t minLOD = 0;
        std::uint32_t maxLOD = UINT_MAX;
        float lodBias = 0.0F;
    };
}

#endif
