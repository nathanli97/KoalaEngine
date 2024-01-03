#include <iostream>
#define PY_SSIZE_T_CLEAN
#include <spdlog/spdlog.h>

#include "Core.h"

int main()
{
    PyIntegrateInitialize();

    void *init_script = PyLoadScriptFromDisk("init.py");

    if (init_script)
    {
        PyIntegrateRunNoArg(init_script, "on_init");
    }
    else
    {
        spdlog::warn("Init script not found. The engine may not working.");
    }

    return 0;
}
