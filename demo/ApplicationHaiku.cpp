//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include "ApplicationHaiku.hpp"

namespace demo
{
    WindowHaiku::WindowHaiku(Application& initApplication):
        Window(initApplication)
    {
        // TODO: implement
    }

    WindowHaiku::~WindowHaiku()
    {
        // TODO: implement
    }

    void WindowHaiku::draw()
    {
        render();

        // TODO: implement
    }

    void WindowHaiku::didResize(int newWidth, int newHeight)
    {
        width = static_cast<uint32_t>(newWidth);
        height = static_cast<uint32_t>(newHeight);

        onResize();
    }

    Application::Application()
    {
    }

    Application::~Application()
    {
    }

    void Application::run()
    {
        // TODO: implement
    }

    std::string Application::getResourcePath()
    {
        return "Resources";
    }
}

int main()
{
    try
    {
        demo::Application application;
        application.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
        return EXIT_FAILURE;
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }
}
