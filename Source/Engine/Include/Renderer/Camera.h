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

#include "Projectors/Projector.h"

namespace Koala::Renderer {
    enum ECameraMode
    {
        PerspectiveCamera,
        OrthographicCamera
    };
    class Camera {
    public:
        Vec3f position;
        Quatf orientation;

        // Perspective params
        // FOV angle. in rad. only used in perspective projection.
        float fov{0};
        float aspect{0};
        float nearPlane{0}, farPlane{0};

        // Orthographic params
        float bottom{0}, top{0}, left{0}, right{0};

        SceneProjector *projector;
        ECameraMode cameraMode{ECameraMode::OrthographicCamera};

        Camera():projector(new SceneProjector)
        {
            projector->SetProjectType(OrthographicProjectorType);
        }

        void Update() const;
        NODISCARD FORCEINLINE bool IsProjectionDirty() const
        {
            if (cameraMode == PerspectiveCamera)
            {
                if (projector->GetProjectType() != EProjectorType::PerspectiveProjectorType)
                {
                    projector->SetProjectType(EProjectorType::PerspectiveProjectorType);
                    return false;
                }

                return !IsNearlyEqual(farPlane, projector->commParam.farPlane) ||
                    !IsNearlyEqual(nearPlane, projector->commParam.nearPlane) ||
                    !IsNearlyEqual(fov, projector->projParam.persp.fov) ||
                    !IsNearlyEqual(aspect, projector->projParam.persp.aspect);
            } else // cameraMode == OrthographicCamera
            {
                if (projector->GetProjectType() != EProjectorType::OrthographicProjectorType)
                {
                    projector->SetProjectType(EProjectorType::OrthographicProjectorType);
                    return false;
                }

                return !IsNearlyEqual(farPlane, projector->commParam.farPlane) ||
                    !IsNearlyEqual(nearPlane, projector->commParam.nearPlane) ||
                    !IsNearlyEqual(bottom, projector->projParam.ortho.bottom) ||
                    !IsNearlyEqual(top, projector->projParam.ortho.top) ||
                    !IsNearlyEqual(left, projector->projParam.ortho.left) ||
                    !IsNearlyEqual(right, projector->projParam.ortho.right);
            }
        }
    };
}

