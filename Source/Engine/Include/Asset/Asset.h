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

#include "Core/HashedString.h"
#include "FileSystem/FileStream.h"

namespace Koala
{
    class IAsset
    {
    public:
        virtual ~IAsset() = default;
        // Load the asset. The implementation of LoadAsset should check
        // The input data is baked or not.
        virtual bool LoadAsset(FileIO::ReadFileStream &file) = 0;
        // Save Unbaked Asset. Only unbaked asset can be saved via this function.
        virtual bool SaveAssetUnbaked(FileIO::WriteFileStream &file) = 0;

        virtual bool IsHardcodedAsset() { return false; }
        virtual bool Initialize() { return true; }
        
        // Bake current unbaked data into baked format.
        // Default implementation is use the same format between baked and unbaked version of asset.
        // If asset's baked data format is different with unbaked version (e.g. Textures),
        // Override it.
        virtual bool Bake(FileIO::WriteFileStream &file)
        {
            return SaveAssetUnbaked(file);
        }
        virtual HashedString GetAssetFilePath() = 0;

        FORCEINLINE bool IsBakedData() const
        {
            return bBaked;
        }
    protected:
        bool bBaked{false};
    };
}
