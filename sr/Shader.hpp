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
        struct VertexShaderOutput final
        {
            Vector<float, 4> position;
            Color color;
            Vector<float, 2> texCoords[2];
            Vector<float, 3> normal;
        };

        virtual ~Shader() = default;

        virtual VertexShaderOutput vertexShader(const Matrix<float, 4>& modelViewProjection, const Vertex& vertex) const = 0;
        virtual Color fragmentShader(const VertexShaderOutput& input, Sampler* samplers[2], Texture* textures[2]) const = 0;
    };
}

#endif
