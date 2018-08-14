//
//  SoftwareRenderer
//

#include <stdexcept>
#include "ApplicationIOS.hpp"

namespace demo
{
    ApplicationIOS::ApplicationIOS()
    {
    }

    ApplicationIOS::~ApplicationIOS()
    {
    }

    void ApplicationIOS::draw()
    {
    }

    void ApplicationIOS::didResize()
    {
    }

    void ApplicationIOS::run()
    {
    }

    std::string Application::getResourcePath()
    {
        CFBundleRef bundle = CFBundleGetMainBundle(); // [NSBundle mainBundle]
        CFURLRef path = CFBundleCopyResourcesDirectoryURL(bundle); // [bundle resourceURL]

        if (path)
        {
            char resourceDirectory[1024];
            CFURLGetFileSystemRepresentation(path, TRUE, reinterpret_cast<UInt8*>(resourceDirectory), sizeof(resourceDirectory));
            CFRelease(path);
            return resourceDirectory;
        }
        else
            throw std::runtime_error("Failed to get current directory");

        return "";
    }
}

int main()
{
    try
    {
        NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

        demo::ApplicationIOS application;
        application.run();

        [pool release];

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
