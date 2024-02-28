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

#include "Definations.h"
#include <cmath>
#include <Eigen/Dense>



namespace Koala {
    FORCEINLINE bool IsNearlyZero(float x)
    {
        return ::fabs(x) <= 0.0001f;
    }

    FORCEINLINE bool IsNearlyEqual(float a, float b)
    {
        return IsNearlyZero(a - b);
    }

    typedef Eigen::Matrix3f    Mat3f;
    typedef Eigen::Matrix3d    Mat3d;
    typedef Eigen::Matrix4f    Mat4f;
    typedef Eigen::Matrix4d    Mat4d;
    typedef Eigen::Vector3f    Vec3f;
    typedef Eigen::Vector3d    Vec3d;
    typedef Eigen::Vector4f    Vec4f;
    typedef Eigen::Vector4d    Vec4d;
    typedef Eigen::Quaternionf Quat;
}
