//
//  SoftwareRenderer
//

#ifndef SHADER_H
#define SHADER_H

#include "Matrix.hpp"
#include "Texture.hpp"
#include "Vertex.hpp"

namespace sr
{
    class Shader
    {
    public:
        struct VSOutput
        {
            Vector4F position;
            Color color;
            Vector2F texCoords[2];
            Vector3F normal;
        };

        virtual VSOutput vertexShader(const Matrix4F& modelViewProjection, const Vertex& vertex) const = 0;
        virtual Color fragmentShader(VSOutput input, Sampler* samplers[2], Texture* textures[2]) const = 0;
    };
}

#endif
