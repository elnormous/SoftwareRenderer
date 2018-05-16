//
//  SoftwareRenderer
//

#pragma once

#include <memory>
#include "Renderer.hpp"
#include "Buffer.hpp"
#include "Color.hpp"

namespace sr
{
    class Material final
    {
    public:
        Material()
        {
            std::fill(std::begin(textures), std::end(textures), nullptr);
        }

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;

        Material(Material&&) = delete;
        Material& operator=(Material&&) = delete;

        //BlendState* blendState = nullptr;
        Shader* shader = nullptr;
        Buffer* textures[2];
        //CullMode cullMode = CullMode::BACK;
        Color diffuseColor = Color::WHITE;
        float opacity = 1.0F;
    };
}
