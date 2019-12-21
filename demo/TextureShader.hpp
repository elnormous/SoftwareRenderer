//
//  SoftwareRenderer
//

#ifndef TEXTURESHADER_H
#define TEXTURESHADER_H

#include "sr.hpp"

namespace demo
{
    class TextureShader: public sr::Shader
    {
    public:
        virtual VSOutput vertexShader(const sr::Matrix4F& modelViewProjection, const sr::Vertex& vertex) const override
        {
            VSOutput result;

            // transform to clip space
            result.position = modelViewProjection * vertex.position;
            result.color = vertex.color;
            result.texCoords[0] = vertex.texCoords[0];
            result.texCoords[1] = vertex.texCoords[1];
            result.normal = vertex.normal;

            return result;
        }

        virtual sr::Color fragmentShader(const VSOutput& input, sr::Sampler* samplers[2], sr::Texture* textures[2]) const override
        {
            sr::Color sampleColor = textures[0]->sample(samplers[0], input.texCoords[0]);

            sr::Color result;
            result.r = input.color.r * sampleColor.r;
            result.g = input.color.g * sampleColor.g;
            result.b = input.color.b * sampleColor.b;
            result.a = input.color.a * sampleColor.a;

            return result;
        }
    };
}

#endif
