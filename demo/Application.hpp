//
//  SoftwareRenderer
//

#pragma once

#include <memory>
#include "Window.hpp"

namespace demo
{
    class Application
    {
    public:
        Application(int initArgc, const char** initArgv);
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        bool run();

        static std::string getResourcePath();

    private:
        std::unique_ptr<Window> window;
        int argc;
        const char** argv;
    };
}
