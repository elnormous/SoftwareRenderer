//
//  SoftwareRenderer
//

#include <cstdio>
#include <iostream>
#include "BMP.hpp"

#define BITMAPFILEHEADER_TYPE_BM    0x4D42

typedef struct                       /**** BMP file header structure ****/
{
    unsigned short bfType;           /* Magic number for file */
    unsigned int   bfSize;           /* Size of file */
    unsigned short bfReserved1;      /* Reserved */
    unsigned short bfReserved2;      /* ... */
    unsigned int   bfOffBits;        /* Offset to bitmap data */
} BITMAPFILEHEADER;

typedef struct                       /**** BMP file info structure ****/
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
} BITMAPINFOHEADER;

typedef struct                       /**** Colormap entry structure ****/
{
    unsigned char rgbBlue;          /* Blue value */
    unsigned char rgbGreen;         /* Green value */
    unsigned char rgbRed;           /* Red value */
    unsigned char rgbReserved;      /* Reserved */
} RGBQUAD;

typedef enum
{
    BI_RGB = 0x0000,
    BI_RLE8 = 0x0001,
    BI_RLE4 = 0x0002,
    BI_BITFIELDS = 0x0003,
    BI_JPEG = 0x0004,
    BI_PNG = 0x0005,
    BI_CMYK = 0x000B,
    BI_CMYKRLE8 = 0x000C,
    BI_CMYKRLE4 = 0x000D
} Compression;

namespace sr
{
    Buffer BMP::getBuffer()
    {
        Buffer buffer;
        buffer.init(sr::Buffer::Type::RGBA, width, height);
        buffer.setDate(data);

        return buffer;
    }

    bool BMP::setBuffer(const Buffer& buffer)
    {
        if (buffer.getType() != sr::Buffer::Type::RGBA) return false;

        width = buffer.getWidth();
        height = buffer.getHeight();
        data = buffer.getData();

        return true;
    }

    bool BMP::load(const std::string& filename)
    {
        FILE* f = fopen(filename.c_str(), "rb");

        if (!f)
        {
            std::cerr << "Failed to open " << filename << std::endl;
            return false;
        }

        BITMAPFILEHEADER header;

        fread(&header.bfType, sizeof(header.bfType), 1, f);

        if (header.bfType != BITMAPFILEHEADER_TYPE_BM)
        {
            std::cerr << "Bad bitmap file" << std::endl;
            fclose(f);
            return false;
        }

        fread(&header.bfSize, sizeof(header.bfSize), 1, f);
        fread(&header.bfReserved1, sizeof(header.bfReserved1), 1, f);
        fread(&header.bfReserved2, sizeof(header.bfReserved2), 1, f);
        fread(&header.bfOffBits, sizeof(header.bfOffBits), 1, f);

        BITMAPINFOHEADER infoHeader;
        fread(&infoHeader.biSize, sizeof(infoHeader.biSize), 1, f);

        uint32_t offset = 0;

        if (offset + sizeof(infoHeader.biWidth) < infoHeader.biSize)
        {
            fread(&infoHeader.biWidth, sizeof(infoHeader.biWidth), 1, f);
            offset += sizeof(infoHeader.biWidth);
        }

        if (offset + sizeof(infoHeader.biHeight) < infoHeader.biSize)
        {
            fread(&infoHeader.biHeight, sizeof(infoHeader.biHeight), 1, f);
            offset += sizeof(infoHeader.biHeight);
        }

        if (offset + sizeof(infoHeader.biPlanes) < infoHeader.biSize)
        {
            fread(&infoHeader.biPlanes, sizeof(infoHeader.biPlanes), 1, f);
            offset += sizeof(infoHeader.biPlanes);
        }

        if (offset + sizeof(infoHeader.biBitCount) < infoHeader.biSize)
        {
            fread(&infoHeader.biBitCount, sizeof(infoHeader.biBitCount), 1, f);
            offset += sizeof(infoHeader.biBitCount);
        }

        if (offset + sizeof(infoHeader.biCompression) < infoHeader.biSize)
        {
            fread(&infoHeader.biCompression, sizeof(infoHeader.biCompression), 1, f);
            offset += sizeof(infoHeader.biCompression);
        }

        if (infoHeader.biCompression != BI_RGB)
        {
            std::cerr << "Compression not supported" << std::endl;
            fclose(f);
            return false;
        }

        if (offset + sizeof(infoHeader.biSizeImage) < infoHeader.biSize)
        {
            fread(&infoHeader.biSizeImage, sizeof(infoHeader.biSizeImage), 1, f);
            offset += sizeof(infoHeader.biSizeImage);
        }

        if (offset + sizeof(infoHeader.biXPelsPerMeter) < infoHeader.biSize)
        {
            fread(&infoHeader.biXPelsPerMeter, sizeof(infoHeader.biXPelsPerMeter), 1, f);
            offset += sizeof(infoHeader.biXPelsPerMeter);
        }

        if (offset + sizeof(infoHeader.biYPelsPerMeter) < infoHeader.biSize)
        {
            fread(&infoHeader.biYPelsPerMeter, sizeof(infoHeader.biYPelsPerMeter), 1, f);
            offset += sizeof(infoHeader.biYPelsPerMeter);
        }

        if (offset + sizeof(infoHeader.biClrUsed) < infoHeader.biSize)
        {
            fread(&infoHeader.biClrUsed, sizeof(infoHeader.biClrUsed), 1, f);
            offset += sizeof(infoHeader.biClrUsed);
        }

        if (offset + sizeof(infoHeader.biClrImportant) < infoHeader.biSize)
        {
            fread(&infoHeader.biClrImportant, sizeof(infoHeader.biClrImportant), 1, f);
            offset += sizeof(infoHeader.biClrImportant);
        }

        fseek(f, header.bfOffBits, SEEK_SET);

        data.resize(infoHeader.biWidth * std::abs(infoHeader.biHeight) * sizeof(RGBQUAD));
        std::fill(data.begin(), data.end(), 255);

        if (infoHeader.biHeight > 0) // bottom to top
        {
            for (int y = infoHeader.biHeight - 1; y >= 0; --y)
            {
                for (int x = 0; x < infoHeader.biWidth; ++x)
                {
                    fread(&data[(y * infoHeader.biWidth + x) * sizeof(RGBQUAD)], infoHeader.biBitCount / 8, 1, f);
                }
            }
        }
        else // top to bottom
        {
            for (int y = 0; y < std::abs(infoHeader.biHeight); ++y)
            {
                for (int x = 0; x < infoHeader.biWidth; ++x)
                {
                    fread(&data[(y * infoHeader.biWidth + x) * sizeof(RGBQUAD)], infoHeader.biBitCount / 8, 1, f);
                }
            }
        }

        fclose(f);

        width = static_cast<uint32_t>(infoHeader.biWidth);
        height = static_cast<uint32_t>(std::abs(infoHeader.biHeight));

        return true;
    }

    bool BMP::save(const std::string& filename)
    {
        FILE* f = fopen(filename.c_str(), "wb");

        if (!f)
        {
            std::cerr << "Failed to open " << filename << std::endl;
            return 0;
        }

        BITMAPFILEHEADER header;
        header.bfType = BITMAPFILEHEADER_TYPE_BM;
        header.bfSize = width  * height * 4 + 14 + 40;
        header.bfReserved1 = 0;
        header.bfReserved2 = 0;
        header.bfOffBits = 14 + 40; // size of BITMAPFILEHEADER + size of BITMAPINFOHEADER

        fwrite(&header.bfType, sizeof(header.bfType), 1, f);
        fwrite(&header.bfSize, sizeof(header.bfSize), 1, f);
        fwrite(&header.bfReserved1, sizeof(header.bfReserved1), 1, f);
        fwrite(&header.bfReserved2, sizeof(header.bfReserved2), 1, f);
        fwrite(&header.bfOffBits, sizeof(header.bfOffBits), 1, f);

        BITMAPINFOHEADER infoHeader;
        infoHeader.biSize = 40;
        infoHeader.biWidth = (int)width;
        infoHeader.biHeight = -(int)height;
        infoHeader.biPlanes = 1;
        infoHeader.biBitCount = 32;
        infoHeader.biCompression = 0;
        infoHeader.biSizeImage = 0;
        infoHeader.biXPelsPerMeter = 0;
        infoHeader.biYPelsPerMeter = 0;
        infoHeader.biClrUsed = 0;
        infoHeader.biClrImportant = 0;

        fwrite(&infoHeader.biSize, sizeof(infoHeader.biSize), 1, f);
        fwrite(&infoHeader.biWidth, sizeof(infoHeader.biWidth), 1, f);
        fwrite(&infoHeader.biHeight, sizeof(infoHeader.biHeight), 1, f);
        fwrite(&infoHeader.biPlanes, sizeof(infoHeader.biPlanes), 1, f);
        fwrite(&infoHeader.biBitCount, sizeof(infoHeader.biBitCount), 1, f);
        fwrite(&infoHeader.biCompression, sizeof(infoHeader.biCompression), 1, f);
        fwrite(&infoHeader.biSizeImage, sizeof(infoHeader.biSizeImage), 1, f);
        fwrite(&infoHeader.biXPelsPerMeter, sizeof(infoHeader.biXPelsPerMeter), 1, f);
        fwrite(&infoHeader.biYPelsPerMeter, sizeof(infoHeader.biYPelsPerMeter), 1, f);
        fwrite(&infoHeader.biClrUsed, sizeof(infoHeader.biClrUsed), 1, f);
        fwrite(&infoHeader.biClrImportant, sizeof(infoHeader.biClrImportant), 1, f);

        fwrite(data.data(), 1, data.size(), f);

        fclose(f);

        return true;
    }
}
