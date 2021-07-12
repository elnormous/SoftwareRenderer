//
//  SoftwareRenderer
//

#ifndef APPLICATION_H
#define APPLICATION_H

#include <cstdint>
#include <memory>
#include "sr.hpp"
#include "BMP.hpp"
#include "TextureShader.hpp"

namespace demo
{
    class Application
    {
    public:
        Application():
            indices{
                0, 1, 2, 1, 3, 2, // front
                4, 5, 6, 5, 7, 6, // back
                16, 17, 18, 17, 19, 18, // bottom
                20, 21, 22, 21, 23, 22, // top
                8, 9, 10, 9, 11, 10, // left
                12, 13, 14, 13, 15, 14 // right
            },
            vertices{
                // front
                sr::Vertex(sr::Vector<float, 4>(-20.0F, -20.0F, -20.0F, 1.0F), sr::Color(0xFF0000FFU), sr::Vector<float, 2>(0.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(-20.0F, 20.0F, -20.0F, 1.0F), sr::Color(0x00FF00FFU), sr::Vector<float, 2>(0.0F, 1.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, -20.0F, -20.0F, 1.0F), sr::Color(0x0000FFFFU), sr::Vector<float, 2>(1.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, 20.0F, -20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(1.0F, 1.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),

                // back
                sr::Vertex(sr::Vector<float, 4>(-20.0F, -20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(0.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(-20.0F, 20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(0.0F, 1.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, -20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(1.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, 20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(1.0F, 1.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),

                // left
                sr::Vertex(sr::Vector<float, 4>(-20.0F, -20.0F, -20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(0.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(-20.0F, 20.0F, -20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(0.0F, 4.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(-20.0F, -20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(4.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(-20.0F, 20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(4.0F, 4.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),

                // right
                sr::Vertex(sr::Vector<float, 4>(20.0F, -20.0F, -20.0F, 1.0F), sr::Color(0xFFFFFFA0U), sr::Vector<float, 2>(0.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, 20.0F, -20.0F, 1.0F), sr::Color(0xFFFFFFA0U), sr::Vector<float, 2>(0.0F, 1.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, -20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFA0U), sr::Vector<float, 2>(1.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, 20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFA0U), sr::Vector<float, 2>(1.0F, 1.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),

                // bottom
                sr::Vertex(sr::Vector<float, 4>(-20.0F, -20.0F, -20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(0.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(-20.0F, -20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(0.0F, 1.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, -20.0F, -20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(1.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, -20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(1.0F, 1.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),

                // top
                sr::Vertex(sr::Vector<float, 4>(-20.0F, 20.0F, -20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(0.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(-20.0F, 20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(0.0F, 1.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, 20.0F, -20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(1.0F, 0.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F)),
                sr::Vertex(sr::Vector<float, 4>(20.0F, 20.0F, 20.0F, 1.0F), sr::Color(0xFFFFFFFFU), sr::Vector<float, 2>(1.0F, 1.0F), sr::Vector<float, 3>(0.0F, 0.0F, 1.0F))
            }
        {
            const sr::BMP bmp{Application::getResourcePath() + "/cube.bmp"};
            texture = sr::Texture{sr::Texture::PixelFormat::rgba8, bmp.getWidth(), bmp.getHeight()};
            texture.setData(bmp.getData(), 0);

            sampler.addressModeX = sr::Sampler::AddressMode::repeat;
            sampler.addressModeY = sr::Sampler::AddressMode::repeat;
            sampler.filter = sr::Sampler::Filter::linear;

            blendState.colorBlendSource = sr::BlendState::Factor::srcAlpha;
            blendState.colorBlendDest = sr::BlendState::Factor::invSrcAlpha;
            blendState.colorOperation = sr::BlendState::Operation::add;
            blendState.alphaBlendSource = sr::BlendState::Factor::one;
            blendState.alphaBlendDest = sr::BlendState::Factor::one;
            blendState.alphaOperation = sr::BlendState::Operation::add;
            blendState.enabled = true;

            depthState.read = true;
            depthState.write = true;
        }
        virtual ~Application() = default;

        Application(const Application&) = delete;
        Application& operator=(const Application&) = delete;
        Application(Application&&) = delete;
        Application& operator=(Application&&) = delete;

        void setup()
        {
            renderTarget = sr::RenderTarget{width, height};

            projection.setPerspective(static_cast<float>(sr::tau / 6.0),
                                      static_cast<float>(width) / static_cast<float>(height),
                                      1.0F, 1000.0F);

            view.setTranslation(0.0F, 0.0F, 100.0F);
        }

        void render()
        {
            renderer.setRenderTarget(&renderTarget);

            rotationY += 0.05F;
            model.setRotationY(rotationY);

            const auto modelViewProjection = projection * view * model;

            renderer.setViewport(sr::Rect<float>{0.0F, 0.0F, static_cast<float>(width), static_cast<float>(height)});
            renderer.setBlendState(blendState);
            renderer.setDepthState(depthState);

            renderer.clear(sr::Color{255, 255, 255, 255}, 1000.0F);

            renderer.setShader(shader);
            renderer.setTexture(texture, 0);
            renderer.setSampler(sampler, 0);

            renderer.drawTriangles(indices, vertices, modelViewProjection);
        }

        static std::string getResourcePath();

    protected:
        void onResize()
        {
            renderTarget.resize(width, height);

            projection.setPerspective(static_cast<float>(sr::tau / 6.0),
                                      static_cast<float>(width) / static_cast<float>(height),
                                      1.0F, 1000.0F);
        }

        std::size_t width;
        std::size_t height;

        sr::Matrix<float, 4> projection = sr::Matrix<float, 4>::identity();
        sr::Matrix<float, 4> view = sr::Matrix<float, 4>::identity();
        sr::Matrix<float, 4> model = sr::Matrix<float, 4>::identity();
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
