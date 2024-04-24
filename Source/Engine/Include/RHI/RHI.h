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
#include <string>
#include <unordered_set>

#include "Core/Singleton.h"


namespace Koala
{
    struct RHITextureCreateInfo;
}

namespace Koala::RHI
{
    struct IRenderHardware: public ISingleton{
        virtual ~IRenderHardware() = default;


        // The following functions will be called on RenderThread.
        // Caller: RenderThread
        virtual bool Initialize_RenderThread() = 0;
        // Caller: RenderThread
        virtual void Shutdown_RenderThread() = 0;


        // The following functions will be called on MainThread.
        // This function will be called before RT is started.
        // Caller: MainThread
        virtual bool PreInit_MainThread() = 0;
        // This function will be called after RT is stopped.
        // Caller: MainThread
        virtual void PostShutdown_MainThread() = 0;
        // RETURN FALSE if window is closing, renderer should stop rendering
        // Caller: MainThread
        virtual bool Tick_MainThread() = 0;

        // The following functions only can be called on RenderThread.
        // Return the GPU using for rendering.
        // Return NULL if none of GPU is suitable or not choosed yet.
        virtual const char* GetGPUName() = 0;
        virtual const void* CreateTexture(const char* debugName, const RHITextureCreateInfo& info) = 0;
    };
    std::unordered_set<std::string> GetAvaliableRHIs();
    IRenderHardware* GetRHI(const std::string&);
}

