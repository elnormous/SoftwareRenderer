//
//  SoftwareRenderer
//

#ifndef RENDERER_H
#define RENDERER_H

#include <cassert>
#include <algorithm>
#include <stdexcept>
#include "BlendState.hpp"
#include "Color.hpp"
#include "DepthState.hpp"
#include "Matrix.hpp"
#include "Rect.hpp"
#include "RenderTarget.hpp"
#include "Sampler.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Vector.hpp"
#include "Vertex.hpp"

namespace sr
{
    class RenderError final: public std::runtime_error
    {
    public:
        explicit RenderError(const std::string& str): std::runtime_error(str) {}
        explicit RenderError(const char* str): std::runtime_error(str) {}
    };

    class Renderer final
    {
    public:
        Renderer() noexcept = default;

        RenderTarget* getRenderTarget() const noexcept
        {
            return renderTarget;
        }

        void setRenderTarget(RenderTarget* newRenderTarget) noexcept
        {
            renderTarget = newRenderTarget;
        }

        void setShader(const Shader& newShader) noexcept
        {
            shader = &newShader;
        }

        void setTexture(Texture& newTexture, std::uint32_t level)
        {
            assert(level < 2);

            textures[level] = &newTexture;
        }

        void setSampler(Sampler& newSampler, std::uint32_t level)
        {
            assert(level < 2);

            samplers[level] = &newSampler;
        }

        void setViewport(const RectF& newViewport) noexcept
        {
            viewport = newViewport;
        }

        void setScissorRect(const RectF& newScissorRect) noexcept
        {
            scissorRect = newScissorRect;
        }

        void setBlendState(const BlendState& newBlendState) noexcept
        {
            blendState = newBlendState;
        }

        void setDepthState(const DepthState& newDepthState) noexcept
        {
            depthState = newDepthState;
        }

        void clear(Color color, float depth)
        {
            if (!renderTarget)
                throw RenderError("No render target set");

            auto frameBufferData = reinterpret_cast<std::uint32_t*>(renderTarget->getFrameBuffer().getData().data());
            auto depthBufferData = reinterpret_cast<float*>(renderTarget->getDepthBuffer().getData().data());
            const auto rgba = color.getIntValueRaw();

            const auto frameBufferSize = renderTarget->getFrameBuffer().getWidth() * renderTarget->getFrameBuffer().getHeight();
            for (std::size_t p = 0; p < frameBufferSize; ++p)
                frameBufferData[p] = rgba;

            const auto depthBufferSize = renderTarget->getDepthBuffer().getWidth() * renderTarget->getDepthBuffer().getHeight();
            for (std::size_t p = 0; p < depthBufferSize; ++p)
                depthBufferData[p] = depth;
        }

        void drawTriangles(const std::vector<std::uint32_t>& indices, const std::vector<Vertex>& vertices, const Matrix4F& modelViewProjection)
        {
            if (!renderTarget)
                throw RenderError("No render target set");
            if (!shader)
                throw RenderError("No shader set");

            auto frameBufferData = reinterpret_cast<std::uint32_t*>(renderTarget->getFrameBuffer().getData().data());
            auto depthBufferData = reinterpret_cast<float*>(renderTarget->getDepthBuffer().getData().data());

            for (std::size_t i = 0; i + 2 < indices.size(); i += 3)
            {
                const Shader::VSOutput vsOutputs[3] = {
                    shader->vertexShader(modelViewProjection, vertices[indices[i + 0]]),
                    shader->vertexShader(modelViewProjection, vertices[indices[i + 1]]),
                    shader->vertexShader(modelViewProjection, vertices[indices[i + 2]])
                };

                Vector4F ndcPositions[3] = {
                    vsOutputs[0].position,
                    vsOutputs[1].position,
                    vsOutputs[2].position
                };

                // transform to normalized device coordinates
                for (sr::Vector4F& ndcPosition : ndcPositions)
                    ndcPosition /= ndcPosition.v[3];

                Vector2F viewportPositions[3] = {
                    Vector2F(ndcPositions[0].v[0], ndcPositions[0].v[1]),
                    Vector2F(ndcPositions[1].v[0], ndcPositions[1].v[1]),
                    Vector2F(ndcPositions[2].v[0], ndcPositions[2].v[1])
                };

                Vector2F screenMin;
                Vector2F screenMax;

                for (sr::Vector2F& viewportPosition : viewportPositions)
                {
                    // transform to viewport coordinates
                    viewportPosition.v[0] = viewportPosition.v[0] * viewport.size.v[0] / 2.0F + viewport.position.v[0] + viewport.size.v[0] / 2.0F; // xndc * width / 2 + x + width / 2
                    viewportPosition.v[1] = viewportPosition.v[1] * viewport.size.v[1] / 2.0F + viewport.position.v[1] + viewport.size.v[1] / 2.0F;  // yndc * height / 2 + y + height / 2
                    //viewportPosition.v[2] = viewportPosition.v[2] * (1.0F - 0.0F) / 2.0F + (1.0F + 0.0F) / 2.0F; // zndc * (far - near) / 2 + (far + near) / 2

                    if (viewportPosition.v[0] < screenMin.v[0]) screenMin.v[0] = viewportPosition.v[0];
                    if (viewportPosition.v[0] > screenMax.v[0]) screenMax.v[0] = viewportPosition.v[0];
                    if (viewportPosition.v[1] < screenMin.v[1]) screenMin.v[1] = viewportPosition.v[1];
                    if (viewportPosition.v[1] > screenMax.v[1]) screenMax.v[1] = viewportPosition.v[1];
                }

                screenMin.v[0] = clamp(screenMin.v[0], 0.0F, static_cast<float>(renderTarget->getFrameBuffer().getWidth() - 1) * scissorRect.position.v[0]);
                screenMax.v[0] = clamp(screenMax.v[0], 0.0F, static_cast<float>(renderTarget->getFrameBuffer().getWidth() - 1) * (scissorRect.position.v[0] + scissorRect.size.v[0]));
                screenMin.v[1] = clamp(screenMin.v[1], 0.0F, static_cast<float>(renderTarget->getFrameBuffer().getHeight() - 1) * scissorRect.position.v[1]);
                screenMax.v[1] = clamp(screenMax.v[1], 0.0F, static_cast<float>(renderTarget->getFrameBuffer().getHeight() - 1) * (scissorRect.position.v[1] + scissorRect.size.v[1]));

                for (std::size_t screenY = static_cast<std::size_t>(screenMin.v[1]); screenY <= static_cast<std::size_t>(screenMax.v[1]); ++screenY)
                    for (std::size_t screenX = static_cast<std::size_t>(screenMin.v[0]); screenX <= static_cast<std::size_t>(screenMax.v[0]); ++screenX)
                    {
                        const auto s = barycentric(viewportPositions[0],
                                                   viewportPositions[1],
                                                   viewportPositions[2],
                                                   Vector2F(static_cast<float>(screenX),
                                                            static_cast<float>(screenY)));

                        if (s.v[0] >= 0.0F && s.v[1] >= 0.0F && s.v[2] >= 0.0F)
                        {
                            Vector3F clip = Vector3F(s.v[0] / vsOutputs[0].position.v[3],
                                                     s.v[1] / vsOutputs[1].position.v[3],
                                                     s.v[2] / vsOutputs[2].position.v[3]);
                            clip /= (clip.v[0] + clip.v[1] + clip.v[2]);

                            const auto depth = ndcPositions[0].v[2] * clip.v[0] + ndcPositions[1].v[2] * clip.v[1] + ndcPositions[2].v[2] * clip.v[2];

                            if (depthState.read && depthBufferData[screenY * renderTarget->getDepthBuffer().getWidth() + screenX] < depth)
                                continue; // discard the pixel

                            if (depthState.write)
                                depthBufferData[screenY * renderTarget->getDepthBuffer().getWidth() + screenX] = depth;

                            Shader::VSOutput psInput;
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

                            const auto srcColor = shader->fragmentShader(psInput, samplers, textures);

                            if (blendState.enabled)
                            {
                                const auto pixel = reinterpret_cast<std::uint8_t*>(&frameBufferData[screenY * renderTarget->getFrameBuffer().getWidth() + screenX]);
                                const Color destColor(pixel[0], pixel[1], pixel[2], pixel[3]);

                                // alpha blend
                                const Color resultColor{
                                    getValue(blendState.colorOperation,
                                             srcColor.r * getValue(blendState.colorBlendSource, srcColor.r, srcColor.a, destColor.r, destColor.a, blendState.blendFactor.r),
                                             destColor.r * getValue(blendState.colorBlendDest, srcColor.r, srcColor.a, destColor.r, destColor.a, blendState.blendFactor.r)),
                                    getValue(blendState.colorOperation,
                                             srcColor.g * getValue(blendState.colorBlendSource, srcColor.g, srcColor.a, destColor.g, destColor.a, blendState.blendFactor.g),
                                             destColor.g * getValue(blendState.colorBlendDest, srcColor.g, srcColor.a, destColor.g, destColor.a, blendState.blendFactor.g)),
                                    getValue(blendState.colorOperation,
                                             srcColor.b * getValue(blendState.colorBlendSource, srcColor.b, srcColor.a, destColor.b, destColor.a, blendState.blendFactor.b),
                                             destColor.b * getValue(blendState.colorBlendDest, srcColor.b, srcColor.a, destColor.b, destColor.a, blendState.blendFactor.b)),
                                    getValue(blendState.alphaOperation,
                                             srcColor.a * getValue(blendState.alphaBlendSource, srcColor.a, srcColor.a, destColor.a, destColor.a, blendState.blendFactor.a),
                                             destColor.a * getValue(blendState.alphaBlendDest, srcColor.a, srcColor.a, destColor.a, destColor.a, blendState.blendFactor.a))
                                };

                                frameBufferData[screenY * renderTarget->getFrameBuffer().getWidth() + screenX] = resultColor.getIntValueRaw();
                            }
                            else
                                frameBufferData[screenY * renderTarget->getFrameBuffer().getWidth() + screenX] = srcColor.getIntValueRaw();
                        }
                    }
            }
        }

    private:
        static float getValue(BlendState::Factor factor, float srcColor, float srcAlpha,
                              float destColor, float destAlpha, float blendFactor)
        {
            switch (factor)
            {
                case BlendState::Factor::zero: return 0.0F;
                case BlendState::Factor::one: return 1.0F;
                case BlendState::Factor::srcColor: return srcColor;
                case BlendState::Factor::invSrcColor: return 1.0F - srcColor;
                case BlendState::Factor::srcAlpha: return srcAlpha;
                case BlendState::Factor::invSrcAlpha: return 1.0F - srcAlpha;
                case BlendState::Factor::destAlpha: return destAlpha;
                case BlendState::Factor::invDestAlpha: return 1.0F - destAlpha;
                case BlendState::Factor::destColor: return destColor;
                case BlendState::Factor::invDestColor: return 1.0F - destColor;
                case BlendState::Factor::srcAlphaSat: return std::min(srcAlpha, 1.0F - destAlpha);
                case BlendState::Factor::blendFactor: return blendFactor;
                case BlendState::Factor::invBlendFactor: return 1.0F - blendFactor;
                default: throw RenderError("Invalid blend factor");
            }
        }

        static float getValue(BlendState::Operation operation, float a, float b)
        {
            switch (operation)
            {
                case BlendState::Operation::add: return clamp(a + b, 0.0F, 1.0F);
                case BlendState::Operation::subtract: return clamp(a - b, 0.0F, 1.0F);
                case BlendState::Operation::reverseSubtract: return clamp(b - a, 0.0F, 1.0F);
                case BlendState::Operation::min: return std::min(a, b);
                case BlendState::Operation::max: return std::max(a, b);
                default: throw RenderError("Invalid blend operation");
            }
        }

        RenderTarget* renderTarget = nullptr;
        RectF viewport;
        RectF scissorRect = RectF(0.0F, 0.0F, 1.0F, 1.0F);
        const Shader* shader = nullptr;
        Sampler* samplers[2]{nullptr, nullptr};
        Texture* textures[2]{nullptr, nullptr};
        BlendState blendState;
        DepthState depthState;
    };
}

#endif
