//
//  SoftwareRenderer
//

#pragma once

#include "Color.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace sr
{
    class Vertex final
    {
    public:
        Vertex()
        {
        }

        Vertex(const Vector4& initPosition, Color initColor, const Vector2& initTexCoord, const Vector3& initNormal):
            position(initPosition), color(initColor), normal(initNormal)
        {
            texCoords[0] = initTexCoord;
        }

        Vector4 position;
        Color color;
        Vector2 texCoords[2];
        Vector3 normal;
    };
}
