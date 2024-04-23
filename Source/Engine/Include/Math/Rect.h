//Copyright 2024 Li Xingru
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
//associated documentation files (the “Software”), to deal in the Software without restriction,
//including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
//and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
//so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial
//portions of the Software.
//
//THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
//FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
//OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
//WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
//CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once
#include <cstdint>

namespace Koala {
    template <typename T>
    struct IntPoint {
        typedef T Type;
        T x, y;
        IntPoint(): x(0), y(0){}
        IntPoint(T in_x, T in_y): x(in_x), y(in_y) {}

        bool operator==(const IntPoint& rhs) {
            return x == rhs.x && y == rhs.y;
        }
        bool operator!=(const IntPoint& rhs) { return !operator==(rhs); }

        // Scaling
        IntPoint& operator*=(Type scale) {
            x *= scale;
            y *= scale;
            return *this;
        }
        IntPoint operator*(Type scale) {
            return IntPoint(x * scale, y * scale);
        }

        // Div
        IntPoint& operator/=(Type div) {
            x /= div;
            y /= div;
            return *this;
        }
        IntPoint operator/(Type div) {
            return IntPoint(x / div, y / div);
        }

        IntPoint& operator+=(IntPoint rhs) {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }
        IntPoint operator+(IntPoint rhs) {
            return IntPoint(x + rhs.x, y + rhs.y);
        }

        IntPoint& operator-=(IntPoint rhs) {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }
        IntPoint operator-(IntPoint rhs) {
            return IntPoint(x - rhs.x, y - rhs.y);
        }
        NODISCARD FORCEINLINE_DEBUGABLE bool IsEmpty () const { return x == 0 && y == 0; }
    };
    template <typename T>
    struct IntRect {
        typedef T Type;

        IntPoint<T> min, max;
        IntRect() = default;
        IntRect(T x0, T x1, T y0, T y1): min(x0, y0), max(x1, y1) {}
        IntRect(IntPoint<T> in_min, IntPoint<T> in_max): min(in_min), max(in_max) {}

        bool operator==(const IntRect& rhs) {
            return min == rhs.min && max == rhs.max;
        }
        bool operator!=(const IntRect& rhs) { return !operator==(rhs); }

        // Scaling
        IntRect& operator*=(Type scale) {
            min *= scale;
            max *= scale;
            return *this;
        }
        IntRect operator*(Type scale) {
            return IntRect(min * scale, max* scale);
        }

        // Div
        IntRect& operator/=(Type div) {
            min /= div;
            max /= div;
            return *this;
        }
        IntRect operator/(Type div) {
            return IntPoint(min / div, max / div);
        }

        Type Height () const { return max.y - max.y; }
        Type Width () const { return max.x - max.x; }
        Type Area () const { return Height() * Width(); }
        NODISCARD FORCEINLINE_DEBUGABLE bool IsEmpty () const { return min.IsEmpty() && max.IsEmpty(); }
    };
    typedef IntPoint<int8_t> Int8Point;
    typedef IntPoint<uint8_t> UInt8Point;
    typedef IntPoint<int16_t> Int16Point;
    typedef IntPoint<uint16_t> UInt16Point;
    typedef IntPoint<int32_t> Int32Point;
    typedef IntPoint<uint32_t> UInt32Point;
    typedef IntPoint<int64_t> Int64Point;
    typedef IntPoint<uint64_t> UInt64Point;

    typedef IntRect<int8_t> Int8Rect;
    typedef IntRect<uint8_t> UInt8Rect;
    typedef IntRect<int16_t> Int16Rect;
    typedef IntRect<uint16_t> UInt16Rect;
    typedef IntRect<int32_t> Int32Rect;
    typedef IntRect<uint32_t> UInt32Rect;
    typedef IntRect<int64_t> Int64Rect;
    typedef IntRect<uint64_t> UInt64Rect;
}
