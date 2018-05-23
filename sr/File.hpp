//
//  SoftwareRenderer
//

#pragma once

#include <string>

#if defined(_WIN32)
#include <Windows.h>
#else
#include <fcntl.h>
#include <unistd.h>
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

        File()
        {
        }

        File(const std::string& filename, int mode)
        {
            open(filename, mode);
        }

        ~File()
        {
            close();
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
                if (fd != -1) ::close(fd);
                fd = other.fd;
                other.fd = -1;
#endif
            }

            return *this;
        }

        bool open(const std::string& filename, int mode)
        {
#if defined(_WIN32)
            DWORD access = 0;
            if (mode & READ) access |= GENERIC_READ;
            if (mode & WRITE) access |= GENERIC_WRITE;
            if (mode & APPEND) access |= FILE_APPEND_DATA;
            DWORD createDisposition = (mode & CREATE) ? OPEN_ALWAYS : OPEN_EXISTING;

            WCHAR buffer[MAX_PATH];
            if (MultiByteToWideChar(CP_UTF8, 0, filename.c_str(), -1, buffer, MAX_PATH) == 0)
                return false;
            file = CreateFileW(buffer, access, 0, nullptr, createDisposition, FILE_ATTRIBUTE_NORMAL, nullptr);
            return file != INVALID_HANDLE_VALUE;
#else
            int access = 0;
            if ((mode & READ) && (mode & WRITE)) access |= O_RDWR;
            else if (mode & READ) access |= O_RDONLY;
            else if (mode & WRITE) access |= O_WRONLY;
            if (mode & CREATE) access |= O_CREAT;
            if (mode & APPEND) access |= O_APPEND;

            fd = ::open(filename.c_str(), access, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            return fd != -1;
#endif
        }

        bool close()
        {
#if defined(_WIN32)
            if (file == INVALID_HANDLE_VALUE) return false;
            return CloseHandle(file) != 0;
#else
            if (fd == -1) return false;
            return ::close(fd) == 0;
#endif
        }

        inline bool isOpen() const
        {
#if defined(_WIN32)
            return file != INVALID_HANDLE_VALUE;
#else
            return fd != -1;
#endif
        }

        bool read(void* buffer, uint32_t size) const
        {
            uint8_t* dest = static_cast<uint8_t*>(buffer);
            uint32_t bytesRead;

            for (; size > 0; size -= bytesRead, dest += bytesRead)
            {
                if (!read(dest, size, bytesRead)) return false;
                if (bytesRead == 0) return false;
            }

            return true;
        }

        bool read(void* buffer, uint32_t size, uint32_t& bytesRead) const
        {
#if defined(_WIN32)
            if (file == INVALID_HANDLE_VALUE) return false;
            DWORD n;
            BOOL ret = ReadFile(file, buffer, size, &n, nullptr);
            bytesRead = static_cast<uint32_t>(n);
            return ret != 0;
#else
            if (fd == -1) return false;
            ssize_t ret = ::read(fd, buffer, size);
            bytesRead = static_cast<uint32_t>(ret);
            return ret != -1;
#endif
        }

        bool write(const void* buffer, uint32_t size) const
        {
            const uint8_t* src = static_cast<const uint8_t*>(buffer);
            uint32_t bytesWritten;

            for (; size > 0; size -= bytesWritten, src += bytesWritten)
            {
                if (!write(src, size, bytesWritten)) return false;
                if (bytesWritten == 0) return false;
            }

            return true;
        }

        bool write(const void* buffer, uint32_t size, uint32_t& bytesWritten) const
        {
#if defined(_WIN32)
            if (file == INVALID_HANDLE_VALUE) return false;
            DWORD n;
            BOOL ret = WriteFile(file, buffer, size, &n, nullptr);
            bytesWritten = static_cast<uint32_t>(n);
            return ret != 0;
#else
            if (fd == -1) return false;
            ssize_t ret = ::write(fd, buffer, size);
            bytesWritten = static_cast<uint32_t>(ret);
            return ret != -1;
#endif
        }

        bool seek(int32_t offset, int method) const
        {
#if defined(_WIN32)
            if (file == INVALID_HANDLE_VALUE) return false;
            DWORD moveMethod = 0;
            if (method == BEGIN) moveMethod = FILE_BEGIN;
            else if (method == CURRENT) moveMethod = FILE_CURRENT;
            else if (method == END) moveMethod = FILE_END;
            return SetFilePointer(file, offset, nullptr, moveMethod) != 0;
#else
            if (fd == -1) return false;
            int whence = 0;
            if (method == BEGIN) whence = SEEK_SET;
            else if (method == CURRENT) whence = SEEK_CUR;
            else if (method == END) whence = SEEK_END;
            return lseek(fd, offset, whence) != -1;
#endif
        }

        uint32_t getOffset() const
        {
#if defined(_WIN32)
            if (file == INVALID_HANDLE_VALUE) return 0;
            DWORD ret = SetFilePointer(file, 0, nullptr, FILE_CURRENT);
            return static_cast<uint32_t>(ret);
#else
            if (fd == -1) return 0;
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
