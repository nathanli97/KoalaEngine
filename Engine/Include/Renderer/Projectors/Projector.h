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


namespace Koala::Renderer {
    enum EProjectorType {
        NoProjector = 0,             // Just Identity proj matrix.
        OrthographicProjector,
        PerspectiveProjector,
        ProjectorTypeNum,
    };

    class Projector {
    public:
        // Update Projection Matrix if needed.
        void UpdateProjection();

        // Params
        // The params shared on multiple projection methods.
        struct {
        } comm_param;

        // Proj method-specified projection params.
        union {
            struct NoProject{};
            struct PerspectiveProject{};
            struct OrthographicProject{};
        } proj_param;

        // Get Projection Matrix.
        const Mat4f& GetProjectionMatrix() const;
        // Update proj matrix if needed. then, return result matrix.
        const Mat4f& AcquireProjectionMatrix() const;

        FORCEINLINE NODISCARD bool IsProjectorTypeValid() const {return projector_type < EProjectorType::ProjectorTypeNum;}
        FORCEINLINE NODISCARD EProjectorType GetProjectType() const {return projector_type;}
    private:
        Mat4f project_matrix;
        void CalculateProjectionMatrix();

        EProjectorType projector_type{ProjectorTypeNum};
    };
}
