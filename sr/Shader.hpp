//
//  SoftwareRenderer
//

#pragma once

#include "Buffer.hpp"
#include "Matrix4.hpp"
#include "Vertex.hpp"

namespace sr
{
    class Shader
    {
    public:
        struct VSOutput
        {
            Vector4 position;
            Color color;
            Vector2 texCoords[2];
            Vector3 normal;
        };

        virtual VSOutput vertexShader(const Matrix4& modelViewProjection, const Vertex& vertex) const = 0;
        virtual Color fragmentShader(VSOutput input, Sampler samplers[2]) const = 0;
    };
}
