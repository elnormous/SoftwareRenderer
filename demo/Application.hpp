//
//  SoftwareRenderer
//

#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <cstdint>
#include <memory>
#include "sr.hpp"
#include "Bmp.hpp"

namespace demo
{
    inline sr::VertexShaderOutput vertexShader(const sr::Matrix<float, 4>& modelViewProjection,
                                               const sr::Vertex& vertex)
    {
        sr::VertexShaderOutput result;

        // transform to clip space
        result.position = modelViewProjection * vertex.position;
        result.color = vertex.color;
        result.texCoords[0] = vertex.texCoords[0];
        result.texCoords[1] = vertex.texCoords[1];
        result.normal = vertex.normal;

        return result;
    }

    inline sr::Color fragmentShader(const sr::VertexShaderOutput& input,
                                    const std::array<const sr::Sampler*, 2>& samplers,
                                    const std::array<const sr::Texture*, 2>& textures)
    {
        const auto sampleColor = textures[0]->sample(samplers[0], input.texCoords[0]);

        const sr::Color result{
            input.color.r * sampleColor.r,
            input.color.g * sampleColor.g,
            input.color.b * sampleColor.b,
            input.color.a * sampleColor.a
        };

        return result;
    }

    std::string getResourcePath();

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
                sr::Vertex{sr::Vector<float, 4>{-20.0F, -20.0F, -20.0F, 1.0F}, sr::Color{0xFF0000FFU}, sr::Vector<float, 2>{0.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{-20.0F, 20.0F, -20.0F, 1.0F}, sr::Color{0x00FF00FFU}, sr::Vector<float, 2>{0.0F, 1.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, -20.0F, -20.0F, 1.0F}, sr::Color{0x0000FFFFU}, sr::Vector<float, 2>{1.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, 20.0F, -20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{1.0F, 1.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},

                // back
                sr::Vertex{sr::Vector<float, 4>{-20.0F, -20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{0.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{-20.0F, 20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{0.0F, 1.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, -20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{1.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, 20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{1.0F, 1.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},

                // left
                sr::Vertex{sr::Vector<float, 4>{-20.0F, -20.0F, -20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{0.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{-20.0F, 20.0F, -20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{0.0F, 4.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{-20.0F, -20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{4.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{-20.0F, 20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{4.0F, 4.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},

                // right
                sr::Vertex{sr::Vector<float, 4>{20.0F, -20.0F, -20.0F, 1.0F}, sr::Color{0xFFFFFFA0U}, sr::Vector<float, 2>{0.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, 20.0F, -20.0F, 1.0F}, sr::Color{0xFFFFFFA0U}, sr::Vector<float, 2>{0.0F, 1.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, -20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFA0U}, sr::Vector<float, 2>{1.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, 20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFA0U}, sr::Vector<float, 2>{1.0F, 1.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},

                // bottom
                sr::Vertex{sr::Vector<float, 4>{-20.0F, -20.0F, -20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{0.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{-20.0F, -20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{0.0F, 1.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, -20.0F, -20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{1.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, -20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{1.0F, 1.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},

                // top
                sr::Vertex{sr::Vector<float, 4>{-20.0F, 20.0F, -20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{0.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{-20.0F, 20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{0.0F, 1.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, 20.0F, -20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{1.0F, 0.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}},
                sr::Vertex{sr::Vector<float, 4>{20.0F, 20.0F, 20.0F, 1.0F}, sr::Color{0xFFFFFFFFU}, sr::Vector<float, 2>{1.0F, 1.0F}, sr::Vector<float, 3>{0.0F, 0.0F, 1.0F}}
            }
        {
            const sr::Bmp bmp{getResourcePath() + "/cube.bmp"};
            texture = sr::Texture{sr::PixelFormat::rgba8, bmp.getWidth(), bmp.getHeight()};
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

        void setup(std::size_t newWidth, std::size_t newHeight)
        {
            viewport.size.v[0] = static_cast<float>(newWidth);
            viewport.size.v[1] = static_cast<float>(newHeight);

            frameBuffer.resize(newWidth, newHeight);
            depthBuffer.resize(newWidth, newHeight);

            projection.setPerspective(sr::tau<float> / 6.0,
                                      static_cast<float>(newWidth) / static_cast<float>(newHeight),
                                      1.0F, 1000.0F);

            view.setTranslation(0.0F, 0.0F, 100.0F);
        }
        
        void render()
        {
            rotationY += 0.05F;
            model.setRotationY(rotationY);

            const auto modelViewProjection = projection * view * model;

            clear(frameBuffer, sr::Color{255, 255, 255, 255});
            clear(depthBuffer, 1000.0F);

            drawTriangles(frameBuffer,
                          depthBuffer,
                          vertexShader,
                          fragmentShader,
                          {&sampler, nullptr},
                          {&texture, nullptr},
                          viewport,
                          scissorRect,
                          blendState,
                          depthState,
                          indices,
                          vertices,
                          modelViewProjection);
        }
        
        const sr::Texture& getFrameBuffer() const noexcept { return frameBuffer; }
        sr::Texture& getFrameBuffer() noexcept { return frameBuffer; }

    protected:
        void onResize(std::size_t newWidth, std::size_t newHeight)
        {
            viewport.size.v[0] = static_cast<float>(newWidth);
            viewport.size.v[1] = static_cast<float>(newHeight);
            
            frameBuffer.resize(newWidth, newHeight);
            depthBuffer.resize(newWidth, newHeight);

            projection.setPerspective(sr::tau<float> / 6.0,
                                      static_cast<float>(newWidth) / static_cast<float>(newHeight),
                                      1.0F, 1000.0F);
        }

    private:
        sr::Matrix<float, 4> projection = sr::Matrix<float, 4>::identity();
        sr::Matrix<float, 4> view = sr::Matrix<float, 4>::identity();
        sr::Matrix<float, 4> model = sr::Matrix<float, 4>::identity();
        float rotationY = 0.0F;

        sr::Texture frameBuffer{sr::PixelFormat::rgba8};
        sr::Texture depthBuffer{sr::PixelFormat::float32};

        sr::Rect<float> viewport;
        sr::Rect<float> scissorRect{0.0F, 0.0F, 1.0F, 1.0F};
        sr::BlendState blendState;
        sr::DepthState depthState;

        sr::Sampler sampler;
        sr::Texture texture;

        std::vector<std::size_t> indices;
        std::vector<sr::Vertex> vertices;
    };
}

#endif
