//
//  SoftwareRenderer
//

#ifndef RENDERTARGET_H
#define RENDERTARGET_H

#include "Texture.hpp"

namespace sr
{
    class RenderTarget final
    {
    public:
        RenderTarget(uint32_t width = 0, uint32_t height = 0):
            frameBuffer(Texture::PixelFormat::Rgba8, width, height),
            depthBuffer(Texture::PixelFormat::Float32, width, height)
        {
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

#endif
