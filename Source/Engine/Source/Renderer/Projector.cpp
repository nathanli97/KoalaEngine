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

#include <Renderer/Projectors/Projector.h>
#include <Renderer/Projectors/Orthographic/OrthographicProjector.h>
#include <Renderer/Projectors/Perspective/PerspectiveProjector.h>

namespace Koala::Renderer {
    void SceneProjector::CalculateProjectionMatrix()
    {

        switch (projectorType)
        {
        case EProjectorType::NoProjectorType:
            matProjection.setIdentity();
            break;
        case EProjectorType::OrthographicProjectorType:
            OrthographicProjector::CalculateProjMatrix(matProjection,
                commParam.nearPlane, commParam.farPlane,
                projParam.ortho.left, projParam.ortho.right, projParam.ortho.bottom, projParam.ortho.top);
            break;
        case EProjectorType::PerspectiveProjectorType:
            PerspectiveProjector::CalculateProjMatrix_LHNDC_FullZ(matProjection,
                projParam.persp.fov, projParam.persp.aspect,
                commParam.nearPlane, commParam.farPlane);
            break;
        default:
            ASSERT(projectorType < ProjectorTypeNum);
        }
        bMatrixDirty = false;
    }

    void SceneProjector::UpdateProjection()
    {
        if (bMatrixDirty)
        {
            CalculateProjectionMatrix();
        }
    }

    const Mat4f& SceneProjector::GetProjectionMatrix() const
    {
        return matProjection;
    }

    const Mat4f& SceneProjector::AcquireProjectionMatrix()
    {
        UpdateProjection();
        return matProjection;
    }

    void PerspectiveProjector::CalculateProjMatrix_LHNDC_FullZ(Mat4f& out, float fov, float aspect, float zNear, float zFar)
    {
        float fov_sin = ::sinf(fov / 2.0f);
        float fov_cos = ::cosf(fov / 2.0f);
        float fov_cot = fov_cos / fov_sin;

        out.setZero();

        out(0,0) = fov_cot / aspect;
        out(1,1) = fov_cot;

        float delta_z = zFar - zNear;
        float v22 = -(zFar + zNear) / delta_z;
        float v23 = -(2 * zFar * zNear) / delta_z;

        if (zFar == 0) {
            v22 = 1;
            v23 = -2 * zNear;
        }

        out(2,2) = v22;
        out(2,3) = v23;
        out(3,2) = -1;
    }

    void PerspectiveProjector::CalculateProjMatrix_LHNDC_HalfZ(Mat4f& out, float fov, float aspect, float zNear, float zFar)
    {
        // TODO: Handle infinity zFar
        float fov_sin = ::sinf(fov / 2.0f);
        float fov_cos = ::cosf(fov / 2.0f);
        float fov_cot = fov_cos / fov_sin;

        out.setZero();

        out(0,0) = fov_cot / aspect;
        out(1,1) = fov_cot;

        float delta_z = zFar - zNear;
        float v22 = -(zFar) / delta_z;
        float v23 = -(zFar * zNear) / delta_z;

        out(2,2) = v22;
        out(2,3) = v23;
        out(3,2) = -1;
    }


    void OrthographicProjector::CalculateProjMatrix(Mat4f &out,
        float near, float far, float left, float right, float bottom, float top) {

    }

}