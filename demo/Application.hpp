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
        Application();
        ~Application();

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        void run();

        static std::string getResourcePath();

    private:
        std::unique_ptr<Window> window;
    };
}
