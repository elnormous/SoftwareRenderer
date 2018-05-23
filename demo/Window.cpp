//
//  SoftwareRenderer
//

#include "Window.hpp"
#include "Application.hpp"

namespace demo
{
    Window::Window(Application& initApplication):
        application(initApplication)
    {
        projection.setIdentity();
        view.setIdentity();
        model.setIdentity();

        sr::BMP bmp;
        bmp.load(application.getResourcePath() + "/cube.bmp");
        texture.setLevel(bmp.getBuffer(), 0);

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

    Window::~Window()
    {
    }

    bool Window::init(int argc, const char** argv)
    {
        renderer.init(width, height);

        sr::Matrix4::createPerspective(sr::TAU / 6.0F, static_cast<float>(width) / static_cast<float>(height),
                                       1.0F, 1000.0F, projection);

        view.translate(0.0F, 0.0F, 100.0F);

        return true;
    }

    void Window::render()
    {
        rotationY += 0.05F;
        sr::Matrix4::createRotationY(rotationY, model);

        sr::Matrix4 modelViewProjection = projection * view * model;

        renderer.setViewport(sr::Rect(0.0F, 0.0F, static_cast<float>(width), static_cast<float>(height)));
        renderer.setBlendState(blendState);
        renderer.setDepthState(depthState);

        renderer.clear(sr::Color(255, 255, 255, 255), 1000.0F);

        renderer.setShader(shader);
        renderer.setTexture(texture, 0);
        renderer.setAddressModeX(sr::Sampler::AddressMode::REPEAT, 0);
        renderer.setAddressModeY(sr::Sampler::AddressMode::REPEAT, 0);

        renderer.drawTriangles(indices, vertices, modelViewProjection);
    }

    void Window::onResize()
    {
        renderer.resize(width, height);

        sr::Matrix4::createPerspective(sr::TAU / 6.0F, static_cast<float>(width) / static_cast<float>(height),
                                       1.0F, 1000.0F, projection);
    }
}
