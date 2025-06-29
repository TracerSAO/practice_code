#include "App.hpp"

#include <mutex>

#include "SDL/SDL.hpp"

constexpr auto WINDOW_WIDTH{1080};
constexpr auto WINDOW_HEIGHT{720};

std::mutex g_mutex;
std::shared_ptr<App> g_app;

void App::Create()
{
    std::lock_guard<std::mutex> GUARD{g_mutex};
    if (nullptr == g_app) {
        g_app = std::make_shared<App>();
        g_app->init();
    }
}

void App::Destory()
{
    std::lock_guard<std::mutex> GUARD{g_mutex};
    if (nullptr != g_app) {
        g_app.reset();
    }
}

App &App::GetInstance()
{
    std::lock_guard<std::mutex> GUARD{g_mutex};
    return *g_app;
}

void App::init()
{
    window_ = SDL::Meta<SDL_Window>::create("hello OpenGL", WINDOW_WIDTH, WINDOW_HEIGHT,
                                            SDL_WINDOW_HIGH_PIXEL_DENSITY);

    renderer_ = SDL::Meta<SDL_Renderer>::create(window_.get(), nullptr);
}

void App::render()
{
    SDL_RenderPresent(renderer_.get());
}
