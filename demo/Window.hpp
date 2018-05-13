//
//  SoftwareRenderer
//

#pragma once

#include <memory>

class WindowImpl
{
};

class Window
{
public:
    std::unique_ptr<WindowImpl> impl;
};
