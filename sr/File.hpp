//
//  SoftwareRenderer
//

#ifndef FILE_H
#define FILE_H

#include <stdexcept>
#include <string>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

namespace sr
{
    class File final
    {
    public:
        enum Mode
        {
            Read = 0x01,
            Write = 0x02,
            Create = 0x04,
            Append = 0x08,
            Truncate = 0x10
        };

        enum Seek
        {
            Begin,
            Current,
            End
        };

        File() = default;

        File(const std::string& filename, int mode)
        {
#if defined(_WIN32)
            const DWORD access =
                ((mode & Mode::Read) ? GENERIC_READ : 0) |
                ((mode & Mode::Write) ? GENERIC_WRITE : 0) |
                ((mode & Mode::Append) ? FILE_APPEND_DATA : 0);

            const DWORD createDisposition = (mode & Truncate) ?
                ((mode & Mode::Create) ? CREATE_ALWAYS : TRUNCATE_EXISTING) :
                ((mode & Mode::Create) ? OPEN_ALWAYS : OPEN_EXISTING);

            int size = MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, nullptr, 0);
            if (size == 0)
                throw std::runtime_error("Failed to convert UTF-8 to wide char");

            std::vector<WCHAR> buffer(size);
            if (MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, buffer.data(), size) == 0)
                throw std::runtime_error("Failed to convert UTF-8 to wide char");

            // relative paths longer than MAX_PATH are not supported
            if (buffer.size() > MAX_PATH)
                buffer.insert(buffer.begin(), {L'\\', L'\\', L'?', L'\\'});

            file = CreateFileW(buffer.data(), access, 0, nullptr, createDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
            if (file == INVALID_HANDLE_VALUE)
                throw std::runtime_error("Failed to open " + filename);
#else
            const int access =
                ((mode & Mode::Read) && (mode & Mode::Write) ? O_RDWR :
                 (mode & Mode::Read) ? O_RDONLY :
                 (mode & Mode::Write) ? O_WRONLY : 0) |
                ((mode & Mode::Create) ? O_CREAT : 0) |
                ((mode & Mode::Append) ? O_APPEND : 0) |
                ((mode & Mode::Truncate) ? O_TRUNC : 0);

            fd = ::open(filename.c_str(), access, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (fd == -1)
                throw std::runtime_error("Failed to open " + filename);
#endif
        }

        ~File()
        {
#if defined(_WIN32)
            if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
#else
            if (fd != -1) close(fd);
#endif
        }

        File(File&& other)
        {
#if defined(_WIN32)
            file = other.file;
            other.file = nullptr;
#else
            fd = other.fd;
            other.fd = -1;
#endif
        }

        File& operator=(File&& other)
        {
            if (&other != this)
            {
#if defined(_WIN32)
                if (file != INVALID_HANDLE_VALUE) CloseHandle(file);
                file = other.file;
                other.file = nullptr;
#else
                if (fd != -1) close(fd);
                fd = other.fd;
                other.fd = -1;
#endif
            }

            return *this;
        }

        inline bool isOpen() const
        {
#if defined(_WIN32)
            return file != INVALID_HANDLE_VALUE;
#else
            return fd != -1;
#endif
        }

        uint32_t read(void* buffer, uint32_t size, bool all = false) const
        {
            if (all)
            {
                uint8_t* dest = static_cast<uint8_t*>(buffer);
                uint32_t remaining = size;

                while (remaining > 0)
                {
                    const uint32_t bytesRead = read(dest, remaining);

                    if (bytesRead == 0)
                        throw std::runtime_error("End of file reached");

                    remaining -= bytesRead;
                    dest += bytesRead;
                }

                return size;
            }
            else
            {
#if defined(_WIN32)
                if (file == INVALID_HANDLE_VALUE)
                    throw std::runtime_error("File is not open");

                DWORD n;
                if (!ReadFile(file, buffer, size, &n, nullptr))
                    throw std::runtime_error("Failed to read from file");

                return static_cast<uint32_t>(n);
#else
                if (fd == -1)
                    throw std::runtime_error("File is not open");

                const ssize_t ret = ::read(fd, buffer, size);

                if (ret == -1)
                    throw std::runtime_error("Failed to read from file");

                return static_cast<uint32_t>(ret);
#endif
            }
        }

        uint32_t write(const void* buffer, uint32_t size, bool all = false)
        {
            if (all)
            {
                const uint8_t* src = static_cast<const uint8_t*>(buffer);
                uint32_t remaining = size;

                while (remaining > 0)
                {
                    const uint32_t bytesWritten = write(src, remaining);
                    remaining -= bytesWritten;
                    src += bytesWritten;
                }

                return size;
            }
            else
            {
#if defined(_WIN32)
                if (file == INVALID_HANDLE_VALUE)
                    throw std::runtime_error("File is not open");

                DWORD n;
                if (!WriteFile(file, buffer, size, &n, nullptr))
                    throw std::runtime_error("Failed to write to file");

                return static_cast<uint32_t>(n);
#else
                if (fd == -1)
                    throw std::runtime_error("File is not open");

                const ssize_t ret = ::write(fd, buffer, size);

                if (ret == -1)
                    throw std::runtime_error("Failed to write to file");

                return static_cast<uint32_t>(ret);
#endif
            }
        }

        void seek(int32_t offset, int method) const
        {
#if defined(_WIN32)
            if (file == INVALID_HANDLE_VALUE)
                throw std::runtime_error("File is not open");

            const DWORD moveMethod =
                (method == Seek::Begin) ? FILE_BEGIN :
                (method == Seek::Current) ? FILE_CURRENT :
                (method == Seek::End) ? FILE_END :
                throw std::runtime_error("Unsupported seek method");

            if (SetFilePointer(file, offset, nullptr, moveMethod) == 0)
                throw std::runtime_error("Failed to seek file");
#else
            if (fd == -1)
                throw std::runtime_error("File is not open");

            const int whence =
                (method == Seek::Begin) ? SEEK_SET :
                (method == Seek::Current) ? SEEK_CUR :
                (method == Seek::End) ? SEEK_END :
                throw std::runtime_error("Unsupported seek method");

            if (lseek(fd, offset, whence) == -1)
                throw std::runtime_error("Failed to seek file");
#endif
        }

        uint32_t getOffset() const
        {
#if defined(_WIN32)
            if (file == INVALID_HANDLE_VALUE)
                throw std::runtime_error("File is not open");

            DWORD ret = SetFilePointer(file, 0, nullptr, FILE_CURRENT);
            return static_cast<uint32_t>(ret);
#else
            if (fd == -1)
                throw std::runtime_error("File is not open");

            off_t ret = lseek(fd, 0, SEEK_CUR);
            if (ret == -1) return 0;
            return static_cast<uint32_t>(ret);
#endif
        }

    private:
#if defined(_WIN32)
        HANDLE file = INVALID_HANDLE_VALUE;
#else
        int fd = -1;
#endif
    };
}

#endif
