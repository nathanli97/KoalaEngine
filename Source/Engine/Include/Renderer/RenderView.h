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
#include "Camera.h"
#include "Core/SingletonInterface.h"
#include "Math/Rect.h"
#include "Projectors/Projector.h"

namespace Koala::Renderer
{
    class View
    {
    public:
        NODISCARD FORCEINLINE_DEBUGABLE bool CanRender() const
        {
            return IsEmpty() && !IsHidden();
        }
        NODISCARD FORCEINLINE_DEBUGABLE bool IsEmpty() const
        {
            return viewRect.IsEmpty();
        }

        FORCEINLINE_DEBUGABLE View& SetSize(int in_w, int in_h)
        {
            viewRect.max = viewRect.min + Int32Point(in_w, in_h);
            return *this;
        }
        FORCEINLINE_DEBUGABLE View& SetPosition(int in_x, int in_y)
        {
            auto size = GetViewSize();
            viewRect.min = Int32Point(in_x, in_y);
            viewRect.max = viewRect.min + size;
            return *this;
        }
        FORCEINLINE_DEBUGABLE View& Hide()
        {
            isHidden = false;
            return *this;
        }
        FORCEINLINE_DEBUGABLE View& Show()
        {
            isHidden = true;
            return *this;
        }
        NODISCARD FORCEINLINE_DEBUGABLE bool IsHidden() const {return isHidden;}
        FORCEINLINE_DEBUGABLE Int32Point GetViewSize() {
            return viewRect.max - viewRect.min;
        }
    private:
        // The size of RenderTarget.
        Int32Rect viewRect;
        // Is this view hidden?
        bool isHidden = false;
    };
}
