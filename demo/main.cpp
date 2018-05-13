//
//  SoftwareRenderer
//

#include <functional>
#include <iostream>
#include <thread>
#include "Renderer.hpp"
#include "Application.hpp"

std::thread thread;
std::condition_variable startCondition;
std::mutex startMutex;
std::atomic<bool> done;
Renderer renderer;

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
#else
int main(int argc, const char* argv[])
#endif
{
    Application application;
    int result;

    application.init(argc, argv);
    result = application.run() ? EXIT_SUCCESS : EXIT_FAILURE;

    done = true;
    if (thread.joinable()) thread.join();

    return result;
}
