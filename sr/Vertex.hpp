//
//  SoftwareRenderer
//

#ifndef VERTEX_H
#define VERTEX_H

#include "Color.hpp"
#include "Vector.hpp"

namespace sr
{
    class Vertex final
    {
    public:
        Vertex()
        {
        }

        Vertex(const Vector4F& initPosition, Color initColor, const Vector2F& initTexCoord, const Vector3F& initNormal):
            position(initPosition), color(initColor), normal(initNormal)
        {
            texCoords[0] = initTexCoord;
        }

        Vector4F position;
        Color color;
        Vector2F texCoords[2];
        Vector3F normal;
    };
}

#endif
