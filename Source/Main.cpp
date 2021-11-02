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
#include "Engine/Engine.h"

int main(int argc, char* argv[])
{
    bool isServer = false;

    if (argc > 1 && strcmp(argv[1], "server") == 0)
    {
        isServer = true;
    }

    if (isServer)
    {
        DEBUG_LOG("NET", LOG_MSG, "Initializing Server...");
    }
    else
    {
        DEBUG_LOG("NET", LOG_MSG, "Initializing Client...");
    }

    SDL_SetMainReady();

    if (enet_initialize() != 0)
    {
        fprintf(stderr, "An error occurred while initializing ENet!\n");
        return EXIT_FAILURE;
    }
    atexit(enet_deinitialize);

    Core core = Core(60, isServer ? HostType::SERVER : HostType::CLIENT);

    core.Create();
    core.Run();
    core.Destroy();

    return 0;
}