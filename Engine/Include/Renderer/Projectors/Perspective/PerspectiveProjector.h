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
#include <Math.h>

namespace Koala::Renderer {

    struct PerspectiveProjector{
        /**
         * The functions used to calculate the perspective projection matrix.
         * _{LH,RH}NDC_ means this matrix is based on LeftHand or RightHand coordinate.
         * Half/Full Z means this matrix will map the z axis to [-1,1](Full) or [0,1](Half)
         * _ReservedZ version of the functions will remap the z to 1-z for depth precision consideration.
         * Common params:
         * @param out the final perspective projection matrix output.
         * @param fov the field of view angle in rad
         * @param aspect the aspect ratio for display. e.g. 16/9
         * @param z_near the near plane used in clipping
         * @param z_far the far plane used in clipping
         */

        static void CalculatePerspectiveProjMatrix_LHNDC_HalfZ(Mat4f &out, float fov, float aspect, float z_near, float z_far);
        static void CalculatePerspectiveProjMatrix_RHNDC_HalfZ(Mat4f &out, float fov, float aspect, float z_near, float z_far);
        static void CalculatePerspectiveProjMatrix_LHNDC_FullZ(Mat4f &out, float fov, float aspect, float z_near, float z_far);
        static void CalculatePerspectiveProjMatrix_RHNDC_FullZ(Mat4f &out, float fov, float aspect, float z_near, float z_far);

        static void CalculatePerspectiveProjMatrix_LHNDC_HalfZ_ReversedZ(Mat4f &out, float fov, float aspect, float z_near, float z_far);
        static void CalculatePerspectiveProjMatrix_RHNDC_HalfZ_ReversedZ(Mat4f &out, float fov, float aspect, float z_near, float z_far);
        static void CalculatePerspectiveProjMatrix_LHNDC_FullZ_ReversedZ(Mat4f &out, float fov, float aspect, float z_near, float z_far);
        static void CalculatePerspectiveProjMatrix_RHNDC_FullZ_ReversedZ(Mat4f &out, float fov, float aspect, float z_near, float z_far);
    };
};
