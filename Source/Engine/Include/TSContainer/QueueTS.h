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
#include <mutex>
#include <queue>

namespace Koala
{
    // Thread-Safe version of Queue.
    template <typename Type>
    class QueueTS: private std::queue<Type>
    {
    public:
        typedef std::queue<Type> Super;

        template <typename T>
        void Push(T&& inValue)
        {
            std::unique_lock lock(mutex);
            const bool bIsEmpty = Super::empty();
            Super::push(std::forward<T>(inValue));
            
            lock.unlock();
            
            if (bIsEmpty)
                cv.notify_one();
        }

        void WaitAndPop(Type& outPop)
        {
            std::unique_lock lock(mutex);

            while (Super::empty())
                cv.wait(lock);

            outPop = Super::front();
            Super::pop();
        }

        bool TryPop(Type& outPop)
        {
            std::unique_lock lock(mutex);

            if (Super::empty())
                return false;

            outPop = Super::front();
            Super::pop();

            return true;
        }
    private:
        std::mutex mutex;
        std::condition_variable cv;
    };
}
