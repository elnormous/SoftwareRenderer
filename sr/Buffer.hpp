//
//  SoftwareRenderer
//

#pragma once

#include <cstdint>
#include <vector>

namespace sr
{
    class Buffer
    {
    public:
        enum class Type
        {
            NONE,
            RGB,
            RGBA,
            FLOAT32
        };

        Buffer() {}
        Buffer(const Buffer& other)
        {
            type = other.type;
            width = other.width;
            height = other.height;
            data = other.data;
        }

        Buffer& operator=(const Buffer& other)
        {
            type = other.type;
            width = other.width;
            height = other.height;
            data = other.data;

            return *this;
        }

        Buffer(Buffer&& other)
        {
            type = other.type;
            width = other.width;
            height = other.height;
            data = std::move(other.data);

            other.type = Type::NONE;
            other.width = 0;
            other.height = 0;
        }

        Buffer& operator=(Buffer&& other)
        {
            if (&other != this)
            {
                type = other.type;
                width = other.width;
                height = other.height;
                data = std::move(other.data);

                other.type = Type::NONE;
                other.width = 0;
                other.height = 0;
            }

            return *this;
        }

        ~Buffer() {}

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

        bool init(Type initType, uint32_t initWidth, uint32_t initHeight)
        {
            type = initType;
            width = initWidth;
            height = initHeight;

            uint32_t pixelSize = getPixelSize(type);
            if (pixelSize == 0) return false;

            data.resize(width * height * pixelSize);

            return true;
        }

        bool resize(uint32_t newWidth, uint32_t newHeight)
        {
            width = newWidth;
            height = newHeight;

            uint32_t pixelSize = getPixelSize(type);
            if (pixelSize == 0) return false;

            data.resize(width * height * pixelSize);

            return true;
        }

        inline Type getType() const { return type; }
        inline uint32_t getWidth() const { return width; }
        inline uint32_t getHeight() const { return height; }

        inline std::vector<uint8_t>& getData() { return data; }
        inline const std::vector<uint8_t>& getData() const { return data; }
        inline void setDate(std::vector<uint8_t>& newData) { data = newData; }

    private:
        Type type = Type::NONE;
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> data;
    };
}
