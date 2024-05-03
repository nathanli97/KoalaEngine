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
#include <optional>
#include <shared_mutex>

#include "Core/ModuleInterface.h"
#include <unordered_map>
#include <string>

namespace Koala
{
    class Config : public IModule
    {
    public:
        KOALA_IMPLEMENT_SINGLETON(Config)
        bool Initialize_MainThread() override;
        bool Shutdown_MainThread() override;
        void Tick(float delta_time) override;
        bool Reload();
        bool Save();
        void PrintAllConfigurations() const;

        std::optional<std::string> GetSetting(std::string key, std::string defaultValue = "") const;
        // TODO: This should be replaced by CVar. Removed it after console variable is working.
        std::string GetSettingAndWriteDefault(std::string key, std::string defaultValue, bool bWriteIntoEngineConfig = false);
        void SetSetting(std::string key, std::string value, bool bWriteIntoEngineConfig = false);
    private:
        mutable std::shared_mutex globalConfigLock;
        bool bReadonlyMode = false;

        void LoadINI(std::ifstream& file, std::unordered_map<std::string, std::string> &out);
        void SaveINI(std::ofstream& file, const std::unordered_map<std::string, std::string> &config) const;

        std::unordered_map<std::string, std::string> engineConfigs, gameConfigs;
    };
}
