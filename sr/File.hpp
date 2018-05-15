//
//  SoftwareRenderer
//

#pragma once

#include <string>

#if defined(_WIN32)
#include <Windows.h>
#endif

namespace sr
{
    class File final
    {
    public:
        enum Mode
        {
            READ = 0x01,
            WRITE = 0x02,
            CREATE = 0x04,
            APPEND = 0x08
        };

        enum Seek
        {
            BEGIN,
            CURRENT,
            END
        };

        File();
        File(const std::string& filename, int mode);
        ~File();

        File(const File&) = delete;
        File& operator=(const File&) = delete;

        File(File&&);
        File& operator=(File&&);

        bool open(const std::string& filename, int mode);
        bool close();

        inline bool isOpen() const
        {
#if defined(_WIN32)
            return file != INVALID_HANDLE_VALUE;
#else
            return fd != -1;
#endif
        }

        bool read(void* buffer, uint32_t size) const;
        bool read(void* buffer, uint32_t size, uint32_t& bytesRead) const;
        bool write(const void* buffer, uint32_t size) const;
        bool write(const void* buffer, uint32_t size, uint32_t& bytesWritten) const;
        bool seek(int32_t offset, int method) const;
        uint32_t getOffset() const;

    private:
#if defined(_WIN32)
        HANDLE file = INVALID_HANDLE_VALUE;
#else
        int fd = -1;
#endif
    };
}
