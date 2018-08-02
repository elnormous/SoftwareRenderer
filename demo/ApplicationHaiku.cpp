//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include "ApplicationHaiku.hpp"

namespace demo
{
    ApplicationHaiku::ApplicationHaiku()
    {
        // TODO: implement
    }

    ApplicationHaiku::~ApplicationHaiku()
    {
        // TODO: implement
    }

    void ApplicationHaiku::draw()
    {
        render();

        // TODO: implement
    }

    void ApplicationHaiku::didResize(int newWidth, int newHeight)
    {
        width = static_cast<uint32_t>(newWidth);
        height = static_cast<uint32_t>(newHeight);

        onResize();
    }

    void ApplicationHaiku::run()
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
        demo::ApplicationHaiku application;
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
