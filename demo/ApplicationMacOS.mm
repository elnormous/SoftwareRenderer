//
//  SoftwareRenderer
//

#include <iostream>
#include "Application.hpp"
#include "WindowMacOS.hpp"

@interface AppDelegate: NSObject<NSApplicationDelegate>
{
    Application* application;
}
@end

@implementation AppDelegate

-(id)initWithApplication:(Application*)initApplication
{
    if (self = [super init])
        application = initApplication;

    return self;
}

-(void)applicationWillFinishLaunching:(__unused NSNotification*)notification
{
}

-(void)applicationDidFinishLaunching:(__unused NSNotification*)notification
{
}

-(void)applicationWillTerminate:(__unused NSNotification*)notification
{
}

-(BOOL)applicationShouldTerminateAfterLastWindowClosed:(__unused NSApplication*)sender
{
    return YES;
}

-(void)applicationDidBecomeActive:(__unused NSNotification*)notification
{
}

-(void)applicationDidResignActive:(__unused NSNotification*)notification
{
}

-(void)handleQuit:(__unused id)sender
{
    [[NSApplication sharedApplication] terminate:nil];
}

@end

Application::Application(int initArgc, const char** initArgv):
    argc(initArgc),
    argv(initArgv)
{
}

Application::~Application()
{
}

bool Application::init()
{
    return true;
}

bool Application::run()
{
    NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

    NSApplication* sharedApplication = [NSApplication sharedApplication];
    [sharedApplication activateIgnoringOtherApps:YES];
    [sharedApplication setDelegate:[[[AppDelegate alloc] initWithApplication:this] autorelease]];

    NSMenu* mainMenu = [[[NSMenu alloc] initWithTitle:@"Main Menu"] autorelease];

    NSMenuItem* mainMenuItem = [[[NSMenuItem alloc] init] autorelease];
    [mainMenu addItem:mainMenuItem];

    NSMenu* subMenu = [[[NSMenu alloc] init] autorelease];
    [mainMenuItem setSubmenu:subMenu];

    NSMenuItem* quitItem = [[[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(handleQuit:) keyEquivalent:@"q"] autorelease];
    [quitItem setTarget: [sharedApplication delegate]];
    [subMenu addItem:quitItem];

    sharedApplication.mainMenu = mainMenu;

    WindowMacOS* windowMacOS = new WindowMacOS(*this);
    window.reset(windowMacOS);
    windowMacOS->init(argc, argv);

    [sharedApplication run];

    [pool release];

    return true;
}

std::string Application::getResourcePath() const
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
        std::cerr << "Failed to get current directory" << std::endl;

    return "";
}
