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

        enum class Filter
        {
            POINT,
            LINEAR
        };

        Sampler()
        {
        }

        explicit Sampler(const Texture* initTexture):
            texture(initTexture)
        {
        }

        inline void setAddressModeX(AddressMode addressMode)
        {
            addressModeX = addressMode;
        }

        inline void setAddressModeY(AddressMode addressMode)
        {
            addressModeY = addressMode;
        }

        inline void setFilter(Filter newFilter)
        {
            filter = newFilter;
        }

        Color sample(Vector2F coord) const
        {
            Color result;

            if (texture && texture->getLevelCount())
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

                float coordX = coord.v[0] * (texture->getWidth() - 1);
                float coordY = coord.v[1] * (texture->getHeight() - 1);

                if (filter == Filter::POINT)
                {
                    uint32_t textureX = static_cast<uint32_t>(roundf(coordX));
                    uint32_t textureY = static_cast<uint32_t>(roundf(coordY));
                    result = texture->getPixel(textureX, textureY, 0);
                }
                else
                {
                    uint32_t textureX0 = static_cast<uint32_t>(coordX - 0.5F);
                    uint32_t textureX1 = textureX0 + 1;
                    uint32_t textureY0 = static_cast<uint32_t>(coordY - 0.5F);
                    uint32_t textureY1 = textureY0 + 1;

                    textureX0 = clamp(textureX0, 0U, texture->getWidth() - 1);
                    textureX1 = clamp(textureX1, 0U, texture->getWidth() - 1);
                    textureY0 = clamp(textureY0, 0U, texture->getHeight() - 1);
                    textureY1 = clamp(textureY1, 0U, texture->getHeight() - 1);

                    Color color[4] = {
                        texture->getPixel(textureX0, textureY0, 0),
                        texture->getPixel(textureX1, textureY0, 0),
                        texture->getPixel(textureX0, textureY1, 0),
                        texture->getPixel(textureX1, textureY1, 0)
                    };

                    float x0 = coordX - (textureX0 + 0.5F);
                    float y0 = coordY - (textureY0 + 0.5F);
                    float x1 = (textureX0 + 1.5F) - coordX;
                    float y1 = (textureY0 + 1.5F) - coordY;

                    result.r = color[0].r * x1 * y1 + color[1].r * x0 * y1 + color[2].r * x1 * y0 + color[3].r * x0 * y0;
                    result.g = color[0].g * x1 * y1 + color[1].g * x0 * y1 + color[2].g * x1 * y0 + color[3].g * x0 * y0;
                    result.b = color[0].b * x1 * y1 + color[1].b * x0 * y1 + color[2].b * x1 * y0 + color[3].b * x0 * y0;
                    result.a = color[0].a * x1 * y1 + color[1].a * x0 * y1 + color[2].a * x1 * y0 + color[3].a * x0 * y0;
                }
            }

            return result;
        }

    private:
        const Texture* texture = nullptr;
        AddressMode addressModeX = AddressMode::CLAMP;
        AddressMode addressModeY = AddressMode::CLAMP;
        Filter filter = Filter::POINT;
    };
}
