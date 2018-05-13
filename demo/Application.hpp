//
//  SoftwareRenderer
//

#pragma once

#include "Window.hpp"

class Application
{
public:
    Application(int initArgc, const char** initArgv);
    ~Application();

    bool init();
    bool run();

    Window window;
    int argc;
    const char** argv;
};
