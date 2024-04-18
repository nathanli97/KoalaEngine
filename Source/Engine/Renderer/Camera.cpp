// Copyright 2023 Li Xingru
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the “Software”), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <Renderer/Camera.h>

namespace Koala::Renderer
{
    void Camera::Update() const
    {
        if (IsProjectionDirty())
        {
            projector->comm_param.far = far;
            projector->comm_param.near = near;

            if (camera_mode == ECameraMode::OrthographicCamera)
            {
                projector->proj_param.ortho.bottom = bottom;
                projector->proj_param.ortho.top = top;
                projector->proj_param.ortho.left = left;
                projector->proj_param.ortho.right = right;
            }
            else
            {
                projector->proj_param.persp.fov = fov;
                projector->proj_param.persp.aspect = aspect;
            }

            projector->MarkDirty();
            projector->UpdateProjection();
        }
    }
}
