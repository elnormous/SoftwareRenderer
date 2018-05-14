//
//  SoftwareRenderer
//

#pragma once

#include <memory>
#include "Window.hpp"

class Application
{
public:
    Application(int initArgc, const char** initArgv);
    ~Application();

    bool init();
    bool run();

    std::string getResourcePath() const;

private:
    std::unique_ptr<Window> window;
    int argc;
    const char** argv;
};
