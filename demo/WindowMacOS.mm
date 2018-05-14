//
//  SoftwareRenderer
//

#include "WindowMacOS.hpp"
#include "Application.hpp"

@interface WindowDelegate: NSObject<NSWindowDelegate>
{
    WindowMacOS* window;
}

@end

@implementation WindowDelegate

-(id)initWithWindow:(WindowMacOS*)initWindow
{
    if (self = [super init])
    {
        window = initWindow;
    }

    return self;
}

-(void)windowDidResize:(NSNotification*)notification
{
    window->didResize();
}

@end

@interface Canvas: NSView
{
    NSInteger width;
    NSInteger height;
    size_t componentsPerPixel;
    size_t bitsPerComponent;
    CGColorSpaceRef colorSpace;
    CGDataProviderRef provider;
    sr::Buffer buffer;
}

@end

static const void* getBytePointer(void* info)
{
    sr::Buffer* buffer = (sr::Buffer*)info;

    return buffer->getData().data();
}

@implementation Canvas

-(id)initWithFrame:(NSRect)frameRect
{
    if (self = [super initWithFrame:frameRect])
    {
        width = frameRect.size.width;
        height = frameRect.size.height;
        componentsPerPixel = 4;
        bitsPerComponent = sizeof(uint8_t) * 8;

        CGDataProviderDirectCallbacks providerCallbacks = {
            0,
            getBytePointer,
            nullptr,
            nullptr,
            nullptr
        };

        colorSpace = CGColorSpaceCreateDeviceRGB();

        buffer.init(sr::Buffer::Type::RGBA,
                    static_cast<uint32_t>(width),
                    static_cast<uint32_t>(height));

        provider = CGDataProviderCreateDirect(&buffer, width * height * componentsPerPixel, &providerCallbacks);
    }

    return self;
}

-(void)dealloc
{
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);

    [super dealloc];
}

-(void)setFrameSize:(NSSize)newSize
{
    [super setFrameSize:newSize];

    CGDataProviderRelease(provider);

    width = newSize.width;
    height = newSize.height;

    buffer.resize(static_cast<uint32_t>(width),
                  static_cast<uint32_t>(height));

    CGDataProviderDirectCallbacks providerCallbacks = {
        0,
        getBytePointer,
        nullptr,
        nullptr,
        nullptr
    };
    
    provider = CGDataProviderCreateDirect(&buffer, width * height * componentsPerPixel, &providerCallbacks);
}

-(void)drawRect:(NSRect)dirtyRect
{
    [super drawRect:dirtyRect];

    CGImageRef image = CGImageCreate(width, height, bitsPerComponent,
                                     bitsPerComponent * componentsPerPixel,
                                     componentsPerPixel * width,
                                     colorSpace,
                                     kCGBitmapByteOrderDefault | kCGImageAlphaLast,
                                     provider, NULL, FALSE, kCGRenderingIntentDefault);

    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];

    CGContextDrawImage(context, CGRectMake(0, 0, width, height), image);
    CGContextFlush(context);

    CGImageRelease(image);
}

-(void)draw:(NSTimer*)timer
{
    NSValue* userInfo = timer.userInfo;

    Window* window = static_cast<Window*>(userInfo.pointerValue);

    buffer = window->render();

    [self setNeedsDisplay:YES];
}

@end

WindowMacOS::~WindowMacOS()
{
    [timer release];
    [content release];
    window.delegate = nil;
    [window release];
}

bool WindowMacOS::init(int argc, const char** argv)
{
    screen = [NSScreen mainScreen];

    CGSize windowSize;
    windowSize.width = round(screen.frame.size.width * 0.6);
    windowSize.height = round(screen.frame.size.height * 0.6);

    NSRect frame = NSMakeRect(round(screen.frame.size.width / 2.0f - windowSize.width / 2.0f),
                              round(screen.frame.size.height / 2.0f - windowSize.height / 2.0f),
                              windowSize.width, windowSize.height);

    NSWindowStyleMask windowStyleMask = NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;

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

    NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                 styleMask:[window styleMask]];

    width = static_cast<uint32_t>(windowFrame.size.width);
    height = static_cast<uint32_t>(windowFrame.size.height);

    content = [[Canvas alloc] initWithFrame:windowFrame];

    window.contentView = content;
    [window makeKeyAndOrderFront:nil];

    [content setNeedsDisplay:TRUE];

    timer = [[NSTimer scheduledTimerWithTimeInterval:0.016 target:content selector:@selector(draw:) userInfo:[NSValue valueWithPointer:this] repeats:YES] retain];

    return Window::init(argc, argv);
}

void WindowMacOS::didResize()
{
    NSRect windowFrame = [NSWindow contentRectForFrameRect:[window frame]
                                                 styleMask:[window styleMask]];

    width = static_cast<uint32_t>(windowFrame.size.width);
    height = static_cast<uint32_t>(windowFrame.size.height);

    onResize();
}
