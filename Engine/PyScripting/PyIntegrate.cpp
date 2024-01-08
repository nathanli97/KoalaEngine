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

#include "PyScripting/PyInclude.h"
#include "PyScripting/PyIntegrate.h"

#include <spdlog/spdlog.h>

#include <filesystem>
#include <optional>
#include <utility>

#include "FileSystem/SystemPath.h"
namespace Koala::Scripting
{
    std::optional<std::string> FindScriptFile(const std::string& in_path)
    {
        spdlog::debug("Finding script {}", in_path.c_str());
        const auto paths = Path::GetScriptLoadPaths();
        for (auto path: paths)
        {

            if (!path.empty())
                path.append("/");
            path.append(in_path);

            if (!std::filesystem::exists(path))
            {
                spdlog::debug("Script {} is not found at {}, considering next path", in_path.c_str(), path);
                continue;
            }

            spdlog::debug("Script found: {}", in_path.c_str());
            return path;
        }
        return {};
    }
    void *LoadScriptFromDisk(const std::string& path)
    {
        const auto found_script = FindScriptFile(path);
        if (!found_script.has_value())
        {
            spdlog::warn("Load script {} failed. Considered paths: ", path);
            for (auto &p : Path::GetScriptLoadPaths())
            {
                spdlog::warn("{}", p.c_str());
            }
            spdlog::warn("ENV RootDir = {}", Path::GetRootPath());
            return nullptr;
        }

        auto& script_path = found_script.value();

        PyObject *name = PyUnicode_DecodeFSDefault(script_path.c_str());
        PyObject *module = PyImport_Import(name);
        if (module == nullptr)
        {
            spdlog::error("Failed to load script: {}", script_path.c_str());
            PyErr_Print();
        }

        Py_DECREF(name);
        return module;
    }

    void UnloadScript(void* program)
    {
        auto *proc = static_cast<PyObject*>(program);
        if (!proc) return;

        Py_DECREF(proc);
    }

    void Initialize()
    {
        Py_Initialize();

        PyConfig config;
        PyConfig_InitPythonConfig(&config);
        config.module_search_paths_set = 1;
        PyWideStringList_Append(&config.module_search_paths, L".");
        Py_InitializeFromConfig(&config);
    }
}
