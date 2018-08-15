//
//  SoftwareRenderer
//

#include <iostream>
#include <stdexcept>
#include <string>
#include "ApplicationAndroid.hpp"

namespace demo
{
    ApplicationAndroid::ApplicationAndroid()
    {
        setup();
    }

    ApplicationAndroid::~ApplicationAndroid()
    {
    }

    void ApplicationAndroid::draw()
    {
        render();
    }

    void ApplicationAndroid::didResize(float newWidth, float newHeight)
    {
        width = static_cast<uint32_t>(newWidth);
        height = static_cast<uint32_t>(newHeight);

        onResize();
    }

    void ApplicationAndroid::run()
    {
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
        demo::ApplicationAndroid application;
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
