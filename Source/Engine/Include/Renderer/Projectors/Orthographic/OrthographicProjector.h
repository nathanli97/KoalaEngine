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
#include <Math/MathDefinations.h>

namespace Koala::Renderer {
    /**
         * The functions used to calculate the orthographic projection matrix.
         * Assumes the world coordinate is RH-based coordinate system.
         * Required params:
         * @param out the final orthographic projection matrix output.
         * @param near the near plane
         * @param far the far plane
         * @param left the left plane
         * @param right the right plane
         * @param bottom the bottom plane
         * @param top the top plane
         */
    struct OrthographicProjector{
        static void CalculateProjMatrix(Mat4f &out,
            float near, float far, float left, float right, float bottom, float top);
    };
};
