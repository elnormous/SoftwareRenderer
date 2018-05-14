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
        void* getData() { return data.data(); }
        const void* getData() const { return data.data(); }

    private:
        Type type = Type::NONE;
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> data;
    };
}
