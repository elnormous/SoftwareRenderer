//
//  SoftwareRenderer
//

#pragma once

#include "sr.hpp"

namespace demo
{
    class TextureShader: public sr::Shader
    {
    public:
        virtual VSOutput vertexShader(const sr::Matrix4& modelViewProjection, const sr::Vertex& vertex) const
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

        virtual sr::Color fragmentShader(VSOutput input, sr::Sampler samplers[2]) const
        {
            sr::Color sampleColor = samplers[0].get(input.texCoords[0]);

            sr::Color result;
            result.r = input.color.r * sampleColor.r;
            result.g = input.color.g * sampleColor.g;
            result.b = input.color.b * sampleColor.b;
            result.a = input.color.a * sampleColor.a;

            return result;
        }
    };
}
