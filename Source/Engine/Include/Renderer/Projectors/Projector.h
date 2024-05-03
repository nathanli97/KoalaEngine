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
    enum EProjectorType {
        NoProjectorType = 0,             // Just Identity proj matrix.
        OrthographicProjectorType,
        PerspectiveProjectorType,
        ProjectorTypeNum,
    };

    class SceneProjector {
    public:
        // Update Projection Matrix if needed.
        void UpdateProjection();

        // Params
        // The params shared on multiple projection methods.
        struct {
            float nearPlane, farPlane;
        } commParam;

        // Proj method-specified projection params.
        union {
            struct NoProject{};
            struct {
                float fov, aspect;
            } persp;
            struct {
                float left, right, bottom, top;
            } ortho;
        } projParam;

        // Get Projection Matrix.
        NODISCARD FORCEINLINE const Mat4f& GetProjectionMatrix() const;
        // Update proj matrix if needed. then, return result matrix.
        const Mat4f& AcquireProjectionMatrix();

        NODISCARD FORCEINLINE bool IsProjectorTypeValid() const {return projectorType < EProjectorType::ProjectorTypeNum;}
        NODISCARD FORCEINLINE EProjectorType GetProjectType() const {return projectorType;}
        FORCEINLINE void SetProjectType(const EProjectorType type)
        {
            projectorType = type;
            MarkDirty();
        }
        FORCEINLINE void MarkDirty()
        {
            bMatrixDirty = true;
        }
    private:
        Mat4f matProjection;

        EProjectorType projectorType{ProjectorTypeNum};
        bool bMatrixDirty{true};

        FORCEINLINE void CalculateProjectionMatrix();
    };
}
