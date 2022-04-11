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

-(void)handleQuit:(__unused id)sender
{
    [[NSApplication sharedApplication] terminate:nil];
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

        NSMenu* mainMenu = [[[NSMenu alloc] initWithTitle:@"Main Menu"] autorelease];

        NSMenuItem* mainMenuItem = [[[NSMenuItem alloc] init] autorelease];
        [mainMenu addItem:mainMenuItem];

        NSMenu* subMenu = [[[NSMenu alloc] init] autorelease];
        [mainMenuItem setSubmenu:subMenu];

        NSMenuItem* quitItem = [[[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(handleQuit:) keyEquivalent:@"q"] autorelease];
        [quitItem setTarget:[sharedApplication delegate]];
        [subMenu addItem:quitItem];

        sharedApplication.mainMenu = mainMenu;

        // create window
        NSScreen* screen = [NSScreen mainScreen];

        CGSize windowSize;
        windowSize.width = std::round(screen.frame.size.width * 0.6);
        windowSize.height = std::round(screen.frame.size.height * 0.6);

        const NSRect frame = NSMakeRect(std::round(screen.frame.size.width / 2.0F - windowSize.width / 2.0F),
                                        std::round(screen.frame.size.height / 2.0F - windowSize.height / 2.0F),
                                        windowSize.width, windowSize.height);

        const NSWindowStyleMask windowStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

        window  = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:windowStyleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO
                                                 screen:screen];
        [window setReleasedWhenClosed:NO];

        [window setAcceptsMouseMovedEvents:YES];
        windowDelegate = [[WindowDelegate alloc] initWithApplication:this];
        [window setDelegate:windowDelegate];

        [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
        [window setTitle:@"SoftwareRenderer"];

        const NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                           styleMask:[window styleMask]];

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

        CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];

        CGContextDrawImage(context, [content frame], image);
        CGContextFlush(context);
    }

    void ApplicationMacOS::didResize()
    {
        const NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                           styleMask:[window styleMask]];

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
