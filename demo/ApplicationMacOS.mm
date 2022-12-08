//
//  SoftwareRenderer
//

#include <iostream>
#include <memory>
#include <stdexcept>
#include <CoreFoundation/CoreFoundation.h>
#include "ApplicationMacOS.hpp"

@interface AppDelegate: NSObject<NSApplicationDelegate>
{
    demo::Application* application;
}
@end

@implementation AppDelegate

-(id)initWithApplication:(demo::Application*)initApplication
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

@end

@interface WindowDelegate: NSObject<NSWindowDelegate>
{
    demo::ApplicationMacOS* application;
}

@end

@implementation WindowDelegate

-(id)initWithApplication:(demo::ApplicationMacOS*)initApplication
{
    if (self = [super init])
    {
        application = initApplication;
    }

    return self;
}

-(void)windowDidResize:(__unused NSNotification*)notification
{
    application->didResize();
}

@end

@interface Canvas: NSView
{
    demo::ApplicationMacOS* application;
}

@end

@implementation Canvas

-(id)initWithFrame:(NSRect)frameRect andApplication:(demo::ApplicationMacOS*)initApplication
{
    if (self = [super initWithFrame:frameRect])
    {
        application = initApplication;
    }

    return self;
}

-(void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];

    application->draw();
}

-(void)draw:(__unused NSTimer*)timer
{
    [self setNeedsDisplay:YES];
}

@end

static const void* getBytePointer(void* info)
{
    const auto frameBuffer = static_cast<sr::Texture*>(info);
    return frameBuffer->getData().data();
}

static void createMainMenu(NSApplication* sharedApplication)
{
    NSMenu* mainMenu = [[[NSMenu alloc] init] autorelease];

    // Apple menu
    NSMenuItem* mainMenuItem = [mainMenu addItemWithTitle:@""
                                                   action:nil
                                            keyEquivalent:@""];

    NSMenu* applicationMenu = [[[NSMenu alloc] init] autorelease];
    mainMenuItem.submenu = applicationMenu;

    NSString* bundleName = NSBundle.mainBundle.infoDictionary[@"CFBundleDisplayName"];
    if (!bundleName)
        bundleName = NSBundle.mainBundle.infoDictionary[@"CFBundleName"];

    [applicationMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"About", nil), bundleName]
                               action:@selector(orderFrontStandardAboutPanel:)
                        keyEquivalent:@""];

    [applicationMenu addItem:[NSMenuItem separatorItem]];

    NSMenuItem* servicesItem = [applicationMenu addItemWithTitle:NSLocalizedString(@"Services", nil)
                                                          action:nil
                                                   keyEquivalent:@""];

    NSMenu* servicesMenu = [[[NSMenu alloc] init] autorelease];
    servicesItem.submenu = servicesMenu;
    NSApp.servicesMenu = servicesMenu;

    [applicationMenu addItem:[NSMenuItem separatorItem]];

    [applicationMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"Hide", nil), bundleName]
                               action:@selector(hide:)
                        keyEquivalent:@"h"];

    NSMenuItem* hideOthersItem = [applicationMenu addItemWithTitle:NSLocalizedString(@"Hide Others", nil)
                                                            action:@selector(hideOtherApplications:)
                                                     keyEquivalent:@"h"];
    hideOthersItem.keyEquivalentModifierMask = NSEventModifierFlagOption | NSEventModifierFlagCommand;

    [applicationMenu addItemWithTitle:NSLocalizedString(@"Show All", nil)
                               action:@selector(unhideAllApplications:)
                        keyEquivalent:@""];

    [applicationMenu addItem:[NSMenuItem separatorItem]];

    [applicationMenu addItemWithTitle:[NSString stringWithFormat:@"%@ %@", NSLocalizedString(@"Quit", nil), bundleName]
                               action:@selector(terminate:)
                        keyEquivalent:@"q"];

    // View menu
    NSMenuItem* viewItem = [mainMenu addItemWithTitle:NSLocalizedString(@"View", nil)
                                               action:nil
                                        keyEquivalent:@""];

    NSMenu* viewMenu = [[[NSMenu alloc] initWithTitle:NSLocalizedString(@"View", nil)] autorelease];
    viewItem.submenu = viewMenu;

    // Window menu
    NSMenuItem* windowsItem = [mainMenu addItemWithTitle:NSLocalizedString(@"Window", nil)
                                                  action:nil
                                           keyEquivalent:@""];

    NSMenu* windowsMenu = [[[NSMenu alloc] initWithTitle:NSLocalizedString(@"Window", nil)] autorelease];
    [windowsMenu addItemWithTitle:NSLocalizedString(@"Minimize", nil) action:@selector(performMiniaturize:) keyEquivalent:@"m"];
    [windowsMenu addItemWithTitle:NSLocalizedString(@"Zoom", nil) action:@selector(performZoom:) keyEquivalent:@""];

    windowsItem.submenu = windowsMenu;
    sharedApplication.windowsMenu = windowsMenu;

    // Help menu
    NSMenuItem* helpItem = [mainMenu addItemWithTitle:NSLocalizedString(@"Help", nil)
                                               action:nil
                                        keyEquivalent:@""];

    NSMenu* helpMenu = [[[NSMenu alloc] initWithTitle:NSLocalizedString(@"Help", nil)] autorelease];
    helpItem.submenu = helpMenu;
    sharedApplication.helpMenu = helpMenu;

    sharedApplication.mainMenu = mainMenu;
}

namespace demo
{
    std::string getResourcePath()
    {
        CFBundleRef bundle = CFBundleGetMainBundle();
        if (!bundle) throw std::runtime_error{"Failed to get main bundle"};

        const cf::Pointer relativePath = CFBundleCopyResourcesDirectoryURL(bundle);
        if (!relativePath) throw std::runtime_error{"Failed to get current directory"};

        const cf::Pointer absolutePath = CFURLCopyAbsoluteURL(relativePath);
        if (!absolutePath) throw std::runtime_error{"Failed to copy absolute URL"};

        const cf::Pointer path = CFURLCopyFileSystemPath(absolutePath, kCFURLPOSIXPathStyle);
        if (!path) throw std::runtime_error{"Failed to copy file system path"};

        const auto maximumSize = CFStringGetMaximumSizeOfFileSystemRepresentation(path);
        const auto resourceDirectory = std::unique_ptr<char[]>(new char[static_cast<std::size_t>(maximumSize)]);
        if (!CFStringGetFileSystemRepresentation(path, resourceDirectory.get(), maximumSize))
            throw std::runtime_error{"Failed to get file system representation"};

        return std::string{resourceDirectory.get()};
    }

    ApplicationMacOS::ApplicationMacOS()
    {
        pool = [[NSAutoreleasePool alloc] init];

        NSApplication* sharedApplication = [NSApplication sharedApplication];
        [sharedApplication activateIgnoringOtherApps:YES];
        [sharedApplication setDelegate:[[[AppDelegate alloc] initWithApplication:this] autorelease]];
        createMainMenu(sharedApplication);

        // create window
        NSScreen* screen = [NSScreen mainScreen];

        const CGSize windowSize = CGSizeMake(std::round(screen.frame.size.width * 0.6),
                                             std::round(screen.frame.size.height * 0.6));

        const NSRect frame = NSMakeRect(std::round(screen.frame.size.width / 2.0F - windowSize.width / 2.0F),
                                        std::round(screen.frame.size.height / 2.0F - windowSize.height / 2.0F),
                                        windowSize.width, windowSize.height);

        const NSWindowStyleMask windowStyleMask = NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable;

        window  = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:windowStyleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO
                                                 screen:screen];
        [window setReleasedWhenClosed:NO];
        [window setTabbingMode:NSWindowTabbingModeDisallowed];

        [window setAcceptsMouseMovedEvents:YES];
        windowDelegate = [[WindowDelegate alloc] initWithApplication:this];
        [window setDelegate:windowDelegate];

        [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
        [window setTitle:@"SoftwareRenderer"];
        [window setReleasedWhenClosed:NO];

        const NSRect windowFrame = [window contentRectForFrameRect:[window frame]];

        const auto w = static_cast<std::size_t>(windowFrame.size.width);
        const auto h = static_cast<std::size_t>(windowFrame.size.height);

        content = [[Canvas alloc] initWithFrame:windowFrame andApplication:this];

        componentsPerPixel = 4;
        bitsPerComponent = sizeof(std::uint8_t) * 8;

        CGDataProviderDirectCallbacks providerCallbacks = {
            0,
            getBytePointer,
            nullptr,
            nullptr,
            nullptr
        };

        colorSpace = CGColorSpaceCreateDeviceRGB();
        
        provider = CGDataProviderCreateDirect(&getFrameBuffer(), w * h * componentsPerPixel, &providerCallbacks);

        [window setContentView:content];
        [window makeKeyAndOrderFront:nil];

        [content setNeedsDisplay:TRUE];

        timer = [[NSTimer scheduledTimerWithTimeInterval:1.0 / 60.0
                                                  target:content
                                                selector:@selector(draw:)
                                                userInfo:[NSValue valueWithPointer:this]
                                                 repeats:YES] retain];

        setup(w, h);
    }

    ApplicationMacOS::~ApplicationMacOS()
    {
        if (window)
            [window setDelegate:nil];
    }

    void ApplicationMacOS::draw()
    {
        render();

        const auto& frameBuffer = getFrameBuffer();

        cf::Pointer image = CGImageCreate(frameBuffer.getWidth(), frameBuffer.getHeight(),
                                          bitsPerComponent,
                                          bitsPerComponent * componentsPerPixel,
                                          componentsPerPixel * frameBuffer.getWidth(),
                                          colorSpace,
                                          kCGBitmapByteOrder32Big | kCGImageAlphaNoneSkipLast,
                                          provider, nullptr, FALSE, kCGRenderingIntentDefault);

        CGContextRef context = [[NSGraphicsContext currentContext] CGContext];

        CGContextDrawImage(context, [content frame], image);
        CGContextFlush(context);
    }

    void ApplicationMacOS::didResize()
    {
        const NSRect windowFrame = [window contentRectForFrameRect:[window frame]];

        const CGDataProviderDirectCallbacks providerCallbacks = {
            0,
            getBytePointer,
            nullptr,
            nullptr,
            nullptr
        };

        const auto w = static_cast<std::size_t>(windowFrame.size.width);
        const auto h = static_cast<std::size_t>(windowFrame.size.height);
        
        provider = CGDataProviderCreateDirect(&getFrameBuffer(),
                                              w * h * componentsPerPixel,
                                              &providerCallbacks);

        onResize(w, h);
    }

    void ApplicationMacOS::run()
    {
        NSApplication* sharedApplication = [NSApplication sharedApplication];
        [sharedApplication run];
    }
}

int main()
{
    try
    {
        demo::ApplicationMacOS application;
        application.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }
}
