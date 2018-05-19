//
//  SoftwareRenderer
//

#pragma once

#include "BlendState.hpp"
#include "Buffer.hpp"
#include "Color.hpp"
#include "DepthState.hpp"
#include "Matrix4.hpp"
#include "Rect.hpp"
#include "Shader.hpp"
#include "Vertex.hpp"

namespace sr
{
    class Renderer
    {
    public:
        Renderer();

        bool init(uint32_t width, uint32_t height);
        bool resize(uint32_t width, uint32_t height);

        void setShader(const Shader& newShader);
        void setTexture(const Buffer& newTexture, uint32_t level);
        void setViewport(const Rect& newViewport);
        void setScissorRect(const Rect& newScissorRect);
        void setBlendState(const BlendState& newBlendState);
        void setDepthState(const DepthState& newDepthState);

        bool clear(Color color, float depth);
        bool drawTriangles(const std::vector<uint32_t>& indices, const std::vector<Vertex>& vertices, const Matrix4& modelViewProjection);

        inline const Buffer& getFrameBuffer() const { return frameBuffer; }
        inline const Buffer& getDepthBuffer() const { return depthBuffer; }

    private:
        Buffer frameBuffer;
        Buffer depthBuffer;

        Rect viewport;
        Rect scissorRect = Rect(0.0F, 0.0F, 1.0F, 1.0F);
        const Shader* shader = nullptr;
        const Buffer* textures[2];
        BlendState blendState;
        DepthState depthState;
    };
}
