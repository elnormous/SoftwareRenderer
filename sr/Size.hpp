//
//  SoftwareRenderer
//

#ifndef SR_SIZE_HPP
#define SR_SIZE_HPP

#include <array>
#include <cstddef>
#include <type_traits>
#include "Vector.hpp"

namespace sr
{
    template <typename T, std::size_t N> class Size final
    {
    public:
        std::array<T, N> v{};

        constexpr Size() noexcept {}

        template <typename ...A>
        explicit constexpr Size(A... args) noexcept:
            v{args...}
        {
        }

        template <std::size_t X = N, std::size_t N2, typename std::enable_if<(X != N2)>::type* = nullptr>
        explicit Size(const Size<T, N2>& size) noexcept
        {
            for (std::size_t i = 0; i < N && i < N2; ++i)
                v[i] = size.v[i];
        }

        explicit constexpr Size(const Vector<T, N>& vec) noexcept:
            v{vec.v}
        {
        }

        T& operator[](std::size_t index) noexcept { return v[index]; }
        [[nodiscard]] constexpr T operator[](std::size_t index) const noexcept { return v[index]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 1)>::type* = nullptr>
        T& width() noexcept { return v[0]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 1)>::type* = nullptr>
        [[nodiscard]] constexpr T width() const noexcept { return v[0]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 2)>::type* = nullptr>
        T& height() noexcept { return v[1]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 2)>::type* = nullptr>
        [[nodiscard]] constexpr T height() const noexcept { return v[1]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 3)>::type* = nullptr>
        T& depth() noexcept { return v[2]; }

        template <std::size_t X = N, typename std::enable_if<(X >= 3)>::type* = nullptr>
        [[nodiscard]] constexpr T depth() const noexcept { return v[2]; }

        void scale(const Vector<T, N>& scale) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] *= scale.v[i];
        }

        [[nodiscard]] const Size operator+(const Size& size) const noexcept
        {
            Size result = *this;
            for (std::size_t i = 0; i < N; ++i)
                result.v[i] += size.v[i];
            return result;
        }

        Size& operator+=(const Size& size) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] += size.v[i];
            return *this;
        }

        [[nodiscard]] const Size operator-(const Size& size) const noexcept
        {
            Size result = *this;
            for (std::size_t i = 0; i < N; ++i)
                result.v[i] -= size.v[i];
            return result;
        }

        Size& operator-=(const Size& size) noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                v[i] -= size.v[i];
            return *this;
        }

        [[nodiscard]] const Size operator-() const noexcept
        {
            Size result = *this;
            for (auto& c : result.v)
                c = -c;
            return result;
        }

        [[nodiscard]] const Size operator*(const T scalar) const noexcept
        {
            Size result = *this;
            for (auto& c : result.v)
                c *= scalar;
            return result;
        }

        Size& operator*=(const T scalar) noexcept
        {
            for (auto& c : v)
                c *= scalar;
            return *this;
        }

        [[nodiscard]] const Size operator/(const T scalar) const noexcept
        {
            Size result = *this;
            for (auto& c : result.v)
                c /= scalar;
            return result;
        }

        Size& operator/=(const T scalar) noexcept
        {
            for (auto& c : v)
                c /= scalar;
            return *this;
        }

        [[nodiscard]] bool operator<(const Size& size) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] < size.v[i]) return true;
                else if (size.v[i] < v[i]) return false;

            return false;
        }

        [[nodiscard]] bool operator==(const Size& size) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] != size.v[i]) return false;
            return true;
        }

        [[nodiscard]] bool operator!=(const Size& size) const noexcept
        {
            for (std::size_t i = 0; i < N; ++i)
                if (v[i] != size.v[i]) return true;
            return false;
        }

        [[nodiscard]] bool isZero() const noexcept
        {
            for (const auto& c : v)
                if (c != T(0)) return false;
            return true;
        }

        [[nodiscard]] T volume() const noexcept
        {
            T result = 0;
            for (const auto& c : v)
                result *= c;
            return result;
        }
    };

    template <typename T, std::size_t N>
    [[nodiscard]] inline const Size<T, N> operator*(const Size<T, N>& size, const Vector<T, N>& v) noexcept
    {
        auto result = size;
        for (std::size_t i = 0; i < N; ++i)
            result.v[i] *= v.v[i];
        return result;
    }

    template <typename T, std::size_t N>
    [[nodiscard]] inline const Size<T, N> operator/(const Size<T, N>& size, const Vector<T, N>& v) noexcept
    {
        auto result = size;
        for (std::size_t i = 0; i < N; ++i)
            result.v[i] /= v.v[i];
        return result;
    }
}

#endif
