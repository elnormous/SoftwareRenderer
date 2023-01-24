//
//  SoftwareRenderer
//

#ifndef SR_RECT_HPP
#define SR_RECT_HPP

#include "Vector.hpp"
#include "Size.hpp"

namespace sr
{
    template <typename T> class Rect final
    {
    public:
        Vector<T, 2> position;
        Size<T, 2> size;

        constexpr Rect() noexcept {}

        constexpr Rect(const T width, const T height) noexcept:
            size{width, height}
        {
        }

        constexpr Rect(const T x, const T y,
                       const T width, const T height) noexcept:
            position{x, y}, size{width, height}
        {
        }

        constexpr Rect(const Vector<T, 2>& initPosition,
                       const T width, const T height) noexcept:
            position{initPosition}, size{width, height}
        {
        }

        constexpr Rect(const Vector<T, 2>& initPosition,
                       const Size<T, 2>& initSize) noexcept:
            position{initPosition}, size{initSize}
        {
        }

        [[nodiscard]] constexpr bool isEmpty() const noexcept
        {
            return size.isZero();
        }

        void setPosition(const T x, const T y) noexcept
        {
            position.v[0] = x;
            position.v[1] = y;
        }

        void setPosition(const Vector<T, 2>& newPosition) noexcept
        {
            position = newPosition;
        }

        [[nodiscard]] constexpr T left() const noexcept
        {
            return position.v[0];
        }

        [[nodiscard]] constexpr T bottom() const noexcept
        {
            return position.v[1];
        }

        [[nodiscard]] constexpr T right() const noexcept
        {
            return position.v[0] + size.v[0];
        }

        [[nodiscard]] constexpr T top() const noexcept
        {
            return position.v[1] + size.v[1];
        }

        [[nodiscard]] constexpr Vector<T, 2> bottomLeft() const noexcept
        {
            return position;
        }

        [[nodiscard]] constexpr Vector<T, 2> topRight() const noexcept
        {
            return Vector<T, 2>{position.v[0] + size.v[0], position.v[1] + size.v[1]};
        }

        [[nodiscard]] constexpr bool containsPoint(const T x, const T y) const noexcept
        {
            return x >= position.v[0] && x <= (position.v[0] + size.v[0]) &&
                y >= position.v[1] && y <= (position.v[1] + size.v[1]);
        }

        [[nodiscard]] constexpr bool containsPoint(const Vector<T, 2>& point) const noexcept
        {
            return point.v[0] >= position.v[0] && point.v[0] <= (position.v[0] + size.v[0]) &&
                point.v[1] >= position.v[1] && point.v[1] <= (position.v[1] + size.v[1]);
        }

        [[nodiscard]] constexpr bool contains(const T x, const T y,
                                              const T width, const T height) const noexcept
        {
            return containsPoint(x, y) && containsPoint(x + width, y + height);
        }

        [[nodiscard]] constexpr bool contains(const Rect& r) const noexcept
        {
            return contains(r.position.v[0], r.position.v[1], r.size.v[0], r.size.v[1]);
        }

        [[nodiscard]] bool intersects(const T x, const T y,
                                      const T width, const T height) const noexcept
        {
            T t;
            if ((t = x - position.v[0]) > size.v[0] || -t > width)
                return false;
            if ((t = y - position.v[1]) > size.v[1] || -t > height)
                return false;
            return true;
        }

        [[nodiscard]] constexpr bool intersects(const Rect& r) const noexcept
        {
            return intersects(r.position.v[0], r.position.v[1], r.size.v[0], r.size.v[1]);
        }

        [[nodiscard]] static bool intersect(const Rect& r1, const Rect& r2, Rect& dst) noexcept
        {
            const T xmin = std::max(r1.position.v[0], r2.position.v[0]);
            const T xmax = std::min(r1.right(), r2.right());
            if (xmax > xmin)
            {
                const T ymin = std::max(r1.position.v[1], r2.position.v[1]);
                const T ymax = std::min(r1.bottom(), r2.bottom());
                if (ymax > ymin)
                {
                    dst.position.v[0] = xmin;
                    dst.position.v[1] = ymin;
                    dst.size.v[0] = xmax - xmin;
                    dst.size.v[1] = ymax - ymin;
                    return true;
                }
            }

            dst.position.v[0] = dst.position.v[1] = dst.size.v[0] = dst.size.v[1] = 0;
            return false;
        }

        static void combine(const Rect& r1, const Rect& r2, Rect& dst) noexcept
        {
            dst.position.v[0] = std::min(r1.position.v[0], r2.position.v[0]);
            dst.position.v[1] = std::min(r1.position.v[1], r2.position.v[1]);
            dst.size.v[0] = std::max(r1.position.v[0] + r1.size.v[0], r2.position.v[0] + r2.size.v[0]) - dst.position.v[0];
            dst.size.v[1] = std::max(r1.position.v[1] + r1.size.v[1], r2.position.v[1] + r2.size.v[1]) - dst.position.v[1];
        }

        void inflate(const T horizontalAmount,
                     const T verticalAmount) noexcept
        {
            position.v[0] -= horizontalAmount;
            position.v[1] -= verticalAmount;
            size.v[0] += horizontalAmount * T(2);
            size.v[1] += verticalAmount * T(2);
        }

        [[nodiscard]] constexpr bool operator==(const Rect& other) const noexcept
        {
            return position.v[0] == other.position.v[0] && size.v[0] == other.size.v[0] &&
                position.v[1] == other.position.v[1] && size.v[1] == other.size.v[1];
        }

        [[nodiscard]] constexpr bool operator!=(const Rect& other) const noexcept
        {
            return position.v[0] != other.position.v[0] || size.v[0] != other.size.v[0] ||
                position.v[1] != other.position.v[1] || size.v[1] != other.size.v[1];
        }

        [[nodiscard]] constexpr const Rect operator*(const T scalar) const noexcept
        {
            return Rect{
                position.v[0] * scalar, position.v[1] * scalar,
                size.v[0] * scalar, size.v[1] * scalar
            };
        }

        Rect& operator*=(const T scalar) noexcept
        {
            position.v[0] *= scalar;
            position.v[1] *= scalar;
            size.v[0] *= scalar;
            size.v[1] *= scalar;
            return *this;
        }

        [[nodiscard]] constexpr const Rect operator/(const T scalar) const noexcept
        {
            return Rect{
                position.v[0] / scalar, position.v[1] / scalar,
                size.v[0] / scalar, size.v[1] / scalar
            };
        }

        Rect& operator/=(const T scalar) noexcept
        {
            position.v[0] /= scalar;
            position.v[1] /= scalar;
            size.v[0] /= scalar;
            size.v[1] /= scalar;
            return *this;
        }
    };
}

#endif
