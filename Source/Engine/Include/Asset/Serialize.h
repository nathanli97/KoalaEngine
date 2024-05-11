﻿//Copyright 2024 Li Xingru
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
#include <vector>

namespace Koala
{
    class SerializeTarget;
    // Serialize Container 
    template <
        typename T,
        typename ElementType
    >
    void Serialize(SerializeTarget& file, std::vector<T> &data)
    {
        uint32_t count;
        Serialize<uint32_t>(file, count);

            
        if (file.IsLoading())
        {
            data.resize(count);
            for (uint32_t index = 0; index < count; index++)
            {
                    
            }
        }
    }

    // Serialize Scalar reference
    template <typename T>
    void Serialize(SerializeTarget&, T &data) requires std::is_scalar_v<T>
    {}

    // Serialize Scalar
    template <typename T>
    void Serialize(SerializeTarget&, T &&data) requires std::is_scalar_v<T>
    {}

    // Serialize Pointer to Any Asset
    template <typename T> requires std::disjunction_v<std::is_same<Asset, T>, std::is_base_of<Asset, T>>
    void Serialize(SerializeTarget&, T *data){}
}
