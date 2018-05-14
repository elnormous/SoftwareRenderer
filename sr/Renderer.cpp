//
//  SoftwareRenderer
//

#include <iostream>
#include "Renderer.hpp"
#include "Box2.hpp"
#include "MathUtils.hpp"

namespace sr
{
    Renderer::Renderer()
    {

    }

    bool Renderer::init(uint32_t width, uint32_t height)
    {
        if (!frameBuffer.init(Buffer::Type::RGBA, width, height)) return false;
        if (!depthBuffer.init(Buffer::Type::FLOAT32, width, height)) return false;

        return true;
    }

    bool Renderer::resize(uint32_t width, uint32_t height)
    {
        if (!frameBuffer.resize(width, height)) return false;
        if (!depthBuffer.resize(width, height)) return false;

        return true;
    }

    void Renderer::setShader(const Shader& newShader)
    {
        shader = &newShader;
    }

    void Renderer::setTexture(const Buffer& newTexture)
    {
        texture = &newTexture;
    }

    bool Renderer::clear(Color color, float depth)
    {
        uint32_t* frameBufferData = reinterpret_cast<uint32_t*>(frameBuffer.getData().data());
        float* depthBufferData = reinterpret_cast<float*>(depthBuffer.getData().data());
        uint32_t rgba = color.getIntValueRaw();

        for (uint32_t y = 0; y < frameBuffer.getHeight(); ++y)
        {
            for (uint32_t x = 0; x < frameBuffer.getWidth(); ++x)
            {
                frameBufferData[y * frameBuffer.getWidth() + x] = rgba;
            }
        }

        for (uint32_t y = 0; y < frameBuffer.getHeight(); ++y)
        {
            for (uint32_t x = 0; x < frameBuffer.getWidth(); ++x)
            {
                depthBufferData[y * frameBuffer.getWidth() + x] = depth;
            }
        }

        return true;
    }

    bool Renderer::drawTriangle(Vertex vertices[3])
    {
        uint32_t* frameBufferData = reinterpret_cast<uint32_t*>(frameBuffer.getData().data());

        Vector2 positions[3] = {
            vertices[0].position,
            vertices[1].position,
            vertices[2].position
        };

        Box2 boundingBox;
        for (uint32_t i = 0; i < 3; ++i)
        {
            if (positions[i].x < boundingBox.min.x) boundingBox.min.x = positions[i].x;
            if (positions[i].x > boundingBox.max.x) boundingBox.max.x = positions[i].x;
            if (positions[i].y < boundingBox.min.y) boundingBox.min.y = positions[i].y;
            if (positions[i].y > boundingBox.max.y) boundingBox.max.y = positions[i].y;
        }

        boundingBox.min.x = clamp(boundingBox.min.x, 0.0F, static_cast<float>(frameBuffer.getWidth() - 1));
        boundingBox.max.x = clamp(boundingBox.max.x, 0.0F, static_cast<float>(frameBuffer.getHeight() - 1));
        boundingBox.min.y = clamp(boundingBox.min.y, 0.0F, static_cast<float>(frameBuffer.getWidth() - 1));
        boundingBox.max.y = clamp(boundingBox.max.y, 0.0F, static_cast<float>(frameBuffer.getHeight() - 1));

        for (uint32_t y = static_cast<uint32_t>(boundingBox.min.y); y <= static_cast<uint32_t>(boundingBox.max.y); ++y)
        {
            for (uint32_t x = static_cast<uint32_t>(boundingBox.min.x); x <= static_cast<uint32_t>(boundingBox.max.x); ++x)
            {
                Vector3 s = barycentric(positions[0],
                                        positions[1],
                                        positions[2],
                                        Vector2(x, y));

                if (s.x >= 0.0f && s.x <= 1.0f &&
                    s.y >= 0.0f && s.y <= 1.0f &&
                    s.z >= 0.0f && s.z <= 1.0f)
                {
                    float finalRGBA[4] = {
                        vertices[0].color.normR() * s.x + vertices[1].color.normR() * s.y + vertices[2].color.normR() * s.z,
                        vertices[0].color.normG() * s.x + vertices[1].color.normG() * s.y + vertices[2].color.normG() * s.z,
                        vertices[0].color.normB() * s.x + vertices[1].color.normB() * s.y + vertices[2].color.normB() * s.z,
                        vertices[0].color.normA() * s.x + vertices[1].color.normA() * s.y + vertices[2].color.normA() * s.z,
                    };

                    Vector2 texCoords(
                        clamp(vertices[0].texCoords[0].x * s.x + vertices[1].texCoords[0].x * s.y + vertices[2].texCoords[0].x * s.z, 0.0F, 1.0F),
                        clamp(vertices[0].texCoords[0].y * s.x + vertices[1].texCoords[0].y * s.y + vertices[2].texCoords[0].y * s.z, 0.0F, 1.0F)
                    );

                    if (texture)
                    {
                        uint32_t textureX = static_cast<uint32_t>(texCoords.x * texture->getWidth());
                        uint32_t textureY = static_cast<uint32_t>(texCoords.y * texture->getHeight());

                        if (texture->getType() == Buffer::Type::RGB)
                        {
                            const uint8_t* sample = &texture->getData()[(textureY * texture->getWidth() + textureX) * 3];
                            finalRGBA[0] *= sample[0] / 255.0F;
                            finalRGBA[1] *= sample[1] / 255.0F;
                            finalRGBA[2] *= sample[2] / 255.0F;
                        }
                        else if (texture->getType() == Buffer::Type::RGBA)
                        {
                            const uint8_t* sample = &texture->getData()[(textureY * texture->getWidth() + textureX) * 4];
                            finalRGBA[0] *= sample[0] / 255.0F;
                            finalRGBA[1] *= sample[1] / 255.0F;
                            finalRGBA[2] *= sample[2] / 255.0F;
                            finalRGBA[3] *= sample[3] / 255.0F;
                        }
                    }

                    Color color(finalRGBA);
                    frameBufferData[y * frameBuffer.getWidth() + x] = color.getIntValueRaw();
                }
            }
        }

        return true;
    }
}
