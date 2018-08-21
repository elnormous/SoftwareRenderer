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
    class Texture
    {
    public:
        enum class PixelFormat
        {
            NONE,
            R8,
            A8,
            RGBA8,
            FLOAT32
        };

        static constexpr float GAMMA = 2.2F;

        static uint32_t getPixelSize(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::R8:
                case PixelFormat::A8:
                    return sizeof(uint8_t) * 1;
                    break;
                case PixelFormat::RGBA8:
                    return sizeof(uint8_t) * 4;
                    break;
                case PixelFormat::FLOAT32:
                    return sizeof(float);
                    break;
                default:
                    return 0;
            }
        }

        Texture(PixelFormat initPixelFormat = PixelFormat::NONE,
                uint32_t initWidth = 0,
                uint32_t initHeight = 0,
                bool initMipMaps = false):
            pixelFormat(initPixelFormat),
            width(initWidth),
            height(initHeight),
            mipMaps(initMipMaps)
        {
            uint32_t pixelSize = getPixelSize(pixelFormat);

            if (pixelSize > 0 && width > 0 && height > 0)
            {
                levels.push_back(std::vector<uint8_t>(width * height * pixelSize));

                if (mipMaps)
                {
                    uint32_t newWidth = width;
                    uint32_t newHeight = height;

                    while (newWidth > 1 || newHeight > 1)
                    {
                        newWidth >>= 1;
                        newHeight >>= 1;

                        if (newWidth < 1) newWidth = 1;
                        if (newHeight < 1) newHeight = 1;

                        levels.push_back(std::vector<uint8_t>(newWidth * newHeight * pixelSize));
                    }
                }
            }
        }

        void resize(uint32_t newWidth, uint32_t newHeight)
        {
            width = newWidth;
            height = newHeight;

            uint32_t pixelSize = getPixelSize(pixelFormat);
            if (pixelSize == 0)
                throw std::runtime_error("Invalid pixel format");

            levels.clear();
            levels.push_back(std::vector<uint8_t>(width * height * pixelSize));

            if (mipMaps)
            {
                newWidth = width;
                newHeight = height;

                while (newWidth > 1 || newHeight > 1)
                {
                    newWidth >>= 1;
                    newHeight >>= 1;

                    if (newWidth < 1) newWidth = 1;
                    if (newHeight < 1) newHeight = 1;

                    levels.push_back(std::vector<uint8_t>(newWidth * newHeight * pixelSize));
                }
            }
        }

        inline PixelFormat getPixelFormat() const { return pixelFormat; }
        inline uint32_t getWidth() const { return width; }
        inline uint32_t getHeight() const { return height; }

        inline size_t getLevelCount() const
        {
            return levels.size();
        }

        inline std::vector<uint8_t>& getData(uint32_t level = 0)
        {
            return levels[level];
        }

        inline const std::vector<uint8_t>& getData(uint32_t level = 0) const
        {
            return levels[level];
        }

        inline void setData(const std::vector<uint8_t>& buffer, uint32_t level = 0)
        {
            uint32_t pixelSize = getPixelSize(pixelFormat);
            if (pixelSize == 0)
                throw std::runtime_error("Invalid pixel format");

            if (buffer.size() != width * height * pixelSize)
                throw std::runtime_error("Invalid buffer size");

            if (level >= levels.size()) levels.resize(level + 1);
            levels[level] = buffer;
        }

        inline Color getPixel(uint32_t x, uint32_t y, uint32_t level) const
        {
            Color result;

            const std::vector<uint8_t>& buffer = levels[level];

            switch (pixelFormat)
            {
                case Texture::PixelFormat::R8:
                {
                    const uint8_t* r = &buffer[(y * width + x) * 1];
                    result = sr::Color(*r, *r, *r, 255);
                    break;
                }
                case Texture::PixelFormat::A8:
                {
                    const uint8_t* a = &buffer[(y * width + x) * 1];
                    result = sr::Color(0, 0, 0, *a);
                    break;
                }
                case Texture::PixelFormat::RGBA8:
                {
                    const uint8_t* rgba = &buffer[(y * width + x) * 4];
                    result = sr::Color(rgba[0], rgba[1], rgba[2], rgba[3]);
                    break;
                }
                case Texture::PixelFormat::FLOAT32:
                {
                    float f = reinterpret_cast<const float*>(buffer.data())[y * width + x];
                    result.r = result.g = result.b = f;
                    result.a = 1.0F;
                    break;
                }
                default:
                    return result;
            }

            return result;
        }

        void generateMipMaps()
        {
            uint32_t pixelSize = getPixelSize(pixelFormat);
            if (pixelSize == 0)
                throw std::runtime_error("Invalid pixel format");

            if (levels.empty())
                throw std::runtime_error("Base image not provided");

            uint32_t newWidth = width;
            uint32_t newHeight = height;
            uint32_t previousWidth = width;
            uint32_t previousHeight = height;
            size_t level = 1;

            while (newWidth > 1 || newHeight > 1)
            {
                newWidth >>= 1;
                newHeight >>= 1;

                if (newWidth < 1) newWidth = 1;
                if (newHeight < 1) newHeight = 1;

                if (level >= levels.size())
                    levels.push_back(std::vector<uint8_t>(newWidth * newHeight * pixelSize));

                switch (pixelFormat)
                {
                    case PixelFormat::RGBA8:
                        imageRGBA8Downsample2x2(previousWidth, previousHeight, previousWidth * 4,
                                                levels[level - 1].data(),
                                                levels[level].data());
                        break;
                    case PixelFormat::R8:
                        imageR8Downsample2x2(previousWidth, previousHeight, previousWidth * 1,
                                             levels[level - 1].data(),
                                             levels[level].data());
                    break;
                    case PixelFormat::A8:
                        imageA8Downsample2x2(previousWidth, previousHeight, previousWidth * 1,
                                             levels[level - 1].data(),
                                             levels[level].data());
                        break;
                    default:
                        throw std::runtime_error("Invalid pixel format");
                }

                previousWidth = newWidth;
                previousHeight = newHeight;
                ++level;
            }
        }

        Color sample(const Sampler* sampler, const Vector2F& coord) const
        {
            Color result;

            if (sampler && !levels.empty())
            {
                float u, v;

                switch (sampler->getAddressModeX())
                {
                    case Sampler::AddressMode::CLAMP: u = clamp(coord.v[0], 0.0F, 1.0F) * (width - 1); break;
                    case Sampler::AddressMode::REPEAT: u = fmodf(coord.v[0], 1.0F) * (width - 1); break;
                    case Sampler::AddressMode::MIRROR: u = 1.0F - 2.0F * fabsf(fmodf(coord.v[0] / 2.0F, 1.0F) - 0.5F) * (width - 1); break;
                }

                switch (sampler->getAddressModeY())
                {
                    case Sampler::AddressMode::CLAMP: v = clamp(coord.v[1], 0.0F, 1.0F) * (height - 1); break;
                    case Sampler::AddressMode::REPEAT: v = fmodf(coord.v[1], 1.0F) * (height - 1); break;
                    case Sampler::AddressMode::MIRROR: v = 1.0F - 2.0F * fabsf(fmodf(coord.v[1] / 2.0F, 1.0F) - 0.5F) * (height - 1); break;
                }

                if (sampler->getFilter() == Sampler::Filter::POINT)
                {
                    uint32_t textureX = static_cast<uint32_t>(roundf(u));
                    uint32_t textureY = static_cast<uint32_t>(roundf(v));
                    result = getPixel(textureX, textureY, 0);
                }
                else
                {
                    uint32_t textureX0 = static_cast<uint32_t>(u - 0.5F);
                    uint32_t textureX1 = textureX0 + 1;
                    uint32_t textureY0 = static_cast<uint32_t>(v - 0.5F);
                    uint32_t textureY1 = textureY0 + 1;

                    textureX0 = clamp(textureX0, 0U, width - 1);
                    textureX1 = clamp(textureX1, 0U, width - 1);
                    textureY0 = clamp(textureY0, 0U, height - 1);
                    textureY1 = clamp(textureY1, 0U, height - 1);

                    // TODO: calculate mip level
                    Color color[4] = {
                        getPixel(textureX0, textureY0, 0),
                        getPixel(textureX1, textureY0, 0),
                        getPixel(textureX0, textureY1, 0),
                        getPixel(textureX1, textureY1, 0)
                    };

                    float x0 = u - (textureX0 + 0.5F);
                    float y0 = v - (textureY0 + 0.5F);
                    float x1 = (textureX0 + 1.5F) - u;
                    float y1 = (textureY0 + 1.5F) - v;

                    result.r = color[0].r * x1 * y1 + color[1].r * x0 * y1 + color[2].r * x1 * y0 + color[3].r * x0 * y0;
                    result.g = color[0].g * x1 * y1 + color[1].g * x0 * y1 + color[2].g * x1 * y0 + color[3].g * x0 * y0;
                    result.b = color[0].b * x1 * y1 + color[1].b * x0 * y1 + color[2].b * x1 * y0 + color[3].b * x0 * y0;
                    result.a = color[0].a * x1 * y1 + color[1].a * x0 * y1 + color[2].a * x1 * y0 + color[3].a * x0 * y0;
                }
            }

            return result;
        }

    private:
        static void imageA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width >> 1;
            const uint32_t dstHeight = height >> 1;

            if (dstWidth > 0 && dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                    {
                        float a = 0.0F;
                        a += pixel[0];
                        a += pixel[1];
                        a += pixel[pitch + 0];
                        a += pixel[pitch + 1];
                        a /= 4.0F;
                        dst[0] = static_cast<uint8_t>(a);
                    }
                }
            }
            else if (dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 1)
                {
                    const uint8_t* pixel = src;

                    float a = 0.0F;
                    a += pixel[0];
                    a += pixel[pitch + 0];
                    a /= 2.0F;
                    dst[0] = static_cast<uint8_t>(a);
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                {
                    float a = 0.0F;
                    a += pixel[0];
                    a += pixel[1];
                    a /= 2.0F;
                    dst[0] = static_cast<uint8_t>(a);
                }
            }
        }

        static void imageR8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width >> 1;
            const uint32_t dstHeight = height >> 1;

            if (dstWidth > 0 && dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                    {
                        float r = 0.0F;
                        r += powf(pixel[0] / 255.0F, GAMMA);
                        r += powf(pixel[1] / 255.0F, GAMMA);
                        r += powf(pixel[pitch + 0] / 255.0F, GAMMA);
                        r += powf(pixel[pitch + 1] / 255.0F, GAMMA);
                        r /= 4.0F;
                        dst[0] = static_cast<uint8_t>(roundf(powf(r, 1.0F / GAMMA) * 255.0F));
                    }
                }
            }
            else if (dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 1)
                {
                    const uint8_t* pixel = src;

                    float r = 0.0F;
                    r += powf(pixel[0] / 255.0F, GAMMA);
                    r += powf(pixel[pitch + 0] / 255.0F, GAMMA);
                    r /= 2.0F;
                    dst[0] = static_cast<uint8_t>(roundf(powf(r, 1.0F / GAMMA) * 255.0F));
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                {
                    float r = 0.0F;
                    r += powf(pixel[0] / 255.0F, GAMMA);
                    r += powf(pixel[1] / 255.0F, GAMMA);
                    r /= 2.0F;
                    dst[0] = static_cast<uint8_t>(roundf(powf(r, 1.0F / GAMMA) * 255.0F));
                }
            }
        }

        static void imageRGBA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
        {
            const uint32_t dstWidth  = width >> 1;
            const uint32_t dstHeight = height >> 1;

            if (dstWidth > 0 && dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;
                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep)
                {
                    const uint8_t* pixel = src;
                    for (uint32_t x = 0; x < dstWidth; ++x, pixel += 8, dst += 4)
                    {
                        float pixels = 0.0F;
                        float r = 0.0F;
                        float g = 0.0F;
                        float b = 0.0F;
                        float a = 0.0F;

                        if (pixel[3] > 0)
                        {
                            r += powf(pixel[0] / 255.0F, GAMMA);
                            g += powf(pixel[1] / 255.0F, GAMMA);
                            b += powf(pixel[2] / 255.0F, GAMMA);
                            pixels += 1.0F;
                        }
                        a += pixel[3];

                        if (pixel[7] > 0)
                        {
                            r += powf(pixel[4] / 255.0F, GAMMA);
                            g += powf(pixel[5] / 255.0F, GAMMA);
                            b += powf(pixel[6] / 255.0F, GAMMA);
                            pixels += 1.0F;
                        }
                        a += pixel[7];

                        if (pixel[pitch + 3] > 0)
                        {
                            r += powf(pixel[pitch + 0] / 255.0F, GAMMA);
                            g += powf(pixel[pitch + 1] / 255.0F, GAMMA);
                            b += powf(pixel[pitch + 2] / 255.0F, GAMMA);
                            pixels += 1.0F;
                        }
                        a += pixel[pitch + 3];

                        if (pixel[pitch + 7] > 0)
                        {
                            r += powf(pixel[pitch + 4] / 255.0F, GAMMA);
                            g += powf(pixel[pitch + 5] / 255.0F, GAMMA);
                            b += powf(pixel[pitch + 6] / 255.0F, GAMMA);
                            pixels += 1.0F;
                        }
                        a += pixel[pitch + 7];

                        if (pixels > 0.0F)
                        {
                            r /= pixels;
                            g /= pixels;
                            b /= pixels;
                            a *= 0.25F;
                            dst[0] = static_cast<uint8_t>(roundf(powf(r, 1.0F / GAMMA) * 255.0F));
                            dst[1] = static_cast<uint8_t>(roundf(powf(g, 1.0F / GAMMA) * 255.0F));
                            dst[2] = static_cast<uint8_t>(roundf(powf(b, 1.0F / GAMMA) * 255.0F));
                            dst[3] = static_cast<uint8_t>(a);
                        }
                        else
                        {
                            dst[0] = 0;
                            dst[1] = 0;
                            dst[2] = 0;
                            dst[3] = 0;
                        }
                    }
                }
            }
            else if (dstHeight > 0)
            {
                uint32_t y;
                uint32_t ystep;

                for (y = 0, ystep = pitch * 2; y < dstHeight; ++y, src += ystep, dst += 4)
                {
                    const uint8_t* pixel = src;

                    float pixels = 0.0F;
                    float r = 0.0F;
                    float g = 0.0F;
                    float b = 0.0F;
                    float a = 0.0F;

                    if (pixel[3] > 0)
                    {
                        r += powf(pixel[0] / 255.0F, GAMMA);
                        g += powf(pixel[1] / 255.0F, GAMMA);
                        b += powf(pixel[2] / 255.0F, GAMMA);
                        pixels += 1.0F;
                    }
                    a = pixel[3];

                    if (pixel[pitch + 3] > 0)
                    {
                        r += powf(pixel[pitch + 0] / 255.0F, GAMMA);
                        g += powf(pixel[pitch + 1] / 255.0F, GAMMA);
                        b += powf(pixel[pitch + 2] / 255.0F, GAMMA);
                        pixels += 1.0F;
                    }
                    a += pixel[pitch + 3];

                    if (pixels > 0.0F)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                        a *= 0.5F;
                        dst[0] = static_cast<uint8_t>(roundf(powf(r, 1.0F / GAMMA) * 255.0F));
                        dst[1] = static_cast<uint8_t>(roundf(powf(g, 1.0F / GAMMA) * 255.0F));
                        dst[2] = static_cast<uint8_t>(roundf(powf(b, 1.0F / GAMMA) * 255.0F));
                        dst[3] = static_cast<uint8_t>(a);
                    }
                    else
                    {
                        dst[0] = 0;
                        dst[1] = 0;
                        dst[2] = 0;
                        dst[3] = 0;
                    }
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 8, dst += 4)
                {
                    float pixels = 0.0F;
                    float r = 0.0F;
                    float g = 0.0F;
                    float b = 0.0F;
                    float a = 0.0F;

                    if (pixel[3] > 0)
                    {
                        r += powf(pixel[0] / 255.0F, GAMMA);
                        g += powf(pixel[1] / 255.0F, GAMMA);
                        b += powf(pixel[2] / 255.0F, GAMMA);
                        pixels += 1.0F;
                    }
                    a += pixel[3];

                    if (pixel[7] > 0)
                    {
                        r += powf(pixel[4] / 255.0F, GAMMA);
                        g += powf(pixel[5] / 255.0F, GAMMA);
                        b += powf(pixel[6] / 255.0F, GAMMA);
                        pixels += 1.0F;
                    }
                    a += pixel[7];

                    if (pixels > 0.0F)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                        a *= 0.5F;
                        dst[0] = static_cast<uint8_t>(roundf(powf(r, 1.0F / GAMMA) * 255.0F));
                        dst[1] = static_cast<uint8_t>(roundf(powf(g, 1.0F / GAMMA) * 255.0F));
                        dst[2] = static_cast<uint8_t>(roundf(powf(b, 1.0F / GAMMA) * 255.0F));
                        dst[3] = static_cast<uint8_t>(a);
                    }
                    else
                    {
                        dst[0] = 0;
                        dst[1] = 0;
                        dst[2] = 0;
                        dst[3] = 0;
                    }
                }
            }
        }

        PixelFormat pixelFormat = PixelFormat::NONE;
        uint32_t width = 0;
        uint32_t height = 0;
        bool mipMaps = false;
        std::vector<std::vector<uint8_t>> levels;
        uint32_t minLOD = 0;
        uint32_t maxLOD = UINT_MAX;
        float lodBias = 0.0F;
    };
}

#endif
