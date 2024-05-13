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

#include <fstream>

#include "Definations.h"
#include "Core/Check.h"

namespace Koala
{
    enum ESeekFrom
    {
        Begin,
        Current,
        End
    };
    class FileStream
    {
    public:
        FileStream(std::fstream * inFStream, size_t inOffset, size_t inFileSize):
            fstream(inFStream), offset(inOffset), fileSize(inFileSize)
        {}
        FORCEINLINE size_t Tell()
        {
            auto position = (size_t)fstream->tellg();
            check(position >= offset);
            return (size_t)fstream->tellg() - offset;
        }
        FORCEINLINE void Seek(ESeekFrom seekFrom, size_t inSeekWhere)
        {
            check(inSeekWhere >= offset && inSeekWhere < offset + fileSize);
            std::ios::seekdir seekmode;
            
            if (seekFrom == ESeekFrom::Begin)
                seekmode = std::ios::beg;
            else if (seekFrom == ESeekFrom::Current)
                seekmode = std::ios::cur;
            else
                seekmode = std::ios::end;
            
            fstream->seekg(inSeekWhere, seekmode);
        }
        size_t GetFileSize()
        {
            return fileSize;
        }
    protected:
        std::fstream* fstream{nullptr};
        size_t offset{0};
        size_t fileSize{0};
    };

    class ReadFileStream: public FileStream
    {
    public:
        using FileStream::FileStream;
        FORCEINLINE ReadFileStream& Read(void* inBuffer, size_t inSize)
        {
            fstream->read((char*)inBuffer, inSize);

            return *this;
        }
    };

    class WriteFileStream: public FileStream
    {
    public:
        using FileStream::FileStream;
        FORCEINLINE WriteFileStream& Write(void* inBuffer, size_t inSize)
        {
            fstream->write((char*)inBuffer, inSize);

            return *this;
        }
    };
}
