//
//  SoftwareRenderer
//

#include <cassert>
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

    void Renderer::setTexture(const Texture& newTexture, uint32_t level)
    {
        assert(level < 2);

        samplers[level] = Sampler(&newTexture);
    }

    void Renderer::setViewport(const Rect& newViewport)
    {
        viewport = newViewport;
    }

    void Renderer::setScissorRect(const Rect& newScissorRect)
    {
        scissorRect = newScissorRect;
    }

    void Renderer::setBlendState(const BlendState& newBlendState)
    {
        blendState = newBlendState;
    }

    void Renderer::setDepthState(const DepthState& newDepthState)
    {
        depthState = newDepthState;
    }

    bool Renderer::clear(Color color, float depth)
    {
        uint32_t* frameBufferData = reinterpret_cast<uint32_t*>(frameBuffer.getData().data());
        float* depthBufferData = reinterpret_cast<float*>(depthBuffer.getData().data());
        uint32_t rgba = color.getIntValueRaw();

        uint32_t frameBufferSize = frameBuffer.getWidth() * frameBuffer.getHeight();

        for (uint32_t p = 0; p < frameBufferSize; ++p)
            frameBufferData[p] = rgba;

        uint32_t depthBufferSize = depthBuffer.getWidth() * depthBuffer.getHeight();

        for (uint32_t p = 0; p < depthBufferSize; ++p)
            depthBufferData[p] = depth;

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

            boundingBox.min.x = clamp(boundingBox.min.x, 0.0F, static_cast<float>(frameBuffer.getWidth() - 1) * scissorRect.position.x);
            boundingBox.max.x = clamp(boundingBox.max.x, 0.0F, static_cast<float>(frameBuffer.getWidth() - 1) * (scissorRect.position.x + scissorRect.size.width));
            boundingBox.min.y = clamp(boundingBox.min.y, 0.0F, static_cast<float>(frameBuffer.getHeight() - 1) * scissorRect.position.y);
            boundingBox.max.y = clamp(boundingBox.max.y, 0.0F, static_cast<float>(frameBuffer.getHeight() - 1) * (scissorRect.position.y + scissorRect.size.height));

            for (uint32_t screenY = static_cast<uint32_t>(boundingBox.min.y); screenY <= static_cast<uint32_t>(boundingBox.max.y); ++screenY)
            {
                for (uint32_t screenX = static_cast<uint32_t>(boundingBox.min.x); screenX <= static_cast<uint32_t>(boundingBox.max.x); ++screenX)
                {
                    Vector3 s = barycentric(viewportPositions[0],
                                            viewportPositions[1],
                                            viewportPositions[2],
                                            Vector2(static_cast<float>(screenX),
                                                    static_cast<float>(screenY)));

                    if (s.x >= 0.0F && s.y >= 0.0F && s.z >= 0.0F)
                    {
                        Vector3 clip = Vector3(s.x / vsOutputs[0].position.w, s.y / vsOutputs[1].position.w, s.z / vsOutputs[2].position.w);
                        clip /= (clip.x + clip.y + clip.z);

                        float depth = vsOutputs[0].position.z * clip.x + vsOutputs[1].position.z * clip.y + vsOutputs[2].position.z * clip.z;

                        if (depthState.read && depthBufferData[screenY * depthBuffer.getWidth() + screenX] < depth)
                            continue; // discard the pixel

                        if (depthState.write) depthBufferData[screenY * depthBuffer.getWidth() + screenX] = depth;

                        Shader::VSOutput psInput;
                        psInput.position = clip;

                        psInput.color.r = vsOutputs[0].color.r * clip.x + vsOutputs[1].color.r * clip.y + vsOutputs[2].color.r * clip.z;
                        psInput.color.g = vsOutputs[0].color.g * clip.x + vsOutputs[1].color.g * clip.y + vsOutputs[2].color.g * clip.z;
                        psInput.color.b = vsOutputs[0].color.b * clip.x + vsOutputs[1].color.b * clip.y + vsOutputs[2].color.b * clip.z;
                        psInput.color.a = vsOutputs[0].color.a * clip.x + vsOutputs[1].color.a * clip.y + vsOutputs[2].color.a * clip.z;

                        psInput.texCoords[0] = Vector2(vsOutputs[0].texCoords[0].x * clip.x + vsOutputs[1].texCoords[0].x * clip.y + vsOutputs[2].texCoords[0].x * clip.z,
                                                       vsOutputs[0].texCoords[0].y * clip.x + vsOutputs[1].texCoords[0].y * clip.y + vsOutputs[2].texCoords[0].y * clip.z);

                        psInput.texCoords[1] = Vector2(vsOutputs[0].texCoords[1].x * clip.x + vsOutputs[1].texCoords[1].x * clip.y + vsOutputs[2].texCoords[1].x * clip.z,
                                                       vsOutputs[0].texCoords[1].y * clip.x + vsOutputs[1].texCoords[1].y * clip.y + vsOutputs[2].texCoords[1].y * clip.z);

                        psInput.normal = vsOutputs[0].normal * clip.x + vsOutputs[1].normal * clip.y + vsOutputs[2].normal * clip.z;

                        Color psOutput = shader->fragmentShader(psInput, samplers);

                        if (blendState.enabled)
                        {
                            uint8_t* pixel = reinterpret_cast<uint8_t*>(&frameBufferData[screenY * frameBuffer.getWidth() + screenX]);

                            Color destColor(pixel[0], pixel[1], pixel[2], pixel[3]);

                            // alpha blend
                            psOutput.r = psOutput.r * psOutput.a + destColor.r * (1.0F - psOutput.a);
                            psOutput.g = psOutput.g * psOutput.a + destColor.g * (1.0F - psOutput.a);
                            psOutput.b = psOutput.b * psOutput.a + destColor.b * (1.0F - psOutput.a);
                            psOutput.a = 1.0F;

                            frameBufferData[screenY * frameBuffer.getWidth() + screenX] = psOutput.getIntValueRaw();
                        }
                        else
                            frameBufferData[screenY * frameBuffer.getWidth() + screenX] = psOutput.getIntValueRaw();
                    }
                }
            }
        }

        return true;
    }
}
