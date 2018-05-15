//
//  SoftwareRenderer
//

#include "Vertex.hpp"

namespace sr
{
    Vertex::Vertex()
    {
    }

    Vertex::Vertex(const Vector4& initPosition, Color initColor, const Vector2& initTexCoord, const Vector3& initNormal):
        position(initPosition), color(initColor), normal(initNormal)
    {
        texCoords[0] = initTexCoord;
    }
}
