//
//  SoftwareRenderer
//

#include "Window.hpp"
#include "Application.hpp"

Window::Window(Application& initApplication):
    application(initApplication)
{
    projection.setIdentity();
    view.setIdentity();
    model.setIdentity();

    sr::BMP bmp;
    bmp.load(application.getResourcePath() + "/cube.bmp");
    texture = bmp.getBuffer();
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

const sr::Buffer& Window::render()
{
    rotationY += 0.01F;
    sr::Matrix4::createRotationY(rotationY, model);

    sr::Matrix4 modelViewProjection = projection * view * model;

    renderer.setViewport(sr::Rect(0.0F, 0.0F, static_cast<float>(width), static_cast<float>(height)));
    renderer.clear(sr::Color(255, 255, 255, 255), 1.0f);

    renderer.setTexture(texture);

    std::vector<sr::Vertex> vertices = {
        sr::Vertex(sr::Vector4(-20.0F, -20.0F, 0.0F, 1.0F), 0xFF0000FF, sr::Vector2(0.0F, 0.0F), sr::Vector3(0.0F, 0.0F, 1.0F)),
        sr::Vertex(sr::Vector4(-20.0F, 20.0F, 0.0F, 1.0F), 0x00FF00FF, sr::Vector2(0.0F, 1.0F), sr::Vector3(0.0F, 0.0F, 1.0F)),
        sr::Vertex(sr::Vector4(20.0F, -20.0F, 0.0F, 1.0F), 0x0000FFFF, sr::Vector2(1.0F, 0.0F), sr::Vector3(0.0F, 0.0F, 1.0F)),
        sr::Vertex(sr::Vector4(20.0F, 20.0F, 0.0F, 1.0F), 0xFFFFFFFF, sr::Vector2(1.0F, 1.0F), sr::Vector3(0.0F, 0.0F, 1.0F))
    };

    std::vector<uint32_t> indices = {0, 1, 2, 1, 2, 3};

    renderer.drawTriangles(indices, vertices, modelViewProjection);
    return renderer.getFrameBuffer();
}

void Window::onResize()
{
    renderer.resize(width, height);

    sr::Matrix4::createPerspective(sr::TAU / 6.0F, static_cast<float>(width) / static_cast<float>(height),
                                   1.0F, 1000.0F, projection);
}
