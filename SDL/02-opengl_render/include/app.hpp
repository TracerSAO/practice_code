#pragma once

#include <memory>

#include <SDL3/SDL.h>

#include "opengl/gl.hpp"
#include "SDL/SDL.hpp"

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
};