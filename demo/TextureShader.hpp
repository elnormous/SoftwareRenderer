//
//  SoftwareRenderer
//

#pragma once

#include "sr.hpp"

namespace demo
{
    class TextureShader: public sr::Shader
    {
    public:
        virtual VSOutput vertexShader(const sr::Matrix4& modelViewProjection, const sr::Vertex& vertex) const
        {
            VSOutput result;

            // transform to clip space
            result.position = modelViewProjection * vertex.position;
            result.color = vertex.color;
            result.texCoords[0] = vertex.texCoords[0];
            result.texCoords[1] = vertex.texCoords[1];
            result.normal = vertex.normal;

            return result;
        }

        virtual sr::Color fragmentShader(VSOutput input, const sr::Buffer* texture[2]) const
        {
            sr::Color sampleColor;

            if (texture[0])
            {
                uint32_t textureX = static_cast<uint32_t>(input.texCoords[0].x * (texture[0]->getWidth() - 1));
                uint32_t textureY = static_cast<uint32_t>(input.texCoords[0].y * (texture[0]->getHeight() - 1));

                if (texture[0]->getType() == sr::Buffer::Type::RGB)
                {
                    const uint8_t* rgb = &texture[0]->getData()[(textureY * texture[0]->getWidth() + textureX) * 3];
                    sampleColor = sr::Color(rgb[0], rgb[1], rgb[2], 255);
                }
                else if (texture[0]->getType() == sr::Buffer::Type::RGBA)
                {
                    const uint8_t* rgba = &texture[0]->getData()[(textureY * texture[0]->getWidth() + textureX) * 4];
                    sampleColor = sr::Color(rgba[0], rgba[1], rgba[2], rgba[3]);
                }
            }

            sr::Color result;
            result.r = input.color.r * sampleColor.r;
            result.g = input.color.g * sampleColor.g;
            result.b = input.color.b * sampleColor.b;
            result.a = input.color.a * sampleColor.a;

            return result;
        }
    };
}
