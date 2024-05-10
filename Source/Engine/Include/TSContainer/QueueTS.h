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

        void Push(const Type& inValue)
        {
            std::unique_lock lock(mutex);
            const bool bIsEmpty = Super::empty();
            Super::push(inValue);

            lock.unlock();

            if (bIsEmpty)
                cv.notify_one();
        }

        void Push(Type&& inValue)
        {
            std::unique_lock lock(mutex);
            const bool bIsEmpty = Super::empty();
            Super::push(std::move(inValue));
            
            lock.unlock();
            
            if (bIsEmpty)
                cv.notify_one();
        }

        bool WaitAndPop(Type& outPop, int maxWaitMilliseconds = 1000)
        {
            std::unique_lock lock(mutex);

            if (Super::empty())
                cv.wait_for(lock, std::chrono::milliseconds(maxWaitMilliseconds));
            if (Super::empty())
                return false;

            outPop = std::move(Super::front());
            Super::pop();

            return true;
        }

        bool TryPop(Type& outPop)
        {
            std::unique_lock lock(mutex);

            if (Super::empty())
                return false;

            outPop = std::move(Super::front());
            Super::pop();

            return true;
        }

        bool IsEmpty() const
        {
            std::unique_lock lock(mutex);

            return Super::empty();
        }

        // Wait but do not pop
        bool Wait(int maxWaitMilliseconds = 1000)
        {
            std::unique_lock lock(mutex);

            if (Super::empty())
                cv.wait_for(lock, std::chrono::milliseconds(maxWaitMilliseconds));
            if (Super::empty())
                return false;

            return true;
        }
    private:
        mutable std::mutex mutex;
        std::condition_variable cv;
    };
}
