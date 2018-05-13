//
//  SoftwareRenderer
//

#pragma once

#include <cstdint>

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
        Buffer(const Buffer& other);
        Buffer& operator=(const Buffer& other);
        Buffer(Buffer&& other);
        Buffer& operator=(Buffer&& other);
        ~Buffer();

        bool init(Type initType, uint32_t initWidth, uint32_t initHeight);
        bool resize(uint32_t newWidth, uint32_t newHeight);

        Type getType() const { return type; }
        uint32_t getWidth() const { return width; }
        uint32_t getHeight() const { return height; }
        void* getData() const { return data; }

    private:
        Type type = Type::NONE;
        uint32_t width = 0;
        uint32_t height = 0;
        void* data = nullptr;
    };
}
