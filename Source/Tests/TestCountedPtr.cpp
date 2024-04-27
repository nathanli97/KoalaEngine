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

#include <functional>
#include <catch2/catch_test_macros.hpp>
#include <Core/CountedPtr.h>

#include <memory>
TEST_CASE("Empty ptr", "[CountedPtr]") {
    using namespace Koala;

    ICountedPtr<int> ptr;
    std::shared_ptr<int> sss;
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
        REQUIRE(ptr);
        {
            ICountedPtr<MyStruct> ptr2 = ptr;
            REQUIRE(ptr.GetCounter() == 2);
            REQUIRE(ptr);
            {
                ICountedPtr<MyStruct> ptr3 = ptr2;
                REQUIRE(ptr.GetCounter() == 3);
                REQUIRE(ptr);
                {
                    ICountedPtr<MyStruct> ptr4 = ptr3;
                    REQUIRE(ptr.GetCounter() == 4);
                    REQUIRE(ptr);
                }
                REQUIRE(ptr.GetCounter() == 3);
                REQUIRE(isReleased == false);
                REQUIRE(ptr);
            }
            REQUIRE(ptr.GetCounter() == 2);
            REQUIRE(isReleased == false);
            REQUIRE(ptr);
        }
        REQUIRE(ptr.GetCounter() == 1);
        REQUIRE(isReleased == false);
        REQUIRE(ptr);
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
        REQUIRE(!ptr);
    }
}

TEST_CASE("Custom dealloctor", "[CountedPtr]")
{
    using namespace Koala;
    int* ptr = (int*)::malloc(sizeof(int));
    *ptr = 0x12345678;

    bool isReleased = false;
    auto dealloctorFunc= [&isReleased, ptr](int* in_ptr)
    {
        REQUIRE(ptr == in_ptr);
        REQUIRE(isReleased == false); // NO double free
        ::free(in_ptr);
        isReleased = true;
    };

    {
        ICountedPtr<int, std::function<void(int*)>> my_ptr(ptr,dealloctorFunc);
    }

    REQUIRE(isReleased == true);
}

TEST_CASE("CountedPtr with sub-classes", "[CountedPtr]")
{
    using namespace Koala;

    struct Base
    {
        virtual int func()
        {
            return 0;
        }
    };
    struct Child: public Base
    {
        virtual int func_child()
        {
            return 1;
        }
    };
    struct Other {};

    auto ptr = MakeShared<Child>();
    ICountedPtr<Base> ptr_base = ptr;

    // Can be accessed!
    REQUIRE(ptr->func_child() == 1);

    // Can not be compiled!
    //ptr_base->func_child();

    // HA?
    // ptr = MakeShared<Other>();
    
    REQUIRE(0 == 0);
}