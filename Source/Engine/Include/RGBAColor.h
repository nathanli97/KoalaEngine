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
#include "Math/MathDefinations.h"

namespace Koala
{
    struct RGBAColor
    {
    public:
        constexpr RGBAColor(float inR, float inG, float inB, float inA)
            : r(inR), g(inG), b(inB), a(inA) {}
        constexpr RGBAColor(float r, float g, float b): RGBAColor(r, g, b, 1.0f) {}
        // Initialize color using hex.
        // If bHexHasAlpha == True, Hex is treated as 0xRRGGBBAA (RGBA)
        // If bHexHasAlpha == False, Hex is treated as 0x00RRGGBB (RGB)
        template <bool bHexHasAlpha = false>
        constexpr RGBAColor(uint32_t inHex)
        {
            if constexpr (bHexHasAlpha)
            {
                r = (float) ((inHex >> 24) & 0xFF);
                g = (float) ((inHex >> 16) & 0xFF);
                b = (float) ((inHex >> 8) & 0xFF);
                a = (float) (inHex & 0xFF);
            }
            else
            {
                r = (float) ((inHex >> 16) & 0xFF);
                g = (float) ((inHex >> 8) & 0xFF);
                b = (float) (inHex & 0xFF);
                a = 1;
            }
        }
        float r, g, b, a;
        explicit operator Vec4f() const {
            return Vec4f{r, g, b, a};
        }
        explicit operator Vec3f() const {
            return Vec3f{r, g, b};
        }
    };
    namespace Colors
    {
        constexpr RGBAColor Red{0xFF0000};
        constexpr RGBAColor Green{0x00FF00};
        constexpr RGBAColor Blue{0x0000FF};
        constexpr RGBAColor Black{0x000000};
        constexpr RGBAColor White{0xFFFFFF};
        constexpr RGBAColor Yellow{0xFFFF00};
        constexpr RGBAColor Cyan{0x00FFFF};
        constexpr RGBAColor Magenta{0xFF00FF};
        constexpr RGBAColor Silver{0xC0C0C0};
        constexpr RGBAColor Gray{0x808080};
        constexpr RGBAColor Maroon{0x800000};
        constexpr RGBAColor Olive{0x808000};
        constexpr RGBAColor Purple{0x800080};
        constexpr RGBAColor Teal{0x008080};
        constexpr RGBAColor Navy{0x000080};
    }
}
