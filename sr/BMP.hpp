//
//  SoftwareRenderer
//

#ifndef BMP_H
#define BMP_H

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>
#include "File.hpp"

namespace sr
{
    class BMP final
    {
    public:
        static constexpr int BITMAPFILEHEADER_TYPE_BM = 0x4D42;

        struct BitmapFileHeader              /**** BMP file header structure ****/
        {
            unsigned short bfType;           /* Magic number for file */
            unsigned int   bfSize;           /* Size of file */
            unsigned short bfReserved1;      /* Reserved */
            unsigned short bfReserved2;      /* ... */
            unsigned int   bfOffBits;        /* Offset to bitmap data */
        };

        struct BitmapInfoHeader              /**** BMP file info structure ****/
        {
            unsigned int   biSize;           /* Size of info header */
            int            biWidth;          /* Width of image */
            int            biHeight;         /* Height of image */
            unsigned short biPlanes;         /* Number of color planes */
            unsigned short biBitCount;       /* Number of bits per pixel */
            unsigned int   biCompression;    /* Type of compression to use */
            unsigned int   biSizeImage;      /* Size of image data */
            int            biXPelsPerMeter;  /* X pixels per meter */
            int            biYPelsPerMeter;  /* Y pixels per meter */
            unsigned int   biClrUsed;        /* Number of colors used */
            unsigned int   biClrImportant;   /* Number of important colors */
        };

        struct RGBQuad                      /**** Colormap entry structure ****/
        {
            unsigned char rgbBlue;          /* Blue value */
            unsigned char rgbGreen;         /* Green value */
            unsigned char rgbRed;           /* Red value */
            unsigned char rgbReserved;      /* Reserved */
        };

        enum Compression
        {
            RGB = 0x0000,
            RLE8 = 0x0001,
            RLE4 = 0x0002,
            BITFIELDS = 0x0003,
            JPEG = 0x0004,
            PNG = 0x0005,
            CMYK = 0x000B,
            CMYKRLE8 = 0x000C,
            CMYKRLE4 = 0x000D
        };

        BMP() = default;

        BMP(const std::string& filename)
        {
            File f(filename, File::Mode::Read);

            BitmapFileHeader header;

            f.read(&header.bfType, sizeof(header.bfType), true);

            if (header.bfType != BITMAPFILEHEADER_TYPE_BM)
                throw std::runtime_error("Bad bitmap file");

            f.read(&header.bfSize, sizeof(header.bfSize), true);
            f.read(&header.bfReserved1, sizeof(header.bfReserved1), true);
            f.read(&header.bfReserved2, sizeof(header.bfReserved2), true);
            f.read(&header.bfOffBits, sizeof(header.bfOffBits), true);

            BitmapInfoHeader infoHeader;
            f.read(&infoHeader.biSize, sizeof(infoHeader.biSize), true);

            uint32_t offset = 0;

            if (offset + sizeof(infoHeader.biWidth) < infoHeader.biSize)
            {
                f.read(&infoHeader.biWidth, sizeof(infoHeader.biWidth), true);
                offset += sizeof(infoHeader.biWidth);
            }

            if (offset + sizeof(infoHeader.biHeight) < infoHeader.biSize)
            {
                f.read(&infoHeader.biHeight, sizeof(infoHeader.biHeight), true);
                offset += sizeof(infoHeader.biHeight);
            }

            if (offset + sizeof(infoHeader.biPlanes) < infoHeader.biSize)
            {
                f.read(&infoHeader.biPlanes, sizeof(infoHeader.biPlanes), true);
                offset += sizeof(infoHeader.biPlanes);
            }

            if (offset + sizeof(infoHeader.biBitCount) < infoHeader.biSize)
            {
                f.read(&infoHeader.biBitCount, sizeof(infoHeader.biBitCount), true);
                offset += sizeof(infoHeader.biBitCount);
            }

            if (offset + sizeof(infoHeader.biCompression) < infoHeader.biSize)
            {
                f.read(&infoHeader.biCompression, sizeof(infoHeader.biCompression), true);
                offset += sizeof(infoHeader.biCompression);
            }

            if (infoHeader.biCompression != RGB)
                throw std::runtime_error("Compression not supported");

            if (offset + sizeof(infoHeader.biSizeImage) < infoHeader.biSize)
            {
                f.read(&infoHeader.biSizeImage, sizeof(infoHeader.biSizeImage), true);
                offset += sizeof(infoHeader.biSizeImage);
            }

            if (offset + sizeof(infoHeader.biXPelsPerMeter) < infoHeader.biSize)
            {
                f.read(&infoHeader.biXPelsPerMeter, sizeof(infoHeader.biXPelsPerMeter), true);
                offset += sizeof(infoHeader.biXPelsPerMeter);
            }

            if (offset + sizeof(infoHeader.biYPelsPerMeter) < infoHeader.biSize)
            {
                f.read(&infoHeader.biYPelsPerMeter, sizeof(infoHeader.biYPelsPerMeter), true);
                offset += sizeof(infoHeader.biYPelsPerMeter);
            }

            if (offset + sizeof(infoHeader.biClrUsed) < infoHeader.biSize)
            {
                f.read(&infoHeader.biClrUsed, sizeof(infoHeader.biClrUsed), true);
                offset += sizeof(infoHeader.biClrUsed);
            }

            if (offset + sizeof(infoHeader.biClrImportant) < infoHeader.biSize)
            {
                f.read(&infoHeader.biClrImportant, sizeof(infoHeader.biClrImportant), true);
                offset += sizeof(infoHeader.biClrImportant);
            }

            f.seek(header.bfOffBits, File::Seek::Begin);

            data.resize(infoHeader.biWidth * std::abs(infoHeader.biHeight) * sizeof(RGBQuad));
            std::fill(data.begin(), data.end(), 255);

            if (infoHeader.biHeight > 0) // bottom to top
            {
                for (int y = infoHeader.biHeight - 1; y >= 0; --y)
                    for (int x = 0; x < infoHeader.biWidth; ++x)
                        f.read(&data[(y * infoHeader.biWidth + x) * sizeof(RGBQuad)], infoHeader.biBitCount / 8, true);
            }
            else // top to bottom
            {
                for (int y = 0; y < std::abs(infoHeader.biHeight); ++y)
                    for (int x = 0; x < infoHeader.biWidth; ++x)
                        f.read(&data[(y * infoHeader.biWidth + x) * sizeof(RGBQuad)], infoHeader.biBitCount / 8, true);
            }

            width = static_cast<uint32_t>(infoHeader.biWidth);
            height = static_cast<uint32_t>(std::abs(infoHeader.biHeight));
        }

        inline uint32_t getWidth() const { return width; }
        inline uint32_t getHeight() const { return height; }

        const std::vector<uint8_t>& getData() const
        {
            return data;
        }

        void setData(uint32_t newWidth,
                     uint32_t newHeight,
                     const std::vector<uint8_t>& newData)
        {
            width = newWidth;
            height = newHeight;
            data = newData;
        }

        void save(const std::string& filename)
        {
            File f(filename, File::Mode::Write);

            BitmapFileHeader header;
            header.bfType = BITMAPFILEHEADER_TYPE_BM;
            header.bfSize = width  * height * 4 + 14 + 40;
            header.bfReserved1 = 0;
            header.bfReserved2 = 0;
            header.bfOffBits = 14 + 40; // size of BITMAPFILEHEADER + size of BITMAPINFOHEADER

            f.write(&header.bfType, sizeof(header.bfType), true);
            f.write(&header.bfSize, sizeof(header.bfSize), true);
            f.write(&header.bfReserved1, sizeof(header.bfReserved1), true);
            f.write(&header.bfReserved2, sizeof(header.bfReserved2), true);
            f.write(&header.bfOffBits, sizeof(header.bfOffBits), true);

            BitmapInfoHeader infoHeader;
            infoHeader.biSize = 40;
            infoHeader.biWidth = static_cast<int>(width);
            infoHeader.biHeight = -static_cast<int>(height);
            infoHeader.biPlanes = 1;
            infoHeader.biBitCount = 32;
            infoHeader.biCompression = 0;
            infoHeader.biSizeImage = 0;
            infoHeader.biXPelsPerMeter = 0;
            infoHeader.biYPelsPerMeter = 0;
            infoHeader.biClrUsed = 0;
            infoHeader.biClrImportant = 0;

            f.write(&infoHeader.biSize, sizeof(infoHeader.biSize), true);
            f.write(&infoHeader.biWidth, sizeof(infoHeader.biWidth), true);
            f.write(&infoHeader.biHeight, sizeof(infoHeader.biHeight), true);
            f.write(&infoHeader.biPlanes, sizeof(infoHeader.biPlanes), true);
            f.write(&infoHeader.biBitCount, sizeof(infoHeader.biBitCount), true);
            f.write(&infoHeader.biCompression, sizeof(infoHeader.biCompression), true);
            f.write(&infoHeader.biSizeImage, sizeof(infoHeader.biSizeImage), true);
            f.write(&infoHeader.biXPelsPerMeter, sizeof(infoHeader.biXPelsPerMeter), true);
            f.write(&infoHeader.biYPelsPerMeter, sizeof(infoHeader.biYPelsPerMeter), true);
            f.write(&infoHeader.biClrUsed, sizeof(infoHeader.biClrUsed), true);
            f.write(&infoHeader.biClrImportant, sizeof(infoHeader.biClrImportant), true);

            f.write(data.data(), static_cast<uint32_t>(data.size()), true);
        }

    private:
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> data;
    };
}

#endif
