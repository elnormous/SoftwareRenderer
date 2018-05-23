//
//  SoftwareRenderer
//

#pragma once

#include "Color.hpp"
#include "Texture.hpp"
#include "Vector2.hpp"

namespace sr
{
    class Sampler
    {
    public:
        enum AddressMode
        {
            CLAMP,
            REPEAT
        };

        Sampler()
        {
        }

        explicit Sampler(const Texture* initTexture):
            texture(initTexture)
        {
        }

        void setAddressModeX(AddressMode addressMode)
        {
            addressModeX = addressMode;
        }

        void setAddressModeY(AddressMode addressMode)
        {
            addressModeY = addressMode;
        }

        Color sample(Vector2 coord) const
        {
            Color result;

            if (texture)
            {
                const Buffer& buffer = texture->getLevel(0);

                switch (addressModeX)
                {
                    case AddressMode::CLAMP: coord.x = clamp(coord.x, 0.0F, 1.0F); break;
                    case AddressMode::REPEAT: coord.x = fmodf(coord.x, 1.0F); break;
                }

                switch (addressModeY)
                {
                    case AddressMode::CLAMP: coord.y = clamp(coord.y, 0.0F, 1.0F); break;
                    case AddressMode::REPEAT: coord.y = fmodf(coord.y, 1.0F); break;
                }

                uint32_t textureX = static_cast<uint32_t>(coord.x * (buffer.getWidth() - 1));
                uint32_t textureY = static_cast<uint32_t>(coord.y * (buffer.getHeight() - 1));

                if (buffer.getType() == sr::Buffer::Type::RGB)
                {
                    const uint8_t* rgb = &buffer.getData()[(textureY * buffer.getWidth() + textureX) * 3];
                    result = sr::Color(rgb[0], rgb[1], rgb[2], 255);
                }
                else if (buffer.getType() == sr::Buffer::Type::RGBA)
                {
                    const uint8_t* rgba = &buffer.getData()[(textureY * buffer.getWidth() + textureX) * 4];
                    result = sr::Color(rgba[0], rgba[1], rgba[2], rgba[3]);
                }
            }

            return result;
        }

    private:
        const Texture* texture = nullptr;
        AddressMode addressModeX = AddressMode::CLAMP;
        AddressMode addressModeY = AddressMode::CLAMP;
    };
}
