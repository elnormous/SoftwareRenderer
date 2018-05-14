//
//  SoftwareRenderer
//

#include "Renderer.hpp"
#include "Box2.hpp"
#include "MathUtils.hpp"

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

        Vector2 positions[3] = {
            vertices[0].position,
            vertices[1].position,
            vertices[2].position
        };

        Box2 boundingBox;
        for (uint32_t i = 0; i < 3; ++i)
        {
            if (positions[i].x < boundingBox.min.x) boundingBox.min.x = positions[i].x;
            if (positions[i].x > boundingBox.max.x) boundingBox.max.x = positions[i].x;
            if (positions[i].y < boundingBox.min.y) boundingBox.min.y = positions[i].y;
            if (positions[i].y > boundingBox.max.y) boundingBox.max.y = positions[i].y;
        }

        boundingBox.min.x = clamp(boundingBox.min.x, 0.0F, static_cast<float>(frameBuffer.getWidth()));
        boundingBox.max.x = clamp(boundingBox.max.x, 0.0F, static_cast<float>(frameBuffer.getHeight()));
        boundingBox.min.y = clamp(boundingBox.min.y, 0.0F, static_cast<float>(frameBuffer.getWidth()));
        boundingBox.max.y = clamp(boundingBox.max.y, 0.0F, static_cast<float>(frameBuffer.getHeight()));

        for (uint32_t y = static_cast<uint32_t>(boundingBox.min.y); y < static_cast<uint32_t>(boundingBox.max.y); ++y)
        {
            for (uint32_t x = static_cast<uint32_t>(boundingBox.min.x); x < static_cast<uint32_t>(boundingBox.max.x); ++x)
            {
                Vector3 s = barycentric(positions[0],
                                        positions[1],
                                        positions[2],
                                        Vector2(x, y));

                if (s.x >= 0.0f && s.x <= 1.0f &&
                    s.y >= 0.0f && s.y <= 1.0f &&
                    s.z >= 0.0f && s.z <= 1.0f)
                {
                    frameBufferData[y * frameBuffer.getWidth() + x] = vertices[0].color.getIntValue();
                }
            }
        }

        return true;
    }
}
