//
//  SoftwareRenderer
//

#pragma once

#include <string>

namespace sr
{
    class WAV
    {
    public:
        bool load(const std::string& filename);
        bool save(const std::string& filename);
    };
}
