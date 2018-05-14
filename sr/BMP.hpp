//
//  SoftwareRenderer
//

#pragma once

#include <string>
#include <vector>
#include "Buffer.hpp"

namespace sr
{
    class BMP
    {
    public:
        Buffer getBuffer();
        bool setBuffer(const Buffer& buffer);

        bool load(const std::string& filename);
        bool save(const std::string& filename);

    private:
        uint32_t width = 0;
        uint32_t height = 0;
        std::vector<uint8_t> data;
    };
}
