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

        bool init(uint32_t initWidth, uint32_t initHeight)
        {
            if (!frameBuffer.init(Texture::PixelFormat::RGBA8, initWidth, initHeight)) return false;
            if (!depthBuffer.init(Texture::PixelFormat::FLOAT32, initWidth, initHeight)) return false;

            return true;
        }

        bool resize(uint32_t newWidth, uint32_t newHeight)
        {
            if (!frameBuffer.resize(newWidth, newHeight)) return false;
            if (!depthBuffer.resize(newWidth, newHeight)) return false;

            return true;
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
