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
#include "Core/KoalaLogger.h"

namespace Koala
{
    Logger loggerConfig("Configuration");
    bool Config::Initialize_MainThread()
    {
        if (!std::filesystem::exists(Path::GetConfigPath()))
        {
            if (!std::filesystem::create_directories(Path::GetConfigPath()))
            {
                loggerConfig.warning("Failed to create config dir. All settings will be enter readonly mode");
                bReadonlyMode = true;
                return true;
            }
        }

        if (!Reload())
        {
            loggerConfig.warning("Failed to load settings.");
        }
        return true;
    }

    bool Config::Shutdown_MainThread()
    {
        return true;
    }

    std::optional<std::string> Config::GetSetting(std::string key, std::string defaultValue) const
    {
        std::optional<std::string> result;
        std::shared_lock lock(globalConfigLock);
        if (engineConfigs.count(key) != 0)
        {
            result = engineConfigs.at(key);
        }
        if (gameConfigs.count(key) != 0)
        {
            result = gameConfigs.at(key);
        }

        if (!result.has_value() && !defaultValue.empty())
        {
            result = defaultValue;
        }
        return result;
    }

    std::string Config::GetSettingAndWriteDefault(std::string key, std::string defaultValue, bool bWriteIntoEngineConfig)
    {
        auto value = GetSetting(key);
        if (value.has_value())
            return value.value();
        else
        {
            SetSetting(key, defaultValue, bWriteIntoEngineConfig);
            return defaultValue;
        }
    }

    void Config::SetSetting(std::string key, std::string value, bool bWriteIntoEngineConfig)
    {
        std::unique_lock lock_guard(globalConfigLock);
        if (bWriteIntoEngineConfig)
        {
            engineConfigs[key] = value;
        }
        else
        {
            gameConfigs[key] = value;
        }
    }

    void Config::Tick(float delta_time) {}

    bool Config::Reload()
    {
        bool result = true;
        auto pathEngineConfigfile = Path::GetConfigPath() / "EngineConfig.ini";
        auto pathGameConfigfile = Path::GetConfigPath() / "GameConfig.ini";

        std::ifstream file_engine_config(pathEngineConfigfile);
        std::ifstream file_game_config(pathGameConfigfile);

        if (!file_engine_config.is_open())
        {
            loggerConfig.error("Failed to load engine configuration from file {}", pathEngineConfigfile.string());
            result = false;
        }
        else
        {
            LoadINI(file_engine_config, engineConfigs);
        }

        if (!file_game_config.is_open())
        {
            loggerConfig.error("Failed to load game configuration from file {}", pathGameConfigfile.string());
            result = false;
        }
        else
        {
            LoadINI(file_game_config, gameConfigs);
        }

        if (result && bReadonlyMode)
        {
            bReadonlyMode = false;
        }

        return result;
    }

    bool Config::Save()
    {
        auto pathEngineConfigfile = Path::GetConfigPath() / "EngineConfig.ini";
        auto pathGameConfigfile = Path::GetConfigPath() / "GameConfig.ini";

        std::ofstream file_engine_config(pathEngineConfigfile, std::ios::trunc);
        std::ofstream file_game_config(pathGameConfigfile, std::ios::trunc);

        if (!file_engine_config.is_open() || !file_game_config.is_open())
        {
            loggerConfig.error("Failed to save configurations");
            return false;
        }

        SaveINI(file_engine_config, engineConfigs);
        SaveINI(file_game_config, gameConfigs);

        return true;
    }

    void Config::LoadINI(std::ifstream& file, std::unordered_map<std::string, std::string>& out)
    {
        std::unique_lock lock_guard(globalConfigLock);
        assert(file.is_open());
        std::string line;

        const static auto RemoveBlankLines = [](std::string &str)
        {
            while (
                !str.empty() && (
                str.starts_with("\n") ||
                str.starts_with("\r")
                ))
                str = str.substr(1);

            while (
                !str.empty() && (
                str.starts_with("\n") ||
                str.starts_with("\r")
                ))
                str = str.substr(0, str.length() - 2);
        };

        while (std::getline(file, line))
        {
            RemoveBlankLines(line);
            if (
                line.starts_with("#") ||
                line.starts_with("[")
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
                    str.starts_with(" ") ||
                    str.starts_with("\t")
                    ))
                    str = str.substr(1);

                while (
                    !str.empty() && (
                    str.starts_with(" ") ||
                    str.starts_with("\t")
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
                loggerConfig.warning("Configuration Reload: Key already exists: {}={}, now overriding with new value {}", key, out[key], value);
            }

            out[key] = value;
        }
    }

    void Config::SaveINI(std::ofstream& file, const std::unordered_map<std::string, std::string>& config) const
    {
        std::shared_lock lock_guard(globalConfigLock);
        for (auto pair: config)
        {
            std::string line = pair.first + " = " + pair.second;
            file << line << "\n";
        }
    }

    void Config::PrintAllConfigurations() const
    {
        std::shared_lock lock_guard(globalConfigLock);
        Logger logger("PrintAllConfiguration");
        logger.warning("Engine Configuration");
        for (auto const &config: engineConfigs)
        {
            logger.warning("{}\t=\t{}", config.first, config.second);
        }
        logger.warning("Game Configuration");
        for (auto const &config: gameConfigs)
        {
            logger.warning("{}\t=\t{}", config.first, config.second);
        }
    }
}
