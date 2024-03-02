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
#include <KoalaMath.h>

namespace Koala::Renderer {
    struct PerspectiveProjector{
        /**
         * The functions used to calculate the perspective projection matrix.
         * Assumes the world coordinate is RH-based coordinate system.
         * params:
         * @param out the final perspective projection matrix output.
         * @param fov the field of view angle in rad
         * @param aspect the aspect ratio for display. e.g. 16/9
         * @param z_near the near plane used in clipping
         * @param z_far the far plane used in clipping
         *
         *
         * @tparam is_left_hand_ndc this matrix is based on LeftHand(TRUE) or RightHand(FALSE) NDC coordinate.
         * @tparam is_full_z this matrix will map the z axis to [-1,1](Full) or [0,1](Half).
         * @tparam is_reserved_z ReservedZ version of the proj function will remap the z to 1-z for depth precision consideration.
         */

        static void CalculateProjMatrix_LHNDC_FullZ(Mat4f &out, float fov, float aspect, float z_near, float z_far);
        static void CalculateProjMatrix_LHNDC_HalfZ(Mat4f &out, float fov, float aspect, float z_near, float z_far);

        // template <bool is_left_hand_ndc = true, bool is_full_z = true, bool is_reserved_z = false, bool is_endless_far = false>
        // static void CalculateProjMatrix(
        //     Mat4f &out, float fov, float aspect, float z_near, float z_far
        // )
        // {
        //     // TODO: handle is_reserved_z
        //     float fov_sin = ::sinf(fov / 2.0f);
        //     float fov_cos = ::cosf(fov / 2.0f);
        //     float fov_cot = fov_cos / fov_sin;
        //
        //     out.setZero();
        //
        //     out(0,0) = fov_cot / aspect;
        //     out(1,1) = fov_cot;
        //
        //     float delta_z = z_far - z_near;
        //     float v22 = 0;
        //     float v23 = 0;
        //
        //     if constexpr (!is_full_z) {
        //         if constexpr (is_endless_far) {
        //             v22 = 1;
        //             // TODO: endless_far mode in half-z
        //             // v23 = -2 * z_near;
        //         } else {
        //             v22 = -z_far / delta_z;
        //             v23 = (z_far * z_near) / delta_z;
        //         }
        //     } else {
        //         if constexpr (is_endless_far) {
        //             v22 = 1;
        //             v23 = -2 * z_near;
        //         } else {
        //             v22 = -(z_far + z_near) / delta_z;
        //             v23 = -(2 * z_far * z_near) / delta_z;
        //         }
        //     }
        //
        //     if constexpr (!is_left_hand_ndc) {
        //         v22 = -v22;
        //     }
        //
        //     out(2,2) = v22;
        //     out(2,3) = v23;
        //     out(3,2) = -1;
        // }
    };
};
