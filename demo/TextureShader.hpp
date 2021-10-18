//
//  SoftwareRenderer
//

#ifndef TEXTURESHADER_HPP
#define TEXTURESHADER_HPP

#include "sr.hpp"

namespace demo
{
    class TextureShader: public sr::Shader
    {
    public:
        virtual VertexShaderOutput vertexShader(const sr::Matrix<float, 4>& modelViewProjection,
                                                const sr::Vertex& vertex) const override
        {
            VertexShaderOutput result;

            // transform to clip space
            result.position = modelViewProjection * vertex.position;
            result.color = vertex.color;
            result.texCoords[0] = vertex.texCoords[0];
            result.texCoords[1] = vertex.texCoords[1];
            result.normal = vertex.normal;

            return result;
        }

        virtual sr::Color fragmentShader(const VertexShaderOutput& input,
                                         const std::array<sr::Sampler*, 2>& samplers,
                                         const std::array<sr::Texture*, 2>& textures) const override
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
    };
}

#endif
