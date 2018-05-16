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

    bool Renderer::setViewport(const Rect& newViewport)
    {
        viewport = newViewport;
        return true;
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

    bool Renderer::drawTriangles(const std::vector<uint32_t>& indices, const std::vector<Vertex>& vertices, const Matrix4& modelViewProjection)
    {
        if (!shader) return false;

        uint32_t* frameBufferData = reinterpret_cast<uint32_t*>(frameBuffer.getData().data());
        float* depthBufferData = reinterpret_cast<float*>(depthBuffer.getData().data());

        for (uint32_t i = 0; i < indices.size(); i += 3)
        {
            Shader::VSOutput vsOutputs[3];

            // vertex shader step
            for (uint32_t c = 0; c < 3; ++c)
            {
                uint32_t index = indices[i + c];
                vsOutputs[c] = shader->vertexShader(modelViewProjection, vertices[index]);
            }

            Vector4 ndcPositions[3] = {
                vsOutputs[0].position,
                vsOutputs[1].position,
                vsOutputs[2].position
            };

            for (sr::Vector4& ndcPosition : ndcPositions)
            {
                // transform to normalized device coordinates
                ndcPosition /= ndcPosition.w;
            }

            Vector4 viewportPositions[3] = {
                ndcPositions[0],
                ndcPositions[1],
                ndcPositions[2]
            };

            Box2 boundingBox;
            for (sr::Vector4& viewportPosition : viewportPositions)
            {
                // transform to viewport coordinates
                viewportPosition.x = viewportPosition.x * viewport.size.width / 2.0F + viewport.position.x + viewport.size.width / 2.0F; // xndc * width / 2 + x + width / 2
                viewportPosition.y = viewportPosition.y * viewport.size.height / 2.0F + viewport.position.y + viewport.size.height / 2.0F;  // yndc * height / 2 + y + height / 2
                viewportPosition.z = viewportPosition.z * (1.0F - 0.0F) / 2.0F + (1.0F + 0.0F) / 2.0F; // zndc * (far - near) / 2 + (far + near) / 2

                if (viewportPosition.x < boundingBox.min.x) boundingBox.min.x = viewportPosition.x;
                if (viewportPosition.x > boundingBox.max.x) boundingBox.max.x = viewportPosition.x;
                if (viewportPosition.y < boundingBox.min.y) boundingBox.min.y = viewportPosition.y;
                if (viewportPosition.y > boundingBox.max.y) boundingBox.max.y = viewportPosition.y;
            }

            boundingBox.min.x = clamp(boundingBox.min.x, 0.0F, static_cast<float>(frameBuffer.getWidth() - 1));
            boundingBox.max.x = clamp(boundingBox.max.x, 0.0F, static_cast<float>(frameBuffer.getWidth() - 1));
            boundingBox.min.y = clamp(boundingBox.min.y, 0.0F, static_cast<float>(frameBuffer.getHeight() - 1));
            boundingBox.max.y = clamp(boundingBox.max.y, 0.0F, static_cast<float>(frameBuffer.getHeight() - 1));

            for (uint32_t screenY = static_cast<uint32_t>(boundingBox.min.y); screenY <= static_cast<uint32_t>(boundingBox.max.y); ++screenY)
            {
                for (uint32_t screenX = static_cast<uint32_t>(boundingBox.min.x); screenX <= static_cast<uint32_t>(boundingBox.max.x); ++screenX)
                {
                    Vector3 s = barycentric(viewportPositions[0],
                                            viewportPositions[1],
                                            viewportPositions[2],
                                            Vector2(screenX, screenY));

                    Vector3 clip = Vector3(s.x / vsOutputs[0].position.w, s.y / vsOutputs[1].position.w, s.z / vsOutputs[2].position.w);
                    clip = clip / (clip.x + clip.y + clip.z);

                    float depth = vsOutputs[0].position.z * clip.x + vsOutputs[1].position.z * clip.y + vsOutputs[2].position.z * clip.z;

                    if (s.x >= 0.0F && s.y >= 0.0F && s.z >= 0.0F && depthBufferData[screenY * depthBuffer.getWidth() + screenX] > depth)
                    {
                        depthBufferData[screenY * depthBuffer.getWidth() + screenX] = depth;

                        // pixel shader step
                        float finalRGBA[4] = {
                            vsOutputs[0].color.normR() * clip.x + vsOutputs[1].color.normR() * clip.y + vsOutputs[2].color.normR() * clip.z,
                            vsOutputs[0].color.normG() * clip.x + vsOutputs[1].color.normG() * clip.y + vsOutputs[2].color.normG() * clip.z,
                            vsOutputs[0].color.normB() * clip.x + vsOutputs[1].color.normB() * clip.y + vsOutputs[2].color.normB() * clip.z,
                            vsOutputs[0].color.normA() * clip.x + vsOutputs[1].color.normA() * clip.y + vsOutputs[2].color.normA() * clip.z,
                        };

                        Vector2 texCoords(
                            clamp(vsOutputs[0].texCoords[0].x * clip.x + vsOutputs[1].texCoords[0].x * clip.y + vsOutputs[2].texCoords[0].x * clip.z, 0.0F, 1.0F),
                            clamp(vsOutputs[0].texCoords[0].y * clip.x + vsOutputs[1].texCoords[0].y * clip.y + vsOutputs[2].texCoords[0].y * clip.z, 0.0F, 1.0F)
                        );

                        if (texture)
                        {
                            uint32_t textureX = static_cast<uint32_t>(texCoords.x * (texture->getWidth() - 1));
                            uint32_t textureY = static_cast<uint32_t>(texCoords.y * (texture->getHeight() - 1));

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
                        frameBufferData[screenY * frameBuffer.getWidth() + screenX] = color.getIntValueRaw();
                    }
                }
            }
        }

        return true;
    }
}
