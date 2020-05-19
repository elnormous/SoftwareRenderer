//
//  SoftwareRenderer
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <cstdint>
#include <memory>
#include "sr.hpp"
#include "TextureShader.hpp"

namespace demo
{
    class Application
    {
    public:
        Application();
        virtual ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        void setup();

        void render();

        static std::string getResourcePath();

    protected:
        void onResize();

        std::size_t width;
        std::size_t height;

        sr::Matrix4F projection;
        sr::Matrix4F view;
        sr::Matrix4F model;
        float rotationY = 0.0F;

        sr::RenderTarget renderTarget;
        sr::Renderer renderer;

        sr::BlendState blendState;
        sr::DepthState depthState;

        TextureShader shader;
        sr::Sampler sampler;
        sr::Texture texture;

        std::vector<std::uint32_t> indices;
        std::vector<sr::Vertex> vertices;
    };
}

#endif
