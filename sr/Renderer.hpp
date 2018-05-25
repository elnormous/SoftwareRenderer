//
//  SoftwareRenderer
//

#pragma once

#include <cassert>
#include "BlendState.hpp"
#include "Color.hpp"
#include "DepthState.hpp"
#include "Matrix4.hpp"
#include "Rect.hpp"
#include "RenderTarget.hpp"
#include "Sampler.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vector.hpp"
#include "Vertex.hpp"

namespace sr
{
    class Renderer
    {
    public:
        Renderer() {}

        inline RenderTarget* getRenderTarget() const
        {
            return renderTarget;
        }

        inline void setRenderTarget(RenderTarget* newRenderTarget)
        {
            renderTarget = newRenderTarget;
        }

        void setShader(const Shader& newShader)
        {
            shader = &newShader;
        }

        void setTexture(const Texture& newTexture, uint32_t level)
        {
            assert(level < 2);

            samplers[level] = Sampler(&newTexture);
        }

        void setAddressModeX(Sampler::AddressMode addressMode, uint32_t level)
        {
            samplers[level].setAddressModeX(addressMode);
        }

        void setAddressModeY(Sampler::AddressMode addressMode, uint32_t level)
        {
            samplers[level].setAddressModeY(addressMode);
        }

        void setViewport(const Rect& newViewport)
        {
            viewport = newViewport;
        }

        void setScissorRect(const Rect& newScissorRect)
        {
            scissorRect = newScissorRect;
        }

        void setBlendState(const BlendState& newBlendState)
        {
            blendState = newBlendState;
        }

        void setDepthState(const DepthState& newDepthState)
        {
            depthState = newDepthState;
        }

        bool clear(Color color, float depth)
        {
            if (!renderTarget) return false;

            uint32_t* frameBufferData = reinterpret_cast<uint32_t*>(renderTarget->getFrameBuffer().getData().data());
            float* depthBufferData = reinterpret_cast<float*>(renderTarget->getDepthBuffer().getData().data());
            uint32_t rgba = color.getIntValueRaw();

            uint32_t frameBufferSize = renderTarget->getFrameBuffer().getWidth() * renderTarget->getFrameBuffer().getHeight();

            for (uint32_t p = 0; p < frameBufferSize; ++p)
                frameBufferData[p] = rgba;

            uint32_t depthBufferSize = renderTarget->getDepthBuffer().getWidth() * renderTarget->getDepthBuffer().getHeight();

            for (uint32_t p = 0; p < depthBufferSize; ++p)
                depthBufferData[p] = depth;

            return true;
        }

        bool drawTriangles(const std::vector<uint32_t>& indices, const std::vector<Vertex>& vertices, const Matrix4& modelViewProjection)
        {
            if (!renderTarget) return false;
            if (!shader) return false;

            uint32_t* frameBufferData = reinterpret_cast<uint32_t*>(renderTarget->getFrameBuffer().getData().data());
            float* depthBufferData = reinterpret_cast<float*>(renderTarget->getDepthBuffer().getData().data());

            for (uint32_t i = 0; i < indices.size(); i += 3)
            {
                Shader::VSOutput vsOutputs[3];

                // vertex shader step
                for (uint32_t c = 0; c < 3; ++c)
                {
                    uint32_t index = indices[i + c];
                    vsOutputs[c] = shader->vertexShader(modelViewProjection, vertices[index]);
                }

                Vector4F ndcPositions[3] = {
                    vsOutputs[0].position,
                    vsOutputs[1].position,
                    vsOutputs[2].position
                };

                for (sr::Vector4F& ndcPosition : ndcPositions)
                {
                    // transform to normalized device coordinates
                    ndcPosition /= ndcPosition.v[3];
                }

                Vector2F viewportPositions[3] = {
                    Vector2F(ndcPositions[0].v[0], ndcPositions[0].v[1]),
                    Vector2F(ndcPositions[1].v[0], ndcPositions[1].v[1]),
                    Vector2F(ndcPositions[2].v[0], ndcPositions[2].v[1])
                };

                Box2 boundingBox;
                for (sr::Vector2F& viewportPosition : viewportPositions)
                {
                    // transform to viewport coordinates
                    viewportPosition.v[0] = viewportPosition.v[0] * viewport.size.width / 2.0F + viewport.position.v[0] + viewport.size.width / 2.0F; // xndc * width / 2 + x + width / 2
                    viewportPosition.v[1] = viewportPosition.v[1] * viewport.size.height / 2.0F + viewport.position.v[1] + viewport.size.height / 2.0F;  // yndc * height / 2 + y + height / 2
                    //viewportPosition.v[2] = viewportPosition.v[2] * (1.0F - 0.0F) / 2.0F + (1.0F + 0.0F) / 2.0F; // zndc * (far - near) / 2 + (far + near) / 2

                    if (viewportPosition.v[0] < boundingBox.min.v[0]) boundingBox.min.v[0] = viewportPosition.v[0];
                    if (viewportPosition.v[0] > boundingBox.max.v[0]) boundingBox.max.v[0] = viewportPosition.v[0];
                    if (viewportPosition.v[1] < boundingBox.min.v[1]) boundingBox.min.v[1] = viewportPosition.v[1];
                    if (viewportPosition.v[1] > boundingBox.max.v[1]) boundingBox.max.v[1] = viewportPosition.v[1];
                }

                boundingBox.min.v[0] = clamp(boundingBox.min.v[0], 0.0F, static_cast<float>(renderTarget->getFrameBuffer().getWidth() - 1) * scissorRect.position.v[0]);
                boundingBox.max.v[0] = clamp(boundingBox.max.v[0], 0.0F, static_cast<float>(renderTarget->getFrameBuffer().getWidth() - 1) * (scissorRect.position.v[0] + scissorRect.size.width));
                boundingBox.min.v[1] = clamp(boundingBox.min.v[1], 0.0F, static_cast<float>(renderTarget->getFrameBuffer().getHeight() - 1) * scissorRect.position.v[1]);
                boundingBox.max.v[1] = clamp(boundingBox.max.v[1], 0.0F, static_cast<float>(renderTarget->getFrameBuffer().getHeight() - 1) * (scissorRect.position.v[1] + scissorRect.size.height));

                for (uint32_t screenY = static_cast<uint32_t>(boundingBox.min.v[1]); screenY <= static_cast<uint32_t>(boundingBox.max.v[1]); ++screenY)
                {
                    for (uint32_t screenX = static_cast<uint32_t>(boundingBox.min.v[0]); screenX <= static_cast<uint32_t>(boundingBox.max.v[0]); ++screenX)
                    {
                        Vector3F s = barycentric(viewportPositions[0],
                                                 viewportPositions[1],
                                                 viewportPositions[2],
                                                 Vector2F(static_cast<float>(screenX),
                                                          static_cast<float>(screenY)));

                        if (s.v[0] >= 0.0F && s.v[1] >= 0.0F && s.v[2] >= 0.0F)
                        {
                            Vector3F clip = Vector3F(s.v[0] / vsOutputs[0].position.v[3], s.v[1] / vsOutputs[1].position.v[3], s.v[2] / vsOutputs[2].position.v[3]);
                            clip /= (clip.v[0] + clip.v[1] + clip.v[2]);

                            float depth = ndcPositions[0].v[2] * clip.v[0] + ndcPositions[1].v[2] * clip.v[1] + ndcPositions[2].v[2] * clip.v[2];

                            if (depthState.read && depthBufferData[screenY * renderTarget->getDepthBuffer().getWidth() + screenX] < depth)
                                continue; // discard the pixel

                            if (depthState.write) depthBufferData[screenY * renderTarget->getDepthBuffer().getWidth() + screenX] = depth;

                            Shader::VSOutput psInput;
                            //psInput.position = clip;
                            psInput.position = Vector4F(clip.v[0], clip.v[1], clip.v[2], 1.0F);

                            psInput.color.r = vsOutputs[0].color.r * clip.v[0] + vsOutputs[1].color.r * clip.v[1] + vsOutputs[2].color.r * clip.v[2];
                            psInput.color.g = vsOutputs[0].color.g * clip.v[0] + vsOutputs[1].color.g * clip.v[1] + vsOutputs[2].color.g * clip.v[2];
                            psInput.color.b = vsOutputs[0].color.b * clip.v[0] + vsOutputs[1].color.b * clip.v[1] + vsOutputs[2].color.b * clip.v[2];
                            psInput.color.a = vsOutputs[0].color.a * clip.v[0] + vsOutputs[1].color.a * clip.v[1] + vsOutputs[2].color.a * clip.v[2];

                            psInput.texCoords[0] = Vector2F(vsOutputs[0].texCoords[0].v[0] * clip.v[0] + vsOutputs[1].texCoords[0].v[0] * clip.v[1] + vsOutputs[2].texCoords[0].v[0] * clip.v[2],
                                                            vsOutputs[0].texCoords[0].v[1] * clip.v[0] + vsOutputs[1].texCoords[0].v[1] * clip.v[1] + vsOutputs[2].texCoords[0].v[1] * clip.v[2]);

                            psInput.texCoords[1] = Vector2F(vsOutputs[0].texCoords[1].v[0] * clip.v[0] + vsOutputs[1].texCoords[1].v[0] * clip.v[1] + vsOutputs[2].texCoords[1].v[0] * clip.v[2],
                                                            vsOutputs[0].texCoords[1].v[1] * clip.v[0] + vsOutputs[1].texCoords[1].v[1] * clip.v[1] + vsOutputs[2].texCoords[1].v[1] * clip.v[2]);

                            psInput.normal = vsOutputs[0].normal * clip.v[0] + vsOutputs[1].normal * clip.v[1] + vsOutputs[2].normal * clip.v[2];

                            Color psOutput = shader->fragmentShader(psInput, samplers);

                            if (blendState.enabled)
                            {
                                uint8_t* pixel = reinterpret_cast<uint8_t*>(&frameBufferData[screenY * renderTarget->getFrameBuffer().getWidth() + screenX]);

                                Color destColor(pixel[0], pixel[1], pixel[2], pixel[3]);

                                // alpha blend
                                psOutput.r = psOutput.r * psOutput.a + destColor.r * (1.0F - psOutput.a);
                                psOutput.g = psOutput.g * psOutput.a + destColor.g * (1.0F - psOutput.a);
                                psOutput.b = psOutput.b * psOutput.a + destColor.b * (1.0F - psOutput.a);
                                psOutput.a = 1.0F;

                                frameBufferData[screenY * renderTarget->getFrameBuffer().getWidth() + screenX] = psOutput.getIntValueRaw();
                            }
                            else
                                frameBufferData[screenY * renderTarget->getFrameBuffer().getWidth() + screenX] = psOutput.getIntValueRaw();
                        }
                    }
                }
            }

            return true;
        }

    private:
        RenderTarget* renderTarget = nullptr;
        Rect viewport;
        Rect scissorRect = Rect(0.0F, 0.0F, 1.0F, 1.0F);
        const Shader* shader = nullptr;
        Sampler samplers[2];
        BlendState blendState;
        DepthState depthState;
    };
}
