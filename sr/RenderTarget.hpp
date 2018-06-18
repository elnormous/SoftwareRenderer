//
//  SoftwareRenderer
//

#pragma once
#include "Texture.hpp"

namespace sr
{
    class RenderTarget
    {
    public:
        RenderTarget(uint32_t width = 0, uint32_t height = 0):
            frameBuffer(Texture::PixelFormat::RGBA8, width, height),
            depthBuffer(Texture::PixelFormat::FLOAT32, width, height)
        {
        }

        void init(uint32_t initWidth, uint32_t initHeight)
        {
            frameBuffer.init(Texture::PixelFormat::RGBA8, initWidth, initHeight);
            depthBuffer.init(Texture::PixelFormat::FLOAT32, initWidth, initHeight);
        }

        void resize(uint32_t newWidth, uint32_t newHeight)
        {
            frameBuffer.resize(newWidth, newHeight);
            depthBuffer.resize(newWidth, newHeight);
        }

        Texture& getFrameBuffer() { return frameBuffer; }
        const Texture& getFrameBuffer() const { return frameBuffer; }
        Texture& getDepthBuffer() { return depthBuffer; }
        const Texture& getDepthBuffer() const { return depthBuffer; }

    private:
        Texture frameBuffer;
        Texture depthBuffer;
    };
}
