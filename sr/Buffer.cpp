//
//  SoftwareRenderer
//

#include "Buffer.hpp"

namespace sr
{
    static uint32_t getPixelSize(Buffer::Type type)
    {
        switch (type)
        {
            case Buffer::Type::RGB:
                return sizeof(uint8_t) * 3;
                break;
            case Buffer::Type::RGBA:
                return sizeof(uint8_t) * 4;
                break;
            case Buffer::Type::FLOAT32:
                return sizeof(float);
                break;
            default:
                return 0;
        }
    }

    bool Buffer::init(Type initType, uint32_t initWidth, uint32_t initHeight)
    {
        type = initType;
        width = initWidth;
        height = initHeight;

        uint32_t pixelSize = getPixelSize(type);
        if (pixelSize == 0) return false;

        data.resize(width * height * pixelSize);

        return true;
    }

    bool Buffer::resize(uint32_t newWidth, uint32_t newHeight)
    {
        width = newWidth;
        height = newHeight;

        uint32_t pixelSize = getPixelSize(type);
        if (pixelSize == 0) return false;

        data.resize(width * height * pixelSize);

        return true;
    }
}
