//
//  SoftwareRenderer
//

#ifndef SHADER_HPP
#define SHADER_HPP

#include <array>
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
            std::array<Vector<float, 2>, 2> texCoords;
            Vector<float, 3> normal;
        };

        virtual ~Shader() = default;

        virtual VertexShaderOutput vertexShader(const Matrix<float, 4>& modelViewProjection,
                                                const Vertex& vertex) const = 0;
        virtual Color fragmentShader(const VertexShaderOutput& input,
                                     const std::array<Sampler*, 2>& samplers,
                                     const std::array<Texture*, 2>& textures) const = 0;
    };
}

#endif
