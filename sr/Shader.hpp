//
//  SoftwareRenderer
//

#pragma once

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

        VSOutput vertexShader(const Matrix4& modelViewProjection, const Vertex& vertex) const
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

        void fragmentShader() const
        {

        }
    };
}
