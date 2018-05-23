//
//  SoftwareRenderer
//

#pragma once

#include <vector>
#include "Buffer.hpp"

namespace sr
{
    class Texture
    {
    public:
        Texture()
        {
        }

    private:
        std::vector<const Buffer> levels;
    };
}
