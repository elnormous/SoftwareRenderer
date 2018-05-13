//
//  SoftwareRenderer
//

#pragma once

#include "Window.hpp"

class Application
{
public:
    ~Application();

    bool init(int argc, const char** argv);
    bool run();

    Window window;
    int argc;
    const char** argv;
};
