//
//  SoftwareRenderer
//

#pragma once

#include <cstdint>
#include "sr.hpp"
#include "TextureShader.hpp"

namespace demo
{
    class Application;

    class Window
    {
    public:
        Window(Application& initApplication);

        Window(const Window&) = delete;
        Window& operator=(const Window&) = delete;
        Window(Window&&) = delete;
        Window& operator=(Window&&) = delete;
        virtual ~Window();

        virtual bool init(int argc, const char** argv);

        void render();

    protected:
        void onResize();

        Application& application;
        uint32_t width;
        uint32_t height;

        sr::Matrix4 projection;
        sr::Matrix4 view;
        sr::Matrix4 model;
        float rotationY = 0.0F;

        sr::Renderer renderer;

        sr::BlendState blendState;
        sr::DepthState depthState;

        TextureShader shader;
        sr::Buffer texture;
    };
}
