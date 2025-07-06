#define SDL_MAIN_USE_CALLBACKS 1

#include <SDL3/SDL_main.h>

#include <format>
#include <stdexcept>

#include "app.hpp"

extern "C" {

SDL_AppResult SDLCALL SDL_AppInit(void **appstate, int argc, char *argv[])
{
    (void)appstate;
    (void)argc;
    (void)argv;

    try {
        App::Create();
    }
    catch (const std::runtime_error &error) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", std::format(">>> {}", error.what()).c_str());
        return SDL_AppResult::SDL_APP_FAILURE;
    }
    catch (...) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", std::format("unknow error >>> {}", SDL_GetError()).c_str());
        return SDL_AppResult::SDL_APP_FAILURE;
    }

    return SDL_AppResult::SDL_APP_CONTINUE;
}

void SDLCALL SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    (void)appstate;
    (void)result;

    App::Destory();
}

SDL_AppResult SDLCALL SDL_AppIterate(void *appstate)
{
    (void)appstate;

    App::Render();

    return SDL_AppResult::SDL_APP_CONTINUE;
}

SDL_AppResult SDLCALL SDL_AppEvent(void *appstate, SDL_Event *event)
{
    (void)appstate;

    switch (event->type)
    {
        case SDL_EVENT_QUIT: {
            return SDL_APP_SUCCESS;
        }
        default: {
            return SDL_AppResult::SDL_APP_CONTINUE;
        }
    }
}

} // extern "C"