//
//  SoftwareRenderer
//

#include "Window.hpp"

Window::Window()
{
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
    renderer.clear(sr::Color(255, 0, 0, 255), 1.0f);

    sr::Vertex vertices[3] = {
        sr::Vertex(sr::Vector3(10.0F, 10.0F, 0.0F), 0x000000FF, sr::Vector2(0.0F, 0.0F), sr::Vector3(0.0F, 0.0F, 1.0F)),
        sr::Vertex(sr::Vector3(10.0F, 100.0F, 0.0F), 0x000000FF, sr::Vector2(0.0F, 1.0F), sr::Vector3(0.0F, 0.0F, 1.0F)),
        sr::Vertex(sr::Vector3(100.0F, 10.0F, 0.0F), 0x000000FF, sr::Vector2(1.0F, 0.0F), sr::Vector3(0.0F, 0.0F, 1.0F))
    };

    renderer.drawTriangle(vertices);
    return renderer.getFrameBuffer();
}

void Window::onResize()
{
    renderer.resize(width, height);
}
