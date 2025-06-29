#pragma once

#include <SDL3/SDL_init.h>

namespace SDL {

class SDL_Workflow
{
public:
    static SDL_AppResult onInit(int argc, char *argv[]);
    static void          onQuit(SDL_AppResult result);
    static SDL_AppResult onUpdate();
    static SDL_AppResult onEvent(SDL_Event *event);

private:
    SDL_Workflow() = default;
    ~SDL_Workflow() = default;
    SDL_Workflow(const SDL_Workflow &) = default;
    SDL_Workflow(SDL_Workflow &&) = default;
    SDL_Workflow &operator=(const SDL_Workflow &) = default;
    SDL_Workflow &operator=(SDL_Workflow &&) = default;
};

} // namespace SDL