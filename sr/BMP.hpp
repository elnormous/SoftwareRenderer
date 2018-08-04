//
//  SoftwareRenderer
//

#pragma once

#include <stdexcept>
#include <string>
#include <vector>
#include "File.hpp"

namespace sr
{
    class BMP
    {
    public:
        static const int BITMAPFILEHEADER_TYPE_BM = 0x4D42;

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

        BMP() {}

        BMP(const std::string& filename)
        {
            File f(filename, File::Mode::READ);

            BitmapFileHeader header;

            f.readAll(&header.bfType, sizeof(header.bfType));

            if (header.bfType != BITMAPFILEHEADER_TYPE_BM)
                throw std::runtime_error("Bad bitmap file");

            f.readAll(&header.bfSize, sizeof(header.bfSize));
            f.readAll(&header.bfReserved1, sizeof(header.bfReserved1));
            f.readAll(&header.bfReserved2, sizeof(header.bfReserved2));
            f.readAll(&header.bfOffBits, sizeof(header.bfOffBits));

            BitmapInfoHeader infoHeader;
            f.readAll(&infoHeader.biSize, sizeof(infoHeader.biSize));

            uint32_t offset = 0;

            if (offset + sizeof(infoHeader.biWidth) < infoHeader.biSize)
            {
                f.readAll(&infoHeader.biWidth, sizeof(infoHeader.biWidth));
                offset += sizeof(infoHeader.biWidth);
            }

            if (offset + sizeof(infoHeader.biHeight) < infoHeader.biSize)
            {
                f.readAll(&infoHeader.biHeight, sizeof(infoHeader.biHeight));
                offset += sizeof(infoHeader.biHeight);
            }

            if (offset + sizeof(infoHeader.biPlanes) < infoHeader.biSize)
            {
                f.readAll(&infoHeader.biPlanes, sizeof(infoHeader.biPlanes));
                offset += sizeof(infoHeader.biPlanes);
            }

            if (offset + sizeof(infoHeader.biBitCount) < infoHeader.biSize)
            {
                f.readAll(&infoHeader.biBitCount, sizeof(infoHeader.biBitCount));
                offset += sizeof(infoHeader.biBitCount);
            }

            if (offset + sizeof(infoHeader.biCompression) < infoHeader.biSize)
            {
                f.readAll(&infoHeader.biCompression, sizeof(infoHeader.biCompression));
                offset += sizeof(infoHeader.biCompression);
            }

            if (infoHeader.biCompression != RGB)
                throw std::runtime_error("Compression not supported");

            if (offset + sizeof(infoHeader.biSizeImage) < infoHeader.biSize)
            {
                f.readAll(&infoHeader.biSizeImage, sizeof(infoHeader.biSizeImage));
                offset += sizeof(infoHeader.biSizeImage);
            }

            if (offset + sizeof(infoHeader.biXPelsPerMeter) < infoHeader.biSize)
            {
                f.readAll(&infoHeader.biXPelsPerMeter, sizeof(infoHeader.biXPelsPerMeter));
                offset += sizeof(infoHeader.biXPelsPerMeter);
            }

            if (offset + sizeof(infoHeader.biYPelsPerMeter) < infoHeader.biSize)
            {
                f.readAll(&infoHeader.biYPelsPerMeter, sizeof(infoHeader.biYPelsPerMeter));
                offset += sizeof(infoHeader.biYPelsPerMeter);
            }

            if (offset + sizeof(infoHeader.biClrUsed) < infoHeader.biSize)
            {
                f.readAll(&infoHeader.biClrUsed, sizeof(infoHeader.biClrUsed));
                offset += sizeof(infoHeader.biClrUsed);
            }

            if (offset + sizeof(infoHeader.biClrImportant) < infoHeader.biSize)
            {
                f.readAll(&infoHeader.biClrImportant, sizeof(infoHeader.biClrImportant));
                offset += sizeof(infoHeader.biClrImportant);
            }

            f.seek(header.bfOffBits, File::Seek::BEGIN);

            data.resize(infoHeader.biWidth * std::abs(infoHeader.biHeight) * sizeof(RGBQuad));
            std::fill(data.begin(), data.end(), 255);

            if (infoHeader.biHeight > 0) // bottom to top
            {
                for (int y = infoHeader.biHeight - 1; y >= 0; --y)
                    for (int x = 0; x < infoHeader.biWidth; ++x)
                        f.readAll(&data[(y * infoHeader.biWidth + x) * sizeof(RGBQuad)], infoHeader.biBitCount / 8);
            }
            else // top to bottom
            {
                for (int y = 0; y < std::abs(infoHeader.biHeight); ++y)
                    for (int x = 0; x < infoHeader.biWidth; ++x)
                        f.readAll(&data[(y * infoHeader.biWidth + x) * sizeof(RGBQuad)], infoHeader.biBitCount / 8);
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
            File f(filename, File::Mode::WRITE);

            BitmapFileHeader header;
            header.bfType = BITMAPFILEHEADER_TYPE_BM;
            header.bfSize = width  * height * 4 + 14 + 40;
            header.bfReserved1 = 0;
            header.bfReserved2 = 0;
            header.bfOffBits = 14 + 40; // size of BITMAPFILEHEADER + size of BITMAPINFOHEADER

            f.write(&header.bfType, sizeof(header.bfType));
            f.write(&header.bfSize, sizeof(header.bfSize));
            f.write(&header.bfReserved1, sizeof(header.bfReserved1));
            f.write(&header.bfReserved2, sizeof(header.bfReserved2));
            f.write(&header.bfOffBits, sizeof(header.bfOffBits));

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

            f.write(&infoHeader.biSize, sizeof(infoHeader.biSize));
            f.write(&infoHeader.biWidth, sizeof(infoHeader.biWidth));
            f.write(&infoHeader.biHeight, sizeof(infoHeader.biHeight));
            f.write(&infoHeader.biPlanes, sizeof(infoHeader.biPlanes));
            f.write(&infoHeader.biBitCount, sizeof(infoHeader.biBitCount));
            f.write(&infoHeader.biCompression, sizeof(infoHeader.biCompression));
            f.write(&infoHeader.biSizeImage, sizeof(infoHeader.biSizeImage));
            f.write(&infoHeader.biXPelsPerMeter, sizeof(infoHeader.biXPelsPerMeter));
            f.write(&infoHeader.biYPelsPerMeter, sizeof(infoHeader.biYPelsPerMeter));
            f.write(&infoHeader.biClrUsed, sizeof(infoHeader.biClrUsed));
            f.write(&infoHeader.biClrImportant, sizeof(infoHeader.biClrImportant));

            f.write(data.data(), static_cast<uint32_t>(data.size()));
        }

    private:
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> data;
    };
}
