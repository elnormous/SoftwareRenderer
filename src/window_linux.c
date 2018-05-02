//
//  SoftwareRenderer
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <X11/Xutil.h>
#include "window_linux.h"
#include "application.h"

int gpWindowInit(GPWindow* window, int argc, const char** argv)
{
    GPWindowLinux* windowLinux = malloc(sizeof(GPWindowLinux));
    memset(windowLinux, 0, sizeof(GPWindowLinux));
    window->opaque = windowLinux;

    windowLinux->display = XOpenDisplay(NULL);

    if (!windowLinux->display)
    {
        fprintf(stderr, "Failed to open display\n");
        return 0;
    }

    Screen* screen = XDefaultScreenOfDisplay(windowLinux->display);
    int screenIndex = XScreenNumberOfScreen(screen);

    unsigned int width = XWidthOfScreen(screen) * 0.6f;
    unsigned int height = XHeightOfScreen(screen) * 0.6f;

    XSetWindowAttributes swa;
    swa.background_pixel = XWhitePixel(windowLinux->display, screenIndex);
    swa.border_pixel = 0;
    swa.event_mask = KeyPress;

    windowLinux->window = XCreateWindow(windowLinux->display,
        RootWindow(windowLinux->display, screenIndex),
        0, 0, width, height,
        0, DefaultDepth(windowLinux->display, screenIndex), InputOutput,
        DefaultVisual(windowLinux->display, screenIndex),
        CWBorderPixel | CWBackPixel | CWEventMask, &swa);

    XSetStandardProperties(windowLinux->display,
        windowLinux->window, "SoftwareRenderer", "SoftwareRenderer", None,
        argv, argc, NULL);

    XMapWindow(windowLinux->display, windowLinux->window);

    windowLinux->protocolsAtom = XInternAtom(windowLinux->display, "WM_PROTOCOLS", False);
    windowLinux->deleteAtom = XInternAtom(windowLinux->display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(windowLinux->display, windowLinux->window, &windowLinux->deleteAtom, 1);

    windowLinux->gc = XCreateGC(windowLinux->display, windowLinux->window, 0, 0);
    XSetForeground(windowLinux->display, windowLinux->gc, 0);

    windowLinux->font = XLoadQueryFont(windowLinux->display, "fixed");
    if (!windowLinux->font)
    {
        fprintf(stderr, "Failed to load font\n");
        return 0;
    }

    XSetFont(windowLinux->display, windowLinux->gc, windowLinux->font->fid);

    windowLinux->textSize = 1;
    windowLinux->text = malloc(windowLinux->textSize);
    windowLinux->text[0] = 0;

    return 1;
}

int gpWindowDestroy(GPWindow* window)
{
    if (window->opaque)
    {
        GPWindowLinux* windowLinux = (GPWindowLinux*)window->opaque;

        if (windowLinux->text) free(windowLinux->text);
        if (windowLinux->font) XFreeFont(windowLinux->display, windowLinux->font);
        if (windowLinux->gc) XFreeGC(windowLinux->display, windowLinux->gc);

        free(windowLinux);
    }

    return 1;
}
