//
//  SoftwareRenderer
//

#include <iostream>
#include "Application.hpp"

#ifdef WIN32
int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR cmdLine, int cmdShow)
{
    int argc;
    LPWSTR argv = CommandLineToArgvW(cmdLine, &argc);
#else
int main(int argc, const char* argv[])
{
#endif
    Application application(argc, argv);
    int result;

    application.init();
    result = application.run() ? EXIT_SUCCESS : EXIT_FAILURE;

    return result;
}
