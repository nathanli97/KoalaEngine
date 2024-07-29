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

namespace Koala
{
    class ISerializableObject;
    class Archive
    {
    public:
        virtual ~Archive() = default;
        NODISCARD virtual bool IsReading() const = 0;
        NODISCARD virtual bool IsWriting() const = 0;
        NODISCARD virtual bool IsCooking() const = 0;

        template <typename T>
        size_t Serialize(T &v) requires std::is_scalar_v<T>
        {
            return Serialize(&v, sizeof(T));
        }

        template <typename T>
        size_t Serialize(T *ptr) requires std::negation_v<std::is_pointer<T>>
        {
            static_assert(std::is_base_of_v<Archive, T>, "This pointer is not pointed to a serializable object.");
            return ptr->Serialize(*this);
        }

        template <typename T>
        auto operator<=>(T &&v)
        {
            return Serialize(std::forward<T>(v));
        }

        NODISCARD virtual size_t Serialize(void *, size_t) = 0;
    };
}