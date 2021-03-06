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

-(id)initWithWindow:(demo::ApplicationMacOS*)initApplication
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
    sr::RenderTarget* renderTarget = static_cast<sr::RenderTarget*>(info);

    return renderTarget->getFrameBuffer().getData().data();
}

namespace demo
{
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
        [quitItem setTarget: [sharedApplication delegate]];
        [subMenu addItem:quitItem];

        sharedApplication.mainMenu = mainMenu;

        // create window
        screen = [NSScreen mainScreen];

        CGSize windowSize;
        windowSize.width = round(screen.frame.size.width * 0.6);
        windowSize.height = round(screen.frame.size.height * 0.6);

        const NSRect frame = NSMakeRect(round(screen.frame.size.width / 2.0F - windowSize.width / 2.0F),
                                        round(screen.frame.size.height / 2.0F - windowSize.height / 2.0F),
                                        windowSize.width, windowSize.height);

        const NSWindowStyleMask windowStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

        window  = [[NSWindow alloc] initWithContentRect:frame
                                              styleMask:windowStyleMask
                                                backing:NSBackingStoreBuffered
                                                  defer:NO
                                                 screen:screen];
        [window setReleasedWhenClosed:NO];

        window.acceptsMouseMovedEvents = YES;
        windowDelegate = [[WindowDelegate alloc] initWithWindow:this];
        window.delegate = windowDelegate;

        [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
        [window setTitle:@"SoftwareRenderer"];

        const NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                           styleMask:[window styleMask]];

        width = static_cast<std::size_t>(windowFrame.size.width);
        height = static_cast<std::size_t>(windowFrame.size.height);

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
        provider = CGDataProviderCreateDirect(&renderTarget, width * height * componentsPerPixel, &providerCallbacks);

        window.contentView = content;
        [window makeKeyAndOrderFront:nil];

        [content setNeedsDisplay:TRUE];

        timer = [[NSTimer scheduledTimerWithTimeInterval:0.016
                                                  target:content selector:@selector(draw:)
                                                userInfo:[NSValue valueWithPointer:this] repeats:YES] retain];

        setup();
    }

    ApplicationMacOS::~ApplicationMacOS()
    {
        CGDataProviderRelease(provider);
        CGColorSpaceRelease(colorSpace);

        if (timer) [timer release];
        if (content) [content release];
        if (window)
        {
            window.delegate = nil;
            [window release];
        }
        if (pool) [pool release];
    }

    void ApplicationMacOS::draw()
    {
        render();

        CGImageRef image = CGImageCreate(width, height, bitsPerComponent,
                                         bitsPerComponent * componentsPerPixel,
                                         componentsPerPixel * width,
                                         colorSpace,
                                         kCGBitmapByteOrder32Big | kCGImageAlphaNoneSkipLast,
                                         provider, nullptr, FALSE, kCGRenderingIntentDefault);

        CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];

        CGContextDrawImage(context, [content frame], image);
        CGContextFlush(context);

        CGImageRelease(image);
    }

    void ApplicationMacOS::didResize()
    {
        const NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                           styleMask:[window styleMask]];

        width = static_cast<std::size_t>(windowFrame.size.width);
        height = static_cast<std::size_t>(windowFrame.size.height);

        CGDataProviderRelease(provider);

        CGDataProviderDirectCallbacks providerCallbacks = {
            0,
            getBytePointer,
            nullptr,
            nullptr,
            nullptr
        };

        provider = CGDataProviderCreateDirect(&renderTarget, width * height * componentsPerPixel, &providerCallbacks);

        onResize();
    }

    void ApplicationMacOS::run()
    {
        NSApplication* sharedApplication = [NSApplication sharedApplication];
        [sharedApplication run];
    }

    std::string Application::getResourcePath()
    {
        CFBundleRef bundle = CFBundleGetMainBundle();
        CFURLRef relativePath = CFBundleCopyResourcesDirectoryURL(bundle);

        if (relativePath)
        {
            CFURLRef absolutePath = CFURLCopyAbsoluteURL(relativePath);
            if (absolutePath)
            {
                CFStringRef path = CFURLCopyFileSystemPath(absolutePath, kCFURLPOSIXPathStyle);
                if (path)
                {
                    const auto maximumSize = CFStringGetMaximumSizeOfFileSystemRepresentation(path);
                    auto resourceDirectory = std::unique_ptr<char[]>(new char[static_cast<std::size_t>(maximumSize)]);
                    CFStringGetFileSystemRepresentation(path, resourceDirectory.get(), maximumSize);
                    CFRelease(path);
                    return std::string(resourceDirectory.get());
                }
                CFRelease(absolutePath);
            }
            CFRelease(relativePath);
            return std::string();
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
