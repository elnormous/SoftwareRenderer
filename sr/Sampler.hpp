//
//  SoftwareRenderer
//

#pragma once

#include "Buffer.hpp"
#include "Color.hpp"
#include "Vector2.hpp"

namespace sr
{
    class Sampler
    {
    public:
        Sampler()
        {
        }

        explicit Sampler(const Buffer* initBuffer):
            buffer(initBuffer)
        {
        }

        Color get(Vector2 coord) const
        {
            Color result;

            if (buffer)
            {
                uint32_t textureX = static_cast<uint32_t>(coord.x * (buffer->getWidth() - 1));
                uint32_t textureY = static_cast<uint32_t>(coord.y * (buffer->getHeight() - 1));

                if (buffer->getType() == sr::Buffer::Type::RGB)
                {
                    const uint8_t* rgb = &buffer->getData()[(textureY * buffer->getWidth() + textureX) * 3];
                    result = sr::Color(rgb[0], rgb[1], rgb[2], 255);
                }
                else if (buffer->getType() == sr::Buffer::Type::RGBA)
                {
                    const uint8_t* rgba = &buffer->getData()[(textureY * buffer->getWidth() + textureX) * 4];
                    result = sr::Color(rgba[0], rgba[1], rgba[2], rgba[3]);
                }
            }

            return result;
        }

    private:
        const Buffer* buffer = nullptr;
    };
}
