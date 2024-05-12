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

#include "Asset/Mesh.h"

constexpr uint32_t MeshFileMagicMask = 0x12341234;
constexpr uint32_t MeshFileCurrentVersion = 0x1;
namespace Koala
{
    Logger logger("MeshAsset");
    static size_t GetFileSize(std::ifstream& file)
    {
        auto currOffset = file.tellg();
        file.seekg( 0, std::ios::beg );
        auto fsize = file.tellg();
        file.seekg( 0, std::ios::end );
        fsize = file.tellg() - fsize;
        file.seekg( currOffset, std::ios::beg );
        return fsize;
    }
    
    struct AssetFileMetaData
    {
        uint32_t fileMagicMask {0};
        uint32_t fileVersion {0};
        uint64_t numVertices;
        uint64_t numIndices;
        uint64_t offsetIndices;
    };
    
    bool Mesh::LoadAsset(std::ifstream &file)
    {
        if (!file.is_open())
            return false;
        
        size_t fileSize = GetFileSize(file);
        if (fileSize < sizeof(AssetFileMetaData))
            return false;
        
        AssetFileMetaData metaData;
        file.read((char*)&metaData, sizeof(AssetFileMetaData));

        if (metaData.fileMagicMask != MeshFileMagicMask)
        {
            logger.error("This file is not a valid mesh asset file -- MagicMask mismatch!  {} vs {}", MeshFileMagicMask, metaData.fileMagicMask);
            return false;
        }

        if (metaData.fileVersion > MeshFileCurrentVersion)
        {
            logger.error("The engine version used to create this asset file is too new, Koala didn't support!");
            return false;
        } else if (metaData.fileVersion < MeshFileCurrentVersion)
        {
            logger.warning("This asset file is too old, may cause some problems!");
        }
        size_t remainingFileSize = fileSize - sizeof(AssetFileMetaData);

        {
            size_t verticesAreaSize = metaData.numVertices * sizeof(Vector);

            if (remainingFileSize < verticesAreaSize)
            {
                logger.error("File format error -- unable to parse vector data area");
                return false;
            }

            vertices.resize(metaData.numVertices);

            Vector* vecDataPtr = static_cast<Vector *>(vertices.data());

            for (uint64_t i = 0; i < metaData.numVertices; ++i)
            {
                file.read((char*)vecDataPtr, sizeof(Vector));
                vecDataPtr++;
            }
            remainingFileSize += verticesAreaSize;
        }

        {
            size_t indicesAreaSize = metaData.numIndices * sizeof(uint32_t);

            if (remainingFileSize < indicesAreaSize)
            {
                logger.error("File format error -- unable to parse vector data area");
                return false;
            }
            indices.resize(metaData.numIndices);
            uint32_t* indicesDataPtr = static_cast<uint32_t *>(indices.data());

            for (uint64_t i = 0; i < metaData.numIndices; ++i)
            {
                file.read((char*)indicesDataPtr, sizeof(Vector));
                indicesDataPtr++;
            }

            remainingFileSize += indicesAreaSize;
        }

        return true;
    }
}
