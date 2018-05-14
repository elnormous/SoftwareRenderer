//
//  SoftwareRenderer
//

#include "Renderer.hpp"

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
        uint32_t rgba = color.getIntValue();

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

        for (uint32_t i = 0; i < 3; ++i)
        {
            uint32_t x = static_cast<uint32_t>(vertices[i].position.x);
            uint32_t y = static_cast<uint32_t>(vertices[i].position.y);

            frameBufferData[y * frameBuffer.getWidth() + x] = vertices[i].color.getIntValue();
        }

        return true;
    }
}
