#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_main.h>

#include "SDL_workflow.hpp"

extern "C" {

SDL_AppResult SDLCALL SDL_AppInit(void **appstate, int argc, char *argv[])
{
    (void)appstate;
    (void)argc;
    (void)argv;
    return SDL::SDL_Workflow::onInit(argc, argv);
}

void SDLCALL SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    (void)appstate;
    (void)result;
    SDL::SDL_Workflow::onQuit(result);
}

SDL_AppResult SDLCALL SDL_AppIterate(void *appstate)
{
    (void)appstate;
    return SDL::SDL_Workflow::onUpdate();
}

SDL_AppResult SDLCALL SDL_AppEvent(void *appstate, SDL_Event *event)
{
    (void)appstate;
    (void)event;
    return SDL::SDL_Workflow::onEvent(event);
}

} // extern "C"