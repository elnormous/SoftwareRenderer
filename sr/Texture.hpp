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

        void setLevel(const Buffer& buffer, uint32_t level)
        {
            if (level >= levels.size()) levels.resize(level + 1);

            levels[level] = buffer;
        }

        const Buffer& getLevel(uint32_t level) const
        {
            return levels[level];
        }

    private:
        std::vector<Buffer> levels;
        uint32_t minLOD = 0;
        uint32_t maxLOD = UINT_MAX;
        float lodBias = 0.0F;
    };
}
