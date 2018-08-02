//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>

namespace demo
{
    WindowLinux::WindowLinux(Application& initApplication):
        Window(initApplication)
    {
        // TODO: implement
    }

    WindowLinux::~WindowLinux()
    {
        // TODO: implement
    }

    void WindowLinux::draw()
    {
        render();

        // TODO: implement
    }

    void WindowLinux::didResize(int newWidth, int newHeight)
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
