//
//  SoftwareRenderer
//

#pragma once

#include <climits>
#include <vector>

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
            for (uint32_t i = 0; i < 256; ++i)
            {
                GAMMA_ENCODE[i] = static_cast<uint8_t>(roundf(powf(i / 255.0F, 1.0F / GAMMA) * 255.0F));
                GAMMA_DECODE[i] = roundf(powf(i / 255.0F, GAMMA) * 255.0F);
            }

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

        bool init(PixelFormat initPixelFormat, uint32_t initWidth, uint32_t initHeight, bool initMipMaps = false)
        {
            pixelFormat = initPixelFormat;
            width = initWidth;
            height = initHeight;
            mipMaps = initMipMaps;

            uint32_t pixelSize = getPixelSize(pixelFormat);
            if (pixelSize == 0) return false;

            levels.clear();
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

            return true;
        }

        bool resize(uint32_t newWidth, uint32_t newHeight)
        {
            width = newWidth;
            height = newHeight;

            uint32_t pixelSize = getPixelSize(pixelFormat);
            if (pixelSize == 0) return false;

            levels.clear();
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

            return true;
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

        inline bool setData(const std::vector<uint8_t>& buffer, uint32_t level = 0)
        {
            uint32_t pixelSize = getPixelSize(pixelFormat);
            if (pixelSize == 0) return false;

            if (buffer.size() != width * height * pixelSize) return false;

            if (level >= levels.size()) levels.resize(level + 1);
            levels[level] = buffer;

            return true;
        }

        bool generateMipMaps()
        {
            uint32_t pixelSize = getPixelSize(pixelFormat);
            if (pixelSize == 0) return false;

            if (levels.empty()) return false;

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
                        return false;
                }

                previousWidth = newWidth;
                previousHeight = newHeight;
                ++level;
            }

            return true;
        }

    private:
        void imageA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
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

        void imageR8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
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
                        r += GAMMA_DECODE[pixel[0]];
                        r += GAMMA_DECODE[pixel[1]];
                        r += GAMMA_DECODE[pixel[pitch + 0]];
                        r += GAMMA_DECODE[pixel[pitch + 1]];
                        r /= 4.0F;
                        dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
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
                    r += GAMMA_DECODE[pixel[0]];
                    r += GAMMA_DECODE[pixel[pitch + 0]];
                    r /= 2.0F;
                    dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                }
            }
            else if (dstWidth > 0)
            {
                const uint8_t* pixel = src;
                for (uint32_t x = 0; x < dstWidth; ++x, pixel += 2, dst += 1)
                {
                    float r = 0.0F;
                    r += GAMMA_DECODE[pixel[0]];
                    r += GAMMA_DECODE[pixel[1]];
                    r /= 2.0F;
                    dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                }
            }
        }

        void imageRGBA8Downsample2x2(uint32_t width, uint32_t height, uint32_t pitch, const uint8_t* src, uint8_t* dst)
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
                            r += GAMMA_DECODE[pixel[0]];
                            g += GAMMA_DECODE[pixel[1]];
                            b += GAMMA_DECODE[pixel[2]];
                            pixels += 1.0F;
                        }
                        a += pixel[3];

                        if (pixel[7] > 0)
                        {
                            r += GAMMA_DECODE[pixel[4]];
                            g += GAMMA_DECODE[pixel[5]];
                            b += GAMMA_DECODE[pixel[6]];
                            pixels += 1.0F;
                        }
                        a += pixel[7];

                        if (pixel[pitch + 3] > 0)
                        {
                            r += GAMMA_DECODE[pixel[pitch + 0]];
                            g += GAMMA_DECODE[pixel[pitch + 1]];
                            b += GAMMA_DECODE[pixel[pitch + 2]];
                            pixels += 1.0F;
                        }
                        a += pixel[pitch + 3];

                        if (pixel[pitch + 7] > 0)
                        {
                            r += GAMMA_DECODE[pixel[pitch + 4]];
                            g += GAMMA_DECODE[pixel[pitch + 5]];
                            b += GAMMA_DECODE[pixel[pitch + 6]];
                            pixels += 1.0F;
                        }
                        a += pixel[pitch + 7];

                        if (pixels > 0.0F)
                        {
                            r /= pixels;
                            g /= pixels;
                            b /= pixels;
                            a *= 0.25F;
                            dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                            dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(g))];
                            dst[2] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(b))];
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
                        r += GAMMA_DECODE[pixel[0]];
                        g += GAMMA_DECODE[pixel[1]];
                        b += GAMMA_DECODE[pixel[2]];
                        pixels += 1.0F;
                    }
                    a = pixel[3];

                    if (pixel[pitch + 3] > 0)
                    {
                        r += GAMMA_DECODE[pixel[pitch + 0]];
                        g += GAMMA_DECODE[pixel[pitch + 1]];
                        b += GAMMA_DECODE[pixel[pitch + 2]];
                        pixels += 1.0F;
                    }
                    a += pixel[pitch + 3];

                    if (pixels > 0.0F)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                        a *= 0.5F;
                        dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                        dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(g))];
                        dst[2] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(b))];
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
                        r += GAMMA_DECODE[pixel[0]];
                        g += GAMMA_DECODE[pixel[1]];
                        b += GAMMA_DECODE[pixel[2]];
                        pixels += 1.0F;
                    }
                    a += pixel[3];

                    if (pixel[7] > 0)
                    {
                        r += GAMMA_DECODE[pixel[4]];
                        g += GAMMA_DECODE[pixel[5]];
                        b += GAMMA_DECODE[pixel[6]];
                        pixels += 1.0F;
                    }
                    a += pixel[7];

                    if (pixels > 0.0F)
                    {
                        r /= pixels;
                        g /= pixels;
                        b /= pixels;
                        a *= 0.5F;
                        dst[0] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(r))];
                        dst[1] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(g))];
                        dst[2] = GAMMA_ENCODE[static_cast<uint8_t>(roundf(b))];
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
        static constexpr float GAMMA = 2.2F;
        uint8_t GAMMA_ENCODE[256];
        float GAMMA_DECODE[256];
        uint32_t width = 0;
        uint32_t height = 0;
        bool mipMaps = false;
        std::vector<std::vector<uint8_t>> levels;
        uint32_t minLOD = 0;
        uint32_t maxLOD = UINT_MAX;
        float lodBias = 0.0F;
    };
}
