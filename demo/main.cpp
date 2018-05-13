//
//  SoftwareRenderer
//

#include <functional>
#include <iostream>
#include <thread>
#include "sr.hpp"
#include "Application.hpp"

std::thread thread;
std::condition_variable startCondition;
std::mutex startMutex;
std::atomic<bool> done;
sr::Renderer renderer;

void srMain(Application& application)
{
    done = false;

    thread = std::thread([&application]() {
        while (!done)
        {
            renderer.draw();
        }
    });
}

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

    done = true;
    if (thread.joinable()) thread.join();

    return result;
}
