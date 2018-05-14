//
//  SoftwareRenderer
//

#pragma once

#include "Color.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"

namespace sr
{
    class Vertex final
    {
    public:
        Vertex();
        Vertex(const Vector3& initPosition, Color initColor, const Vector2& initTexCoord, const Vector3& initNormal);

        Vector3 position;
        Color color;
        Vector2 texCoords[2];
        Vector3 normal;
    };
}
