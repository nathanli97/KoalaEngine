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
#include <optional>

#include "Core/Module.h"
#include <unordered_map>
#include <string>

namespace Koala
{
    class Config : public IModule
    {
    public:
        bool Initialize() override;
        bool Shutdown() override;
        void Tick(float delta_time) override;
        bool Reload();
        bool Save();

        bool HasAutoSaving() const
        {
            return auto_saving;
        }

        void SetAutoSaving(bool enabled)
        {
            auto_saving = enabled;
        }

        bool HasAutoSavingWhenEngineExiting() const
        {
            return saving_when_engine_exiting;
        }

        void SetAutoSavingWhenEngineExiting(bool enabled)
        {
            saving_when_engine_exiting = enabled;
        }

        void PrintAllConfigurations() const;

        std::optional<std::string> GetSettingStr(std::string key, std::string default_value = "") const;
        std::string GetSettingStrWithAutoSaving(std::string key, std::string default_value, bool write_into_engine_config = false);
        void SetSettingStr(std::string key, std::string value, bool write_into_engine_config = false);
    private:
        mutable std::mutex global_config_lock;
        bool readonly_mode = false;

        void LoadINI(std::ifstream& file, std::unordered_map<std::string, std::string> &out);
        void SaveINI(std::ofstream& file, const std::unordered_map<std::string, std::string> &config) const;

        std::unordered_map<std::string, std::string> engine_configs, game_configs;
        bool auto_saving = false;
        bool saving_when_engine_exiting = false;

        // void OverrideSettingWithCommandLine();
    };
}
