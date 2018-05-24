//
//  SoftwareRenderer
//

#pragma once

#include "Color.hpp"
#include "Texture.hpp"
#include "Vector.hpp"

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

        Color sample(Vector2F coord) const
        {
            Color result;

            if (texture && texture->getLevelCount())
            {
                const Buffer& buffer = texture->getLevel(0);

                if (!buffer.isEmpty())
                {
                    switch (addressModeX)
                    {
                        case AddressMode::CLAMP: coord.v[0] = clamp(coord.v[0], 0.0F, 1.0F); break;
                        case AddressMode::REPEAT: coord.v[0] = fmodf(coord.v[0], 1.0F); break;
                    }

                    switch (addressModeY)
                    {
                        case AddressMode::CLAMP: coord.v[1] = clamp(coord.v[1], 0.0F, 1.0F); break;
                        case AddressMode::REPEAT: coord.v[1] = fmodf(coord.v[1], 1.0F); break;
                    }

                    uint32_t textureX = static_cast<uint32_t>(coord.v[0] * (buffer.getWidth() - 1));
                    uint32_t textureY = static_cast<uint32_t>(coord.v[1] * (buffer.getHeight() - 1));

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
            }

            return result;
        }

    private:
        const Texture* texture = nullptr;
        AddressMode addressModeX = AddressMode::CLAMP;
        AddressMode addressModeY = AddressMode::CLAMP;
    };
}
