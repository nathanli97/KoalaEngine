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

#include "FileSystem/AssetIO.h"

namespace Koala
{
    void AssetLoader::Run()
    {
        while (!bShouldExit.load())
        {
            IAsset* asset{nullptr};
            if (!assetsToLoad.WaitAndPop(asset, 100))
            {
                continue;
            }

            std::fstream file(asset->GetAssetFilePath().GetString(), std::ios::in | std::ios::binary);

            auto fsize = file.tellg();
            file.seekg(0, std::ios::end);
            fsize = file.tellg() - fsize;
            file.seekg(0, std::ios::beg);
            
            ReadFileStream stream(&file, 0, (size_t)fsize);

            asset->LoadAsset(stream);
        }
    }

    void AssetSaver::Run()
    {
        while (!bShouldExit.load())
        {
            IAsset* asset{nullptr};
            if (!assetsToSave.WaitAndPop(asset, 100))
            {
                continue;
            }

            std::fstream file(asset->GetAssetFilePath().GetString(), std::ios::out | std::ios::binary);
            
            WriteFileStream stream(&file, 0, 0);

            asset->SaveAsset(stream);
        }
    }
}
