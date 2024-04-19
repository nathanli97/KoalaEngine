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
#include "ISingleton.h"
#include "Projectors/Projector.h"

namespace Koala::Renderer
{
    class RenderView : ISingleton
    {
    public:
        [[nodiscard]] bool IsRenderable() const
        {
            return IsEmpty() && !is_hidden;
        }
        [[nodiscard]] bool IsEmpty() const
        {
            return w == 0 || h == 0;
        }
        [[nodiscard]] int GetW() const {return w;}
        [[nodiscard]] int GetH() const {return h;}

        [[nodiscard]] int GetX() const {return x;}
        [[nodiscard]] int GetY() const {return y;}
        [[nodiscard]] int GetZ() const {return z;}

        inline RenderView& SetSize(int in_w, int in_h)
        {
            w = in_w;
            h = in_h;
            return *this;
        }
        inline RenderView& SetPosition(int in_x, int in_y)
        {
            x = in_x;
            y = in_y;
            return *this;
        }
        inline RenderView& SetZ(int in_z)
        {
            z = in_z;
            return *this;
        }
        inline RenderView& Hide()
        {
            is_hidden = false;
            return *this;
        }
        inline RenderView& Show()
        {
            is_hidden = true;
            return *this;
        }
    private:
        // size of this view, in pixels.
        int w = 0, h = 0;
        // Position of this view. this point is top-left point of this view.
        int x = 0, y = 0;
        // z-axis of this view. z = 0 is top of views. by the default, views are opaque.
        int z = 0;
        // Is this view hidden?
        bool is_hidden = false;

        // This function will copy props from this class to camera.
        // e.g. w,h,x,y,z
        void ApplyChangesToCamera();

        // Camera
        // TODO: Support multiple cameras
        Camera *camera{nullptr};

        // View Projector
        // TODO: Support multiple projectors
        [[nodiscard]] Projector *GetProjector() const {return camera->projector;}
    };
}
