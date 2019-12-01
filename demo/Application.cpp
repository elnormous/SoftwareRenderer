//
//  SoftwareRenderer
//

#include "Application.hpp"

namespace demo
{
    Application::Application()
    {
        projection.setIdentity();
        view.setIdentity();
        model.setIdentity();

        sr::BMP bmp(Application::getResourcePath() + "/cube.bmp");
        texture = sr::Texture(sr::Texture::PixelFormat::Rgba8, bmp.getWidth(), bmp.getHeight());
        texture.setData(bmp.getData(), 0);
        texture.generateMipMaps();

        sampler.addressModeX = sr::Sampler::AddressMode::Repeat;
        sampler.addressModeY = sr::Sampler::AddressMode::Repeat;
        sampler.filter = sr::Sampler::Filter::Linear;

        blendState.colorBlendSource = sr::BlendState::Factor::SrcAlpha;
        blendState.colorBlendDest = sr::BlendState::Factor::InvSrcAlpha;
        blendState.colorOperation = sr::BlendState::Operation::Add;
        blendState.alphaBlendSource = sr::BlendState::Factor::One;
        blendState.alphaBlendDest = sr::BlendState::Factor::One;
        blendState.alphaOperation = sr::BlendState::Operation::Add;
        blendState.enabled = true;

        depthState.read = true;
        depthState.write = true;

        vertices = {
            // front
            sr::Vertex(sr::Vector4F(-20.0F, -20.0F, -20.0F, 1.0F), 0xFF0000FF, sr::Vector2F(0.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(-20.0F, 20.0F, -20.0F, 1.0F), 0x00FF00FF, sr::Vector2F(0.0F, 1.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, -20.0F, -20.0F, 1.0F), 0x0000FFFF, sr::Vector2F(1.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, 20.0F, -20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(1.0F, 1.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),

            // back
            sr::Vertex(sr::Vector4F(-20.0F, -20.0F, 20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(0.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(-20.0F, 20.0F, 20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(0.0F, 1.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, -20.0F, 20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(1.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, 20.0F, 20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(1.0F, 1.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),

            // left
            sr::Vertex(sr::Vector4F(-20.0F, -20.0F, -20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(0.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(-20.0F, 20.0F, -20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(0.0F, 4.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(-20.0F, -20.0F, 20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(4.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(-20.0F, 20.0F, 20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(4.0F, 4.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),

            // right
            sr::Vertex(sr::Vector4F(20.0F, -20.0F, -20.0F, 1.0F), 0xFFFFFFA0, sr::Vector2F(0.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, 20.0F, -20.0F, 1.0F), 0xFFFFFFA0, sr::Vector2F(0.0F, 1.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, -20.0F, 20.0F, 1.0F), 0xFFFFFFA0, sr::Vector2F(1.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, 20.0F, 20.0F, 1.0F), 0xFFFFFFA0, sr::Vector2F(1.0F, 1.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),

            // bottom
            sr::Vertex(sr::Vector4F(-20.0F, -20.0F, -20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(0.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(-20.0F, -20.0F, 20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(0.0F, 1.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, -20.0F, -20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(1.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, -20.0F, 20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(1.0F, 1.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),

            // top
            sr::Vertex(sr::Vector4F(-20.0F, 20.0F, -20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(0.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(-20.0F, 20.0F, 20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(0.0F, 1.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, 20.0F, -20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(1.0F, 0.0F), sr::Vector3F(0.0F, 0.0F, 1.0F)),
            sr::Vertex(sr::Vector4F(20.0F, 20.0F, 20.0F, 1.0F), 0xFFFFFFFF, sr::Vector2F(1.0F, 1.0F), sr::Vector3F(0.0F, 0.0F, 1.0F))
        };

        indices = {
            0, 1, 2, 1, 3, 2, // front
            4, 5, 6, 5, 7, 6, // back
            16, 17, 18, 17, 19, 18, // bottom
            20, 21, 22, 21, 23, 22, // top
            8, 9, 10, 9, 11, 10, // left
            12, 13, 14, 13, 15, 14 // right
        };
    }

    Application::~Application()
    {
    }

    void Application::setup()
    {
        renderTarget = sr::RenderTarget(width, height);

        projection.setPerspective(sr::tau / 6.0F, static_cast<float>(width) / static_cast<float>(height), 1.0F, 1000.0F);

        view.setTranslation(0.0F, 0.0F, 100.0F);
    }

    void Application::render()
    {
        renderer.setRenderTarget(&renderTarget);

        rotationY += 0.05F;
        model.setRotationY(rotationY);

        sr::Matrix4F modelViewProjection = projection * view * model;

        renderer.setViewport(sr::RectF(0.0F, 0.0F, static_cast<float>(width), static_cast<float>(height)));
        renderer.setBlendState(blendState);
        renderer.setDepthState(depthState);

        renderer.clear(sr::Color(255, 255, 255, 255), 1000.0F);

        renderer.setShader(shader);
        renderer.setTexture(texture, 0);
        renderer.setSampler(sampler, 0);

        renderer.drawTriangles(indices, vertices, modelViewProjection);
    }

    void Application::onResize()
    {
        renderTarget.resize(width, height);

        projection.setPerspective(sr::tau / 6.0F, static_cast<float>(width) / static_cast<float>(height), 1.0F, 1000.0F);
    }
}
