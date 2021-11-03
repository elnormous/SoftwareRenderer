//
//  SoftwareRenderer
//

#ifndef SR_RENDERERROR_HPP
#define SR_RENDERERROR_HPP

#include <stdexcept>

namespace sr
{
    class RenderError final: public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
}

#endif
