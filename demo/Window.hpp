//
//  SoftwareRenderer
//

#pragma once

#include <cstdint>
#include "sr.hpp"

class Application;

class Window
{
public:
    Window(Application& initApplication);
    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
    Window(Window&&) = delete;
    Window& operator=(Window&&) = delete;
    virtual ~Window();

    virtual bool init(int argc, const char** argv);

    const sr::Buffer& render();

protected:
    void onResize();

    Application& application;
    uint32_t width;
    uint32_t height;

    sr::Renderer renderer;

    sr::Buffer texture;
};
