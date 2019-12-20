//
//  SoftwareRenderer
//

#ifndef WAV_H
#define WAV_H

#include <string>

namespace sr
{
    class WAV final
    {
    public:
        WAV() = default;

        explicit WAV(const std::string& filename)
        {
        }

        void save(const std::string& filename)
        {
        }
    };
}

#endif
