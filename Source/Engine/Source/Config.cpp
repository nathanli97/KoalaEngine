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
#include "Config.h"

#include <filesystem>

#include "FileSystem/SystemPath.h"

#include <fstream>
#include <regex>
#include <spdlog/spdlog.h>

#include "CmdParser.h"
#include "Core/StringTools.h"

namespace Koala
{
    bool Config::Initialize()
    {
        if (!std::filesystem::exists(Path::GetConfigPath()))
        {
            if (!std::filesystem::create_directories(Path::GetConfigPath()))
            {
                spdlog::warn("Failed to create config dir. All settings will be enter readonly mode");
                readonly_mode = true;
                return true;
            }
        }

        if (!Reload())
        {
            spdlog::warn("Failed to load settings.");
        }
        return true;
    }

    bool Config::Shutdown()
    {
        if (HasAutoSavingWhenEngineExiting())
        {
            if(!Save())
            {
                spdlog::error("Failed to save settings.");
            }
        }

        return true;
    }

    std::optional<std::string> Config::GetSettingStr(std::string key, std::string default_value) const
    {
        std::optional<std::string> result;
        std::lock_guard<std::mutex> lock_guard(global_config_lock);
        if (engine_configs.count(key) != 0)
        {
            result = engine_configs.at(key);
        }
        if (game_configs.count(key) != 0)
        {
            result = game_configs.at(key);
        }

        if (!result.has_value() && !default_value.empty())
        {
            result = default_value;
        }
        return result;
    }

    std::string Config::GetSettingStrWithAutoSaving(std::string key, std::string default_value, bool write_into_engine_config)
    {
        auto value = GetSettingStr(key);
        if (value.has_value())
            return value.value();
        else
        {
            SetSettingStr(key, default_value, write_into_engine_config);
            return default_value;
        }
    }

    void Config::SetSettingStr(std::string key, std::string value, bool write_into_engine_config)
    {
        std::lock_guard<std::mutex> lock_guard(global_config_lock);
        if (write_into_engine_config)
        {
            engine_configs[key] = value;
        }
        else
        {
            game_configs[key] = value;
        }
    }

    void Config::Tick(float delta_time)
    {
        if (auto_saving)
            Save();
    }

    bool Config::Reload()
    {
        bool result = true;
        auto path_engine_configfile = Path::GetConfigPath() / "EngineConfig.ini";
        auto path_game_configfile = Path::GetConfigPath() / "GameConfig.ini";

        std::ifstream file_engine_config(path_engine_configfile);
        std::ifstream file_game_config(path_game_configfile);

        if (!file_engine_config.is_open())
        {
            spdlog::error("Failed to load engine configuration from file {}", path_engine_configfile.string());
            result = false;
        }
        else
        {
            LoadINI(file_engine_config, engine_configs);
        }

        if (!file_game_config.is_open())
        {
            spdlog::error("Failed to load game configuration from file {}", path_game_configfile.string());
            result = false;
        }
        else
        {
            LoadINI(file_game_config, game_configs);
        }

        if (result && readonly_mode)
        {
            readonly_mode = false;
        }

        return result;
    }

    bool Config::Save()
    {
        auto path_engine_configfile = Path::GetConfigPath() / "EngineConfig.ini";
        auto path_game_configfile = Path::GetConfigPath() / "GameConfig.ini";

        std::ofstream file_engine_config(path_engine_configfile, std::ios::trunc);
        std::ofstream file_game_config(path_game_configfile, std::ios::trunc);

        if (!file_engine_config.is_open() || !file_game_config.is_open())
        {
            spdlog::error("Failed to save configurations");
            return false;
        }

        SaveINI(file_engine_config, engine_configs);
        SaveINI(file_game_config, game_configs);

        return true;
    }

    void Config::LoadINI(std::ifstream& file, std::unordered_map<std::string, std::string>& out)
    {
        std::lock_guard<std::mutex> lock_guard(global_config_lock);
        assert(file.is_open());
        std::string line;

        const static auto remove_blank_lines = [](std::string &str)
        {
            while (
                !str.empty() && (
                StringTool::startswith(str, "\n") ||
                StringTool::startswith(str, "\r")
                ))
                str = str.substr(1);

            while (
                !str.empty() && (
                StringTool::endswith(str, "\n") ||
                StringTool::endswith(str, "\r")
                ))
                str = str.substr(0, str.length() - 2);
        };

        while (std::getline(file, line))
        {
            remove_blank_lines(line);
            if (
                StringTool::startswith(line, "#") ||
                StringTool::startswith(line, "[")
                )
            continue;

            auto idx = line.find('#');
            if (idx != std::string::npos)
            {
                line = line.substr(0, idx);
            }

            std::string key, value;

            const static auto remove_blanks = [](std::string &str)
            {
                while (
                    !str.empty() && (
                    StringTool::startswith(str, " ") ||
                    StringTool::startswith(str, "\t")
                    ))
                    str = str.substr(1);

                while (
                    !str.empty() && (
                    StringTool::endswith(str, " ") ||
                    StringTool::endswith(str, "\t")
                    ))
                    str = str.substr(0, str.length() - 1);
            };

            {
                const static std::regex re("([^=]+)=([^=]+)");
                std::smatch cm;
                if (!std::regex_match(line, cm, re))
                    continue;

                key = cm[1];
                value = cm[2];

                remove_blanks(key);
                remove_blanks(value);
            }

            if (out.count(key) != 0)
            {
                spdlog::warn("Configuration Reload: Key already exists: {}={}, now overriding with new value {}", key, out[key], value);
            }

            out[key] = value;
        }
    }

    void Config::SaveINI(std::ofstream& file, const std::unordered_map<std::string, std::string>& config) const
    {
        std::lock_guard<std::mutex> lock_guard(global_config_lock);
        for (auto pair: config)
        {
            std::string line = pair.first + " = " + pair.second;
            file << line << "\n";
        }
    }

    void Config::PrintAllConfigurations() const
    {
        std::lock_guard<std::mutex> lock_guard(global_config_lock);
        spdlog::warn("Printing all configurations!");
        for (auto const &config: engine_configs)
        {
            spdlog::warn("[ENG] {}={}", config.first, config.second);
        }

        for (auto const &config: game_configs)
        {
            spdlog::warn("[GAME] {}={}", config.first, config.second);
        }

        spdlog::warn("Print all configurations END!");

    }

    // void Config::OverrideSettingWithCommandLine()
    // {
    //     if (CmdParser::Get().HasArg("cfg"))
    //     {
    //         auto cfg_override = CmdParser::Get().GetArgStr("cfg");
    //         const static std::regex regex("");
    //     }
    // }
}
