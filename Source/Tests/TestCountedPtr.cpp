// Copyright 2023 Li Xingru
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the “Software”), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do
// so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS
// OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
// WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include <catch2/catch_test_macros.hpp>
#include <Core/CountedPtr.h>

TEST_CASE("Empty ptr", "[CountedPtr]") {
    using namespace Koala;

    ICountedPtr<int> ptr;
    REQUIRE(0 == 0);
}

TEST_CASE("Release case", "[CountedPtr]")
{
    using namespace Koala;

    struct MyStruct
    {
        bool &released;
        MyStruct(bool &in_released): released(in_released){}
        ~MyStruct() {released = true;}
    };

    bool isReleased = false;

    {
        ICountedPtr<MyStruct> ptr(new MyStruct(isReleased));
        REQUIRE(ptr.GetCounter() == 1);
        {
            ICountedPtr<MyStruct> ptr2 = ptr;
            REQUIRE(ptr.GetCounter() == 2);
            {
                ICountedPtr<MyStruct> ptr3 = ptr2;
                REQUIRE(ptr.GetCounter() == 3);
                {
                    ICountedPtr<MyStruct> ptr4 = ptr3;
                    REQUIRE(ptr.GetCounter() == 4);
                }
                REQUIRE(ptr.GetCounter() == 3);
                REQUIRE(isReleased == false);
            }
            REQUIRE(ptr.GetCounter() == 2);
            REQUIRE(isReleased == false);
        }
        REQUIRE(ptr.GetCounter() == 1);
        REQUIRE(isReleased == false);
    }

    REQUIRE(isReleased == true);
}

TEST_CASE("Memory check", "[CountedPtr]")
{
    using namespace Koala;

    struct MyStruct
    {
        int memory = 0x12345678;
        MyStruct() {}
        int GetInt() {return memory;}
    };
    
    {
        ICountedPtr<MyStruct> ptr(new MyStruct);
        REQUIRE(ptr.GetCounter() == 1);
        REQUIRE(ptr->GetInt() == 0x12345678);
        {
            ICountedPtr<MyStruct> ptr2 = ptr;
            REQUIRE(ptr.GetCounter() == 2);
            REQUIRE(ptr->GetInt() == 0x12345678);
            {
                ICountedPtr<MyStruct> ptr3 = ptr2;
                REQUIRE(ptr.GetCounter() == 3);
                REQUIRE(ptr->GetInt() == 0x12345678);
            }
            REQUIRE(ptr.GetCounter() == 2);
            REQUIRE(ptr->GetInt() == 0x12345678);
        }
        REQUIRE(ptr.GetCounter() == 1);
        REQUIRE(ptr->GetInt() == 0x12345678);

        ptr = nullptr;
        REQUIRE(ptr.GetCounter() == 0);
    }
}