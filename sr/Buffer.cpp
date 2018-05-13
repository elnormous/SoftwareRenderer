//
//  SoftwareRenderer
//

#include <cstdlib>
#include <cstring>
#include "Buffer.hpp"

namespace sr
{
    static uint32_t getPixelSize(Buffer::Type type)
    {
        switch (type)
        {
            case Buffer::Type::RGB:
                return sizeof(uint8_t) * 3;
                break;
            case Buffer::Type::RGBA:
                return sizeof(uint8_t) * 4;
                break;
            case Buffer::Type::FLOAT32:
                return sizeof(float);
                break;
            default:
                return 0;
        }
    }

    Buffer::Buffer(const Buffer& other)
    {
        type = other.type;
        width = other.width;
        height = other.height;

        if (other.data)
        {
            uint32_t pixelSize = getPixelSize(type);
            if (pixelSize == 0) return;

            data = malloc(width * height * pixelSize);
            memcpy(data, other.data, width * height * pixelSize);
        }
    }

    Buffer& Buffer::operator=(const Buffer& other)
    {
        if (data)
        {
            free(data);
            data = nullptr;
        }

        type = other.type;
        width = other.width;
        height = other.height;

        if (other.data)
        {
            uint32_t pixelSize = getPixelSize(type);
            if (pixelSize == 0) return *this;

            data = malloc(width * height * pixelSize);
            memcpy(data, other.data, width * height * pixelSize);
        }

        return *this;
    }

    Buffer::Buffer(Buffer&& other)
    {
        type = other.type;
        width = other.width;
        height = other.height;
        data = other.data;

        other.type = Type::NONE;
        other.width = 0;
        other.height = 0;
        other.data = nullptr;
    }

    Buffer& Buffer::operator=(Buffer&& other)
    {
        if (&other != this)
        {
            if (data) free(data);

            type = other.type;
            width = other.width;
            height = other.height;
            data = other.data;

            other.type = Type::NONE;
            other.width = 0;
            other.height = 0;
            other.data = nullptr;
        }

        return *this;
    }

    Buffer::~Buffer()
    {
        if (data) free(data);
    }

    bool Buffer::init(Type initType, uint32_t initWidth, uint32_t initHeight)
    {
        if (data)
        {
            free(data);
            data = nullptr;
        }

        type = initType;
        width = initWidth;
        height = initHeight;

        uint32_t pixelSize = getPixelSize(type);
        if (pixelSize == 0) return false;

        data = malloc(width * height * pixelSize);

        if (!data) return false;

        return true;
    }

    bool Buffer::resize(uint32_t newWidth, uint32_t newHeight)
    {
        if (data)
        {
            free(data);
            data = nullptr;
        }

        width = newWidth;
        height = newHeight;

        uint32_t pixelSize = getPixelSize(type);
        if (pixelSize == 0) return false;

        data = malloc(width * height * pixelSize);

        if (!data) return false;

        return true;
    }
}
