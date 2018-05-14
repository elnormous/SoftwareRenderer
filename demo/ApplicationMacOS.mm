//
//  SoftwareRenderer
//

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
    WindowMacOS* windowMacOS = new WindowMacOS();
    application->window.reset(windowMacOS);
    windowMacOS->init(application->argc, application->argv);
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

    [sharedApplication run];

    [pool release];

    return true;
}
