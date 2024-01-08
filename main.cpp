#include <iostream>
#define PY_SSIZE_T_CLEAN
#include <spdlog/spdlog.h>

#include "Core.h"

int main()
{
    spdlog::set_level(spdlog::level::debug);
    using namespace Koala;
    Scripting::Initialize();

    void *init_script = Scripting::LoadScriptFromDisk("init");

    if (init_script)
    {
        spdlog::debug("Executing init script");
        Scripting::ExecuteFunctionNoArg(init_script, "on_init");
        Scripting::UnloadScript(init_script);
    }
    else
    {
        spdlog::warn("Init script load failed. the engine may not working.");
    }


    return 0;
}
