#include "App.hpp"

#include "SDL/SDL.hpp"

constexpr auto WINDOW_WIDTH{1080};
constexpr auto WINDOW_HEIGHT{720};

std::shared_ptr<App> g_app;

void App::Create()
{
    if (nullptr == g_app) {
        g_app = std::make_shared<App>();
        g_app->init();
    }
}

void App::Destory()
{
    if (nullptr != g_app) {
        g_app.reset();
    }
}

void App::Render()
{
    g_app->render();
}

void App::init()
{
    window_ = SDL::Meta<SDL_Window>::create("hello OpenGL",
                                            WINDOW_WIDTH,
                                            WINDOW_HEIGHT,
                                            SDL_WINDOW_HIGH_PIXEL_DENSITY
                                                | SDL_WINDOW_OPENGL
                                                | SDL_WINDOW_VULKAN);

    gpu_device_ = SDL::Meta<SDL_GPUDevice>::create(SDL_GPU_SHADERFORMAT_SPIRV
                                                        | SDL_GPU_SHADERFORMAT_DXIL,
                                                    false,
                                                    nullptr);


    return;
}

void App::render()
{
}
