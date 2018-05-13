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

    bool Renderer::clear()
    {
        return true;
    }

    bool Renderer::draw()
    {
        return true;
    }
}
