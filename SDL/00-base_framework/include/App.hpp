#pragma once

#include <memory>

#include <SDL3/SDL.h>

class App
{
public:
    App() = default;
    ~App() = default;
    App(App &&) = delete;
    App(const App &) = delete;
    App &operator=(App &&) = delete;
    App &operator=(const App &) = delete;

    static void Create();
    static void Destory();
    static void Render();

private:
    void init();
    void render();

private:
    std::shared_ptr<SDL_Window> window_;
    std::shared_ptr<SDL_Renderer> renderer_;
};