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

namespace Koala {
    class Engine
    {
    public:
        static bool Start(int argc, char** argv)
        {
            if (!Engine::Get().Initialize(argc, argv))
            {
                return false;
            }

            while(!Engine::Get().IsEngineExitRequested())
                Engine::Get().Tick();

            Engine::Get().Shutdown();
            return true;
        }
        static Engine& Get()
        {
            static Engine engine;
            return engine;
        }
        [[nodiscard]] bool IsEngineExitRequested() const
        {
            return requested_engine_stop;
        }

        void RequestEngineStop()
        {
            requested_engine_stop = true;
        }
    private:
        bool Initialize(int argc, char** argv);
        void Tick();
        void Shutdown();

        // TODO: This flag variable may be need to protect by LOCK
        bool requested_engine_stop = false;
    };
}