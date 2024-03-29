//
//  SoftwareRenderer
//

#ifndef SR_VERTEX_HPP
#define SR_VERTEX_HPP

#include "Color.hpp"
#include "Vector.hpp"

namespace sr
{
    class Vertex final
    {
    public:
        Vertex() noexcept = default;

        Vertex(const Vector<float, 4>& initPosition,
               const Color initColor,
               const Vector<float, 2>& initTexCoord,
               const Vector<float, 3>& initNormal) noexcept:
            position{initPosition}, color{initColor},
            texCoords{initTexCoord}, normal{initNormal}
        {
        }

        Vector<float, 4> position;
        Color color;
        Vector<float, 2> texCoords[2];
        Vector<float, 3> normal;
    };
}

#endif
