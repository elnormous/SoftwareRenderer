//
//  SoftwareRenderer
//

#pragma once

#include "Buffer.hpp"
#include "Color.hpp"
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
        void setTexture(const Buffer& newTexture);

        bool setViewport(const Rect& newViewport);
        bool clear(Color color, float depth);
        bool drawTriangles(const std::vector<uint32_t>& indices, const std::vector<Vertex>& vertices, const Matrix4& modelViewProjection);

        const Buffer& getFrameBuffer() const { return frameBuffer; }
        const Buffer& getDepthBuffer() const { return depthBuffer; }

    private:
        Buffer frameBuffer;
        Buffer depthBuffer;

        Rect viewport;
        const Shader* shader = nullptr;
        const Buffer* texture = nullptr;
    };
}
