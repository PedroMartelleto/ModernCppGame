/// <summary>
/// The logger verbose
/// -1 = Nothing.
///  0 = Msgs only.
///  1 = Msgs + errors.
///  2 = Msgs + errors + warnings.
///  3 = Msgs + errors + warnings + debug.
/// </summary>
#define LOGGER_VERBOSE 3

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define SDL_MAIN_HANDLED
#include "SDL2/SDL.h"

#include <iostream>
#include "Engine/Core/Core.h"

int main(int argc, char* argv[])
{
    SDL_SetMainReady();

    Core core = Core(60);

    core.Create();
    core.Run();
    core.Destroy();

    return 0;
}