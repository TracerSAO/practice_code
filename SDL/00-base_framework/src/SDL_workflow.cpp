#include "SDL_workflow.hpp"

#include <stdexcept>

#include <SDL3/SDL.h>

#include "App.hpp"

using namespace SDL;

SDL_AppResult SDL_Workflow::onInit(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    try {
        App::Create();
    }
    catch (const std::runtime_error &error) {
        return SDL_AppResult::SDL_APP_FAILURE;
    }
    catch (...) {
        return SDL_AppResult::SDL_APP_FAILURE;
    }

    return SDL_AppResult::SDL_APP_CONTINUE;
}

void SDL_Workflow::onQuit(SDL_AppResult result)
{
    (void)result;

    App::Destory();
}

SDL_AppResult SDL_Workflow::onUpdate()
{
    App::Render();

    return SDL_AppResult::SDL_APP_CONTINUE;
}

SDL_AppResult SDL_Workflow::onEvent(SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_EVENT_QUIT: {
            return SDL_APP_SUCCESS;
        }
    }

    return SDL_AppResult::SDL_APP_CONTINUE;
}
