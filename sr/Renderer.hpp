//
//  SoftwareRenderer
//

#pragma once

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
        Renderer();

        void setShader(const Shader& newShader);
        void setTexture(const Texture& newTexture, uint32_t level);
        void setAddressModeX(Sampler::AddressMode addressMode, uint32_t level)
        {
            samplers[level].setAddressModeX(addressMode);
        }

        void setAddressModeY(Sampler::AddressMode addressMode, uint32_t level)
        {
            samplers[level].setAddressModeY(addressMode);
        }

        void setViewport(const Rect& newViewport);
        void setScissorRect(const Rect& newScissorRect);
        void setBlendState(const BlendState& newBlendState);
        void setDepthState(const DepthState& newDepthState);

        bool clear(Color color, float depth);
        bool drawTriangles(const std::vector<uint32_t>& indices, const std::vector<Vertex>& vertices, const Matrix4& modelViewProjection);

        inline RenderTarget* getRenderTarget() const { return renderTarget; }
        inline void setRenderTarget(RenderTarget* newRenderTarget) { renderTarget = newRenderTarget; }

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
