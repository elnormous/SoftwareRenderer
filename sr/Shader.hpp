//
//  SoftwareRenderer
//

#pragma once

#include "Buffer.hpp"
#include "Matrix4.hpp"
#include "Vertex.hpp"

namespace sr
{
    class Shader
    {
    public:
        struct VSOutput
        {
            Vector4 position;
            Color color;
            Vector2 texCoords[2];
            Vector3 normal;
        };

        VSOutput vertexShader(const Matrix4& modelViewProjection, const Vertex& vertex) const
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

        Color fragmentShader(VSOutput input, const Buffer* texture) const
        {
            Color sampleColor;

            if (texture)
            {
                uint32_t textureX = static_cast<uint32_t>(input.texCoords[0].x * (texture->getWidth() - 1));
                uint32_t textureY = static_cast<uint32_t>(input.texCoords[0].y * (texture->getHeight() - 1));

                if (texture->getType() == Buffer::Type::RGB)
                {
                    const uint8_t* rgb = &texture->getData()[(textureY * texture->getWidth() + textureX) * 3];
                    sampleColor = Color(rgb[0], rgb[1], rgb[2], 255);
                }
                else if (texture->getType() == Buffer::Type::RGBA)
                {
                    const uint8_t* rgba = &texture->getData()[(textureY * texture->getWidth() + textureX) * 4];
                    sampleColor = Color(rgba[0], rgba[1], rgba[2], rgba[3]);
                }
            }

            Color result;
            result.r = input.color.r * sampleColor.r;
            result.g = input.color.g * sampleColor.g;
            result.b = input.color.b * sampleColor.b;
            result.a = input.color.a * sampleColor.a;

            return result;
        }
    };
}
