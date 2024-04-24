#include <iostream>
#define PY_SSIZE_T_CLEAN

#include "Engine.h"
#include <Core/CountedRef.h>

int main(int argc, char** argv)
{
    struct Test
    {
        ~Test()
        {
            std::cout << "UNDef!" << std::endl;
        }
        Test() {}
    };
    auto ptr = new Test;
    
    {
        Koala::CountedRef<Test> ptr2 = ptr;
        Koala::CountedRef<Test> ptr3 = ptr2;
    }
    
    Koala::Engine::Start(argc, argv);
    return 0;
}
