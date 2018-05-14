//
//  SoftwareRenderer
//

#include "Window.hpp"
#include "Application.hpp"

Window::Window(Application& initApplication):
    application(initApplication)
{
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

    return true;
}

const sr::Buffer& Window::render()
{
    renderer.clear(sr::Color(255, 255, 255, 255), 1.0f);

    renderer.setTexture(texture);

    sr::Vertex vertices[3] = {
        sr::Vertex(sr::Vector3(10.0F, 10.0F, 0.0F), 0xFF0000FF, sr::Vector2(0.0F, 0.0F), sr::Vector3(0.0F, 0.0F, 1.0F)),
        sr::Vertex(sr::Vector3(10.0F, 100.0F, 0.0F), 0x00FF00FF, sr::Vector2(0.0F, 1.0F), sr::Vector3(0.0F, 0.0F, 1.0F)),
        sr::Vertex(sr::Vector3(100.0F, 10.0F, 0.0F), 0x0000FFFF, sr::Vector2(1.0F, 0.0F), sr::Vector3(0.0F, 0.0F, 1.0F))
    };

    renderer.drawTriangle(vertices);
    return renderer.getFrameBuffer();
}

void Window::onResize()
{
    renderer.resize(width, height);
}
