/// <summary>
/// The logger verbose
/// -1 = Nothing.
///  0 = Msgs only.
///  1 = Msgs + errors.
///  2 = Msgs + errors + warnings.
///  3 = Msgs + errors + warnings + debug.
/// </summary>
#define LOGGER_VERBOSE 3

#include <iostream>
#include "raylib.h"
#include "Engine/Core/Core.h"

int main(void)
{
    Core core = Core(60);
    core.Create();
    core.Run();
    core.Destroy();

    return 0;
}