#include <iostream>
#define PY_SSIZE_T_CLEAN
#include "src/PyIntegrate.h"

int main()
{
    PyIntegrateInitialize();

    void *init_script = PyIntegrateLoadScriptFromDisk("init.py");

    if (init_script)
    {
        PyIntegrateRunNoArg(init_script, "on_init");
    }

    return 0;
}
