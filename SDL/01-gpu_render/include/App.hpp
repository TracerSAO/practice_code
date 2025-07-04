#pragma once

#include <memory>

#include <SDL3/SDL.h>

class App
{
public:
    App();
    ~App();
    App(App &&) = delete;
    App(const App &) = delete;
    App &operator=(App &&) = delete;
    App &operator=(const App &) = delete;

    static void Create();
    static void Destory();
    static void Render();

private:
    void initGpuPiepline();
    void initGpuVertexBuffer();

    void render();

private:
    std::shared_ptr<SDL_Window> window_;
    std::shared_ptr<SDL_GPUDevice> gpu_device_;
    std::shared_ptr<SDL_GPUGraphicsPipeline> gpu_pipeline_;

    std::shared_ptr<SDL_GPUBuffer> gpu_vertex_buffer_;
    std::shared_ptr<SDL_GPUTransferBuffer> gpu_transfer_buffer_;
};