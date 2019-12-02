//
//  SoftwareRenderer
//

#ifndef MATERIAL_H
#define MATERIAL_H

#include <memory>
#include "Color.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

namespace sr
{
    class Material final
    {
    public:
        Material() = default;

        //BlendState* blendState = nullptr;
        Shader* shader = nullptr;
        Texture* textures[2]{nullptr, nullptr};
        //CullMode cullMode = CullMode::BACK;
        Color diffuseColor = Color::WHITE;
        float opacity = 1.0F;
    };
}

#endif
